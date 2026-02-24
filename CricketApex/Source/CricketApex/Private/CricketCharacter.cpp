#include "CricketCharacter.h"
#include "Ball.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"

ACricketCharacter::ACricketCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Setup capsule component
    GetCapsuleComponent()->SetCapsuleSize(40.0f, 90.0f);

    // Setup spring arm
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 300.0f;
    SpringArmComponent->SetRelativeRotation(FRotator(-30.0f, 0.0f, 0.0f));
    SpringArmComponent->bUsePawnControlRotation = true;
    SpringArmComponent->bEnableCameraLag = true;
    SpringArmComponent->CameraLagSpeed = 10.0f;

    // Setup camera
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;

    // Setup bat component
    BatComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bat"));
    BatComponent->SetupAttachment(RootComponent);
    BatComponent->SetRelativeLocationAndRotation(
        FVector(30.0f, 20.0f, -40.0f),
        FRotator(0.0f, 0.0f, -45.0f)
    );

    // Setup helmet component
    HelmetComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helmet"));
    HelmetComponent->SetupAttachment(RootComponent);
    HelmetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));

    // Initialize defaults
    MaxBowlingSpeed = 160.0f;
    MinBowlingSpeed = 80.0f;
    MaxSpinRate = 300.0f;
    PerfectTimingWindow = 0.15f;
    GoodTimingWindow = 0.3f;

    // Initialize batting
    TotalRuns = 0;
    BallsFaced = 0;
    Dismissals = 0;
    BattingSkill = 50.0f;

    // Initialize bowling
    WicketsTaken = 0;
    BallsBowled = 0;
    RunsConceded = 0;
    BowlingSkill = 50.0f;
    BallWear = 0.0f;
    bReverseSwingEnabled = false;

    // Auto possess player
    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ACricketCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Initialize ball arrival time
    BallArrivalTime = 0.0f;
    bBallInPlay = false;

    // Set initial state
    BatterState = EBatterState::Idle;
    IsOnStrike = false;
}

void ACricketCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bBallInPlay)
    {
        // Track ball timing
        float CurrentTime = GetGameTimeSinceCreation();
        float TimeUntilArrival = BallArrivalTime - CurrentTime;

        // Update shot execution timing
        if (TimeUntilArrival > 0.0f && TimeUntilArrival < PerfectTimingWindow)
        {
            // Player is in the timing window - execute shot
            if (BatterState == EBatterState::Ready)
            {
                BatterState = EBatterState::InShot;
            }
        }
    }

    // Update run-up progress for bowler
    if (bInRunUp)
    {
        RunUpProgress += DeltaTime * 2.0f; // Run-up speed
        if (RunUpProgress >= 1.0f)
        {
            ReleaseBall();
        }
    }
}

void ACricketCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Input bindings would be added here for Enhanced Input System
    // This is handled through the CricketPlayerController
}

// ==================== BATTING SYSTEM ====================

void ACricketCharacter::PrepareForDelivery()
{
    BatterState = EBatterState::Ready;
    bBallInPlay = false;

    // Calculate when the ball will arrive based on delivery speed
    if (CurrentDelivery)
    {
        float Distance = FVector::Dist(GetActorLocation(), CurrentDelivery->GetActorLocation());
        float Speed = CurrentDelivery->GetVelocity().Size();
        if (Speed > 0)
        {
            BallArrivalTime = GetGameTimeSinceCreation() + (Distance / Speed);
        }
    }
}

FShotInfo ACricketCharacter::ExecuteShot(const FDeliveryInfo& Delivery, const FVector2D& Footwork, const FVector2D& Timing)
{
    FShotInfo Shot;
    
    // Calculate timing
    float TimingValue = CalculateTiming(Timing, BallArrivalTime - GetGameTimeSinceCreation());
    Shot.Timing = TimingValue;
    
    // Calculate direction based on footwork and timing
    Shot.Direction = CalculateShotDirection(Footwork, Timing);
    
    // Calculate power
    Shot.Power = CalculateShotPower(Timing);
    
    // Calculate impact location
    Shot.ImpactLocation = GetActorLocation() + (Shot.Direction * 50.0f);
    
    // Determine runs based on shot characteristics
    UpdateShotResult(Shot, Delivery);
    
    // Update stats
    BallsFaced++;
    TotalRuns += Shot.Runs;
    
    // Play shot event
    OnShotPlayed.Broadcast(Shot);
    
    // Reset state
    BatterState = EBatterState::Played;
    bBallInPlay = false;
    
    return Shot;
}

