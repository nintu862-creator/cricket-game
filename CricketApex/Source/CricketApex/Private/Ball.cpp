#include "Ball.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"

ABall::ABall()
{
    PrimaryActorTick.bCanEverTick = true;

    // Setup ball mesh
    BallMesh = CreateDefaultSubobject<USphereComponent>(TEXT("BallMesh"));
    BallMesh->SetSphereRadius(BallRadius);
    BallMesh->SetMassOverride(BallMass, true);
    BallMesh->SetSimulatePhysics(true);
    BallMesh->SetEnableGravity(true);
    BallMesh->SetLinearDamping(0.01f);
    BallMesh->SetAngularDamping(0.1f);
    BallMesh->SetRestitution(Bounciness);
    BallMesh->SetFriction(Friction);
    BallMesh->SetNotifyRigidBodyCollision(true);
    BallMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    BallMesh->SetCollisionObjectType(ECC_PhysicsBody);
    BallMesh->SetCollisionResponseToAllChannels(ECR_Block);
    BallMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
    RootComponent = BallMesh;

    // Setup physics constraint (for soft body simulation)
    PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
    PhysicsConstraint->SetupAttachment(RootComponent);
    PhysicsConstraint->SetConstrainedComponents(BallMesh, NAME_None, BallMesh, NAME_None);

    // Setup trail effect
    TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
    TrailEffect->SetupAttachment(RootComponent);
    TrailEffect->SetAutoActivate(false);

    // Default physics settings
    bIsInPlay = false;
    bHasPitched = false;
    CurrentSpeed = 0.0f;
    SwingAmount = 0.0f;
    SpinAmount = 0.0f;
    BallCondition = EBallType::New;
    DeliveryType = EDeliveryType::Normal;
    
    // Environment
    WindVector = FVector::ZeroVector;
    AirDensity = 1.225f;
    
    // Pitch properties
    PitchHardness = 0.5f;
    PitchMoisture = 0.3f;
    
    // Seam
    SeamAngle = 0.0f;
    SeamOrientation = 0.0f;
    
    // Initialize
    InitializeBall();
}

void ABall::BeginPlay()
{
    Super::BeginPlay();
    
    // Register hit callback
    BallMesh->OnComponentHit.AddDynamic(this, &ABall::NotifyHit);
    
    // Disable physics initially
    BallMesh->SetSimulatePhysics(false);
}

void ABall::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (bIsInPlay)
    {
        UpdatePhysics(DeltaTime);
        RecordTrajectory();
    }
}

void ABall::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& HitResult)
{
    // Check what we hit
    if (Other)
    {
        FString OtherName = Other->GetName();
        
        if (OtherName.Contains("Pitch") || OtherName.Contains("Ground"))
        {
            OnPitchContact(HitLocation);
        }
        else if (OtherName.Contains("Bat"))
        {
            FVector BatVelocity = FVector::ZeroVector; // Would be obtained from bat component
            OnBatContact(BatVelocity, HitLocation);
        }
        else if (OtherName.Contains("Wicket"))
        {
            OnWicketHit.Broadcast();
        }
    }
}

void ABall::InitializeBall()
{
    // Reset ball state
    TrajectoryPoints.Empty();
    TimeSinceLaunch = 0.0f;
    bIsInPlay = false;
    bHasPitched = false;
    CurrentSpeed = 0.0f;
    SwingAmount = 0.0f;
    SpinAmount = 0.0f;
    CurrentSpin = FVector::ZeroVector;
    bReverseSwingActive = false;
    ShineAmount = 0.0f;
    
    // Set mesh
    if (BallMesh)
    {
        BallMesh->SetSimulatePhysics(false);
        BallMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
        BallMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
    }
}