FVector ACricketCharacter::CalculateShotDirection(FVector2D Footwork, FVector2D Timing)
{
    // Pure Shot System: Footwork determines shot type, Timing determines direction
    FVector BaseDirection = GetActorForwardVector();
    FVector RightDirection = GetActorRightVector();
    
    // Footwork affects horizontal angle
    float HorizontalAngle = Footwork.X * 90.0f; // -90 (cover) to +90 (leg)
    
    // Timing affects vertical angle (shot height)
    float VerticalAngle = Timing.Y * 45.0f; // 0 (drive) to 45 (pull)
    
    // Calculate final direction
    FVector Direction = BaseDirection.RotateAngleAxis(HorizontalAngle, FVector::UpVector);
    Direction = Direction.RotateAngleAxis(VerticalAngle, RightDirection);
    
    return Direction.GetSafeNormal();
}

float ACricketCharacter::CalculateShotPower(FVector2D Timing)
{
    // Power is based on how deep the timing input is pushed
    float Power = Timing.Size();
    
    // Add skill factor
    Power *= (50.0f + BattingSkill) / 100.0f;
    
    return FMath::Clamp(Power, 0.0f, 1.0f);
}

float ACricketCharacter::CalculateTiming(FVector2D Timing, float BallArrivalTime)
{
    // Perfect timing is when player inputs at the exact moment ball arrives
    float CurrentTime = GetGameTimeSinceCreation();
    float TimeDifference = FMath::Abs(BallArrivalTime - CurrentTime);
    
    float TimingValue = 1.0f - (TimeDifference / PerfectTimingWindow);
    TimingValue = FMath::Clamp(TimingValue, 0.0f, 1.0f);
    
    return TimingValue;
}

void ACricketCharacter::UpdateShotResult(FShotInfo& Shot, const FDeliveryInfo& Delivery)
{
    // Calculate shot quality
    float ShotQuality = Shot.Timing * Shot.Power;
    
    // Check for edge shots (The "Edge" System)
    bool bIsEdge = CheckEdgeShot(Shot.Timing, Delivery.HorizontalAngle, Delivery.VerticalAngle);
    
    if (bIsEdge)
    {
        // Edge shots have reduced power and unpredictable direction
        Shot.Power *= 0.3f;
        Shot.Direction = Shot.Direction.RotateAngleAxis(FMath::RandRange(-45, 45), FVector::UpVector);
        Shot.Runs = FMath::RandRange(0, 2); // Usually 0-2 runs
        Shot.IsBoundary = false;
        Shot.IsSix = false;
        
        UE_LOG(LogCricket, Warning, TEXT("Edge shot! Unlucky for the batsman"));
    }
    else if (ShotQuality > 0.8f)
    {
        // Perfect timing and good power = six
        Shot.IsSix = true;
        Shot.IsBoundary = true;
        Shot.Runs = 6;
    }
    else if (ShotQuality > 0.5f)
    {
        // Good shot = four
        Shot.IsSix = false;
        Shot.IsBoundary = true;
        Shot.Runs = 4;
    }
    else if (ShotQuality > 0.3f)
        {
        // Average shot = 1-3 runs
        Shot.IsSix = false;
        Shot.IsBoundary = false;
        Shot.Runs = FMath::RandRange(1, 3);
    }
    else
    {
        // Poor shot - likely to be caught or LBW
        Shot.Runs = 0;
    }
    
    // Adjust for delivery type
    if (Delivery.BallType == EBallType::Yorker)
    {
        // Yorker is hard to score off
        Shot.Runs = FMath::Max(0, Shot.Runs - 2);
    }
    else if (Delivery.BallType == EBallType::Bouncer)
    {
        // Bouncer can be pulled for runs
        if (FMath::RandRange(0, 100) < 30)
        {
            Shot.Runs += 2; // Bonus for playing pull shot
        }
    }
}