void ABall::LaunchBall(FVector Velocity, FVector Spin)
{
    InitializeBall();
    
    bIsInPlay = true;
    CurrentSpeed = Velocity.Size();
    CurrentSpin = Spin;
    
    // Enable physics
    BallMesh->SetSimulatePhysics(true);
    BallMesh->SetPhysicsLinearVelocity(Velocity);
    BallMesh->SetPhysicsAngularVelocityInDegrees(Spin);
    
    // Activate trail effect
    if (TrailEffect)
    {
        TrailEffect->Activate(true);
    }
    
    UE_LOG(LogCricket, Log, TEXT("Ball launched at speed: %f"), CurrentSpeed);
}

void ABall::UpdatePhysics(float DeltaTime)
{
    if (!BallMesh || !bIsInPlay) return;
    
    FVector Velocity = BallMesh->GetPhysicsLinearVelocity();
    FVector AngularVelocity = BallMesh->GetPhysicsAngularVelocityInDegrees();
    
    TimeSinceLaunch += DeltaTime;
    
    // Calculate aerodynamic forces
    CalculateAerodynamics();
    
    // Apply Magnus effect for spin
    ApplyMagnusEffect();
    
    // Apply seam effect
    ApplySeamEffect();
    
    // Update current speed
    CurrentSpeed = Velocity.Size();
    
    // Check if ball is dead
    if (CurrentSpeed < 5.0f && TimeSinceLaunch > 1.0f)
    {
        MarkDead();
    }
}

void ABall::CalculateAerodynamics()
{
    if (!BallMesh) return;
    
    FVector Velocity = BallMesh->GetPhysicsLinearVelocity();
    float Speed = Velocity.Size();
    
    if (Speed < 1.0f) return;
    
    // Normalize velocity
    FVector VelocityDir = Velocity.GetSafeNormal();
    
    // Drag force (opposes motion)
    float DragMagnitude = 0.5f * AirDensity * DragCoefficient * (Speed * Speed) * (BallRadius * 0.01f);
    FVector DragForce = -VelocityDir * DragMagnitude;
    
    // Apply drag
    BallMesh->AddForce(DragForce);
    
    // Wind effect
    if (!WindVector.IsNearlyZero())
    {
        FVector RelativeWind = WindVector - Velocity;
        float WindSpeed = RelativeWind.Size();
        
        if (WindSpeed > 0.5f)
        {
            // Cross-wind creates lateral movement
            FVector CrossWind = FVector::CrossProduct(RelativeWind, FVector::UpVector).GetSafeNormal();
            float LateralForce = WindSpeed * SwingFactor * 0.1f;
            BallMesh->AddForce(CrossWind * LateralForce);
        }
    }
}

void ABall::ApplyMagnusEffect()
{
    // Magnus effect: spinning ball creates lift/perpendicular force
    if (!BallMesh || CurrentSpin.Size() < 10.0f) return;
    
    FVector Velocity = BallMesh->GetPhysicsLinearVelocity();
    float Speed = Velocity.Size();
    
    if (Speed < 1.0f) return;
    
    // Calculate Magnus force direction (perpendicular to velocity and spin axis)
    FVector SpinAxis = CurrentSpin.GetSafeNormal();
    FVector MagnusDirection = FVector::CrossProduct(SpinAxis, Velocity.GetSafeNormal());
    
    // Magnus force magnitude
    float SpinRate = CurrentSpin.Size();
    float AngleOfAttack = FVector::DotProduct(Velocity.GetSafeNormal(), FVector::UpVector);
    
    // Only apply significant Magnus force when ball is not moving vertically
    if (FMath::Abs(AngleOfAttack) < 0.7f)
    {
        float MagnusMagnitude = LiftCoefficient * SpinRate * Speed * 0.001f;
        BallMesh->AddForce(MagnusDirection * MagnusMagnitude);
    }
}

void ABall::ApplySeamEffect()
{
    // Seam effect: ball pitched on seam moves laterally
    if (!BallMesh || !bHasPitched || bReverseSwingActive) return;
    
    FVector Velocity = BallMesh->GetPhysicsLinearVelocity();
    float Speed = Velocity.Size();
    
    if (Speed < 20.0f) return; // Only apply when ball is moving fast
    
    // Calculate seam deviation
    float SeamDeviation = FMath::Sin(SeamAngle) * SeamFactor * (1.0f - (PitchHardness * 0.5f));
    
    // Apply lateral force based on seam orientation
    FVector RightDir = FVector::CrossProduct(Velocity.GetSafeNormal(), FVector::UpVector);
    FVector SeamForce = RightDir * SeamDeviation * Speed * 0.1f;
    
    BallMesh->AddForce(SeamForce);
}

void ABall::ApplySwing(float Swing, float Angle)
{
    SwingAmount = Swing;
    
    // Positive swing = swings away from bowler (right-handed batsman)
    // Negative swing = swings into batsman
    
    // Reverse swing logic
    if (BallCondition == EBallType::Reversing && CurrentSpeed > 130.0f)
    {
        // Reverse swing is stronger and happens later in the ball's trajectory
        bReverseSwingActive = true;
        SwingAmount *= 2.0f; // Double swing in reverse
    }
    
    UE_LOG(LogCricket, Log, TEXT("Swing applied: %f"), SwingAmount);
}

void ABall::ApplySpin(float Spin)
{
    SpinAmount = Spin;
    CurrentSpin = FVector(0, 0, Spin);
    
    // Spin affects the ball's trajectory after pitching
    // Off-spin = ball turns from off to leg (right-handed batsman's perspective)
    // Leg-spin = ball turns from leg to off
    
    UE_LOG(LogCricket, Log, TEXT("Spin applied: %f"), SpinAmount);
}

void ABall::ApplyWind(FVector Direction, float Speed)
{
    WindVector = Direction * Speed;
}

void ABall::OnPitchContact(FVector ContactPoint)
{
    if (!bHasPitched)
    {
        bHasPitched = true;
        PitchContactPoint = ContactPoint;
        LandingPoint = ContactPoint;
        
        // Ball changes behavior after pitching
        // Add extra bounce based on pitch hardness
        float BounceFactor = 0.5f + (PitchHardness * 0.3f);
        
        FVector Velocity = BallMesh->GetPhysicsLinearVelocity();
        FVector NewVelocity = Velocity;
        NewVelocity.Z = FMath::Abs(Velocity.Z) * BounceFactor;
        
        // Add spin-induced turn after pitching
        if (SpinAmount != 0.0f)
        {
            float TurnAngle = SpinAmount * 0.001f; // Convert to angle
            NewVelocity = NewVelocity.RotateAngleAxis(TurnAngle, FVector::UpVector);
        }
        
        BallMesh->SetPhysicsLinearVelocity(NewVelocity);
        
        // Play pitch sound
        OnPitched.Broadcast();
        
        UE_LOG(LogCricket, Log, TEXT("Ball pitched at: %s"), *ContactPoint.ToString());
    }
}

void ABall::CalculateBounce(FVector HitLocation, FVector SurfaceNormal)
{
    FVector Velocity = BallMesh->GetPhysicsLinearVelocity();
    
    // Reflect velocity
    FVector ReflectedVelocity = Velocity.MirrorByVector(SurfaceNormal);
    
    // Add bounce
    ReflectedVelocity *= Bounciness;
    
    BallMesh->SetPhysicsLinearVelocity(ReflectedVelocity);
}

void ABall::OnBatContact(FVector BatVelocity, FVector ContactPoint)
{
    // Ball has been hit by bat
    FVector BallVelocity = BallMesh->GetPhysicsLinearVelocity();
    
    // Calculate result based on bat velocity and contact point
    FVector ShotVelocity = CalculateShotResult(BatVelocity, 0.5f, 0.5f);
    
    // Apply new velocity
    BallMesh->SetPhysicsLinearVelocity(ShotVelocity);
    
    // Update state
    bIsInPlay = true;
    
    // Play hit sound/effect
    OnBatHit.Broadcast();
    
    UE_LOG(LogCricket, Log, TEXT("Ball hit by bat: %s"), *ShotVelocity.ToString());
}