bool ACricketCharacter::CheckEdgeShot(float Timing, float BallX, float BallZ)
{
    // Edge occurs when timing is slightly off but not completely missed
    float EdgeThreshold = 0.2f;
    
    // Check if ball is in the "edge zone" (outside perfect timing)
    bool bNearEdge = (Timing > EdgeThreshold && Timing < (1.0f - EdgeThreshold));
    
    // Check if ball is in the danger zone (edge of bat)
    float DangerZoneWidth = 0.1f;
    bool bInDangerZone = FMath::Abs(BallX) < DangerZoneWidth;
    
    return bNearEdge && bInDangerZone;
}

EDismissalType ACricketCharacter::CheckDismissal(const FDeliveryInfo& Delivery, const FVector& ImpactLocation)
{
    // Check various dismissal conditions
    
    // Bowled - ball hits wicket
    FVector WicketLocation = GetActorLocation() + FVector(0, 0, 70); // Wicket height
    if (FVector::Dist(ImpactLocation, WicketLocation) < 20.0f)
    {
        return EDismissalType::Bowled;
    }
    
    // LBW - ball hits pads in line with wicket
    // Simplified check - in full implementation would use complex geometry
    if (Delivery.VerticalAngle < 15.0f && FMath::RandRange(0, 100) < 20)
    {
        return EDismissalType::LBW;
    }
    
    // Caught - ball hits bat and goes directly to fielder
    if (ImpactLocation.Z > 100.0f && FMath::RandRange(0, 100) < 10)
    {
        return EDismissalType::Caught;
    }
    
    return EDismissalType::Bowled; // Default to bowled
}

void ACricketCharacter::HandleDismissal(EDismissalType Dismissal)
{
    BatterState = EBatterState::Dismissed;
    Dismissals++;
    
    OnDismissal.Broadcast(Dismissal);
    
    UE_LOG(LogCricket, Log, TEXT("Player %s dismissed: %d"), *PlayerName, (int32)Dismissal);
}

// ==================== BOWLING SYSTEM ====================

FDeliveryInfo ACricketCharacter::PrepareDelivery()
{
    FDeliveryInfo Delivery;
    
    // Initialize bowling state
    bInRunUp = true;
    RunUpProgress = 0.0f;
    
    return Delivery;
}

void ACricketCharacter::ReleaseBall()
{
    bInRunUp = false;
    RunUpProgress = 0.0f;
    
    // Calculate delivery based on inputs
    FDeliveryInfo Delivery = CalculateDelivery(0.5f, FVector2D(0, 0), 0.0f);
    
    // Emit event
    OnBallDelivered.Broadcast(Delivery);
    
    // In full implementation, this would spawn a Ball actor
    UE_LOG(LogCricket, Log, TEXT("Ball released at %f mph"), Delivery.Speed);
}

void ACricketCharacter::SetBowlerRunUp()
{
    bInRunUp = true;
    RunUpProgress = 0.0f;
}

void ACricketCharacter::CompleteFollowThrough()
{
    // Animation follow-through complete
    // Update ball wear
    BallWear = FMath::Clamp(BallWear + 0.01f, 0.0f, 1.0f);
    
    // Enable reverse swing after 40 overs (or 15 in T20)
    if (BallWear > 0.7f)
    {
        bReverseSwingEnabled = true;
    }
}

FDeliveryInfo ACricketCharacter::CalculateDelivery(float PaceControl, FVector2D SpinControl, float WristAngle)
{
    FDeliveryInfo Delivery;
    
    // Fingerprint Bowling System
    
    // Pace Control (Left Trigger) - determines speed
    // Gentle squeeze = seam up, jam trigger = bouncer
    float SpeedRange = MaxBowlingSpeed - MinBowlingSpeed;
    Delivery.Speed = MinBowlingSpeed + (PaceControl * SpeedRange);
    
    // Swing calculation
    Delivery.Swing = CalculateSwing(PaceControl, BallWear);
    
    // Spin calculation
    Delivery.Spin = CalculateSpin(SpinControl, WristAngle);
    
    // Ball type based on vertical angle
    if (Delivery.VerticalAngle < 10.0f)
    {
        Delivery.BallType = EBallType::Yorker;
    }
    else if (Delivery.VerticalAngle > 40.0f)
    {
        Delivery.BallType = EBallType::Bouncer;
    }
    else if (Delivery.VerticalAngle > 80.0f)
    {
        Delivery.BallType = EBallType::FullToss;
    }
    else
    {
        Delivery.BallType = EBallType::Normal;
    }
    
    // Reverse swing
    if (bReverseSwingEnabled && Delivery.Speed > 130.0f)
    {
        Delivery.Swing += 0.5f; // Add extra swing for reverse
    }
    
    // Deviation based on spin
    Delivery.Deviation = Delivery.Spin * 0.1f;
    
    // Set locations
    Delivery.StartLocation = GetActorLocation();
    Delivery.EndLocation = Delivery.StartLocation + (GetActorForwardVector() * 2200.0f); // 22 yards
    
    return Delivery;
}