FVector ABall::CalculateShotResult(FVector BatVelocity, float Timing, float Power)
{
    FVector BallVelocity = BallMesh->GetPhysicsLinearVelocity();
    
    // Calculate relative velocity between bat and ball
    FVector RelativeVelocity = BatVelocity - BallVelocity;
    
    // Timing affects efficiency of shot
    float Efficiency = 0.5f + (Timing * 0.5f); // 0.5 to 1.0
    
    // Power determines shot distance
    float ShotPower = Power * 200.0f; // Scale to realistic velocities
    
    // Calculate shot direction
    FVector ShotDirection = (BallVelocity.GetSafeNormal() + RelativeVelocity.GetSafeNormal()).GetSafeNormal();
    
    // Add elevation based on timing
    float Elevation = 0.3f + (Timing * 0.2f);
    ShotDirection.Z = Elevation;
    ShotDirection = ShotDirection.GetSafeNormal();
    
    // Final velocity
    float FinalSpeed = RelativeVelocity.Size() * Efficiency + ShotPower;
    
    return ShotDirection * FinalSpeed;
}

void ABall::UpdateBallCondition(float OversBowled)
{
    // Ball gets older as overs are bowled
    float BallAge = OversBowled / 40.0f; // New ball after 40 overs in ODIs
    
    if (BallAge >= 1.0f)
    {
        BallCondition = EBallType::Old;
    }
    else if (BallAge >= 0.7f)
    {
        // Ball can reverse after 35-40 overs
        BallCondition = EBallType::Reversing;
    }
    else
    {
        BallCondition = EBallType::New;
    }
}

void ABall::ShineBall()
{
    // Shine the ball to maintain/reverse swing potential
    ShineAmount = FMath::Clamp(ShineAmount + 0.2f, 0.0f, 1.0f);
    
    // Shining one side more creates swing
    if (SeamOrientation != 0.0f)
    {
        SwingAmount += ShineAmount * 0.3f;
    }
    
    UE_LOG(LogCricket, Log, TEXT("Ball shined: %f"), ShineAmount);
}

void ABall::SetBallType(EBallType NewType)
{
    BallCondition = NewType;
}

void ABall::MarkDead()
{
    bIsInPlay = false;
    
    if (TrailEffect)
    {
        TrailEffect->Deactivate();
    }
    
    // Determine how ball went out of play
    FVector Location = GetActorLocation();
    
    // Check for boundary
    float BoundaryDistance = 3000.0f; // ~30 meters from center (typical boundary)
    if (Location.Size() > BoundaryDistance)
    {
        OnBoundary.Broadcast();
    }
    else
    {
        OnOutOfPlay.Broadcast(Location);
    }
}

void ABall::ResetBall()
{
    InitializeBall();
    
    // Reset to starting position
    SetActorLocation(FVector(0, 0, 100));
    SetActorRotation(FRotator::ZeroRotator);
}

FVector ABall::GetVelocity() const
{
    if (BallMesh)
    {
        return BallMesh->GetPhysicsLinearVelocity();
    }
    return FVector::ZeroVector;
}

float ABall::GetSpeed() const
{
    return CurrentSpeed;
}

FVector ABall::GetPredictedPath(int32 Steps) const
{
    // Simple trajectory prediction
    FVector Predicted = FVector::ZeroVector;
    
    if (BallMesh && bIsInPlay)
    {
        FVector Velocity = BallMesh->GetPhysicsLinearVelocity();
        FVector Position = BallMesh->GetComponentLocation();
        
        // Simple parabolic prediction
        for (int32 i = 0; i < Steps; i++)
        {
            float T = (float)i * 0.1f;
            Predicted = Position + (Velocity * T) + (FVector(0, 0, -980) * T * T * 0.5f);
        }
    }
    
    return Predicted;
}

float ABall::GetSwingDirection() const
{
    return SwingAmount;
}

void ABall::RecordTrajectory()
{
    if (BallMesh)
    {
        TrajectoryPoints.Add(BallMesh->GetComponentLocation());
        
        // Limit trajectory points to prevent memory issues
        if (TrajectoryPoints.Num() > 1000)
        {
            TrajectoryPoints.RemoveAt(0);
        }
    }
}