float ACricketCharacter::CalculateSwing(float PaceControl, float BallWear)
{
    // Conventional swing
    float Swing = 0.0f;
    
    // New ball swings more
    float BallFactor = 1.0f - BallWear;
    
    // Reverse swing kicks in when ball is old and bowler bowls fast
    if (BallWear > 0.7f && PaceControl > 0.7f)
    {
        Swing = 0.5f * BallFactor * (BallWear - 0.7f) * 3.0f; // Reverse swing
    }
    else
    {
        // Conventional swing
        Swing = 0.3f * BallFactor;
    }
    
    return Swing;
}

float ACricketCharacter::CalculateSpin(FVector2D SpinControl, float WristAngle)
{
    // Spin is controlled by right analog stick and wrist angle
    float SpinAmount = 0.0f;
    
    // Stick rotation determines spin type
    float StickAngle = FMath::Atan2(SpinControl.Y, SpinControl.X);
    
    // Off-spin (clockwise rotation)
    if (StickAngle > 0)
    {
        SpinAmount = StickAngle * MaxSpinRate;
    }
    // Leg-spin (counter-clockwise rotation)
    else
    {
        SpinAmount = StickAngle * MaxSpinRate;
    }
    
    // Wrist angle adds extra spin
    SpinAmount += WristAngle * 50.0f;
    
    return SpinAmount;
}

// ==================== INPUT ====================

void ACricketCharacter::SetBattingInput(FVector2D Footwork, FVector2D Timing, bool bInDefensive)
{
    CurrentFootwork = Footwork;
    CurrentTiming = Timing;
    bIsDefensive = bInDefensive;
    
    if (bIsDefensive)
    {
        BatterState = EBatterState::Defending;
    }
}

void ACricketCharacter::SetBowlingInput(float PaceControl, FVector2D SpinControl, float WristAngle)
{
    // Store bowling inputs for release
    // Actual calculation happens in ReleaseBall
}

// ==================== ANIMATION ====================

void ACricketCharacter::PlayBattingAnimation(UAnimMontage* Montage, bool bOnStrike)
{
    if (Montage && GetMesh())
    {
        float Duration = GetMesh()->GetAnimInstance()->Montage_Play(Montage);
        
        UE_LOG(LogCricket, Log, TEXT("Playing batting animation: %s, Duration: %f"), 
            *Montage->GetName(), Duration);
    }
}

void ACricketCharacter::PlayBowlingAnimation(UAnimMontage* Montage)
{
    if (Montage && GetMesh())
    {
        float Duration = GetMesh()->GetAnimInstance()->Montage_Play(Montage);
        
        UE_LOG(LogCricket, Log, TEXT("Playing bowling animation: %s, Duration: %f"), 
            *Montage->GetName(), Duration);
    }
}

// ==================== STATS ====================

float ACricketCharacter::GetBattingAverage() const
{
    if (Dismissals > 0)
    {
        return (float)TotalRuns / (float)Dismissals;
    }
    return TotalRuns; // Not out
}

float ACricketCharacter::GetBowlingAverage() const
{
    if (WicketsTaken > 0)
    {
        return (float)RunsConceded / (float)WicketsTaken;
    }
    return 0.0f;
}

float ACricketCharacter::GetStrikeRate() const
{
    if (BallsFaced > 0)
    {
        return ((float)TotalRuns / (float)BallsFaced) * 100.0f;
    }
    return 0.0f;
}

float ACricketCharacter::GetEconomy() const
{
    if (BallsBowled > 0)
    {
        float Overs = (float)BallsBowled / 6.0f;
        return (float)RunsConceded / Overs;
    }
    return 0.0f;
}
