#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Ball.generated.h"

class USphereComponent;
class UNiagaraComponent;
enum class EBallType : uint8;
enum class EDeliveryType : uint8;

/**
 * EDeliveryType - Type of delivery
 */
UENUM(BlueprintType)
enum class EDeliveryType : uint8
{
    Normal UMETA(DisplayName = "Normal"),
    Fast UMETA(DisplayName = "Fast Ball"),
    Bouncer UMETA(DisplayName = "Bouncer"),
    Yorker UMETA(DisplayName = "Yorker"),
    FullToss UMETA(DisplayName = "Full Toss"),
    NoBall UMETA(DisplayName = "No Ball"),
    Wide UMETA(DisplayName = "Wide"),
    FreeHit UMETA(DisplayName = "Free Hit")
};

/**
 * EBallType - Type of ball (new/old)
 */
UENUM(BlueprintType)
enum class EBallType : uint8
{
    New UMETA(DisplayName = "New Ball"),
    Old UMETA(DisplayName = "Old Ball"),
    Reversing UMETA(DisplayName = "Reversing")
};

/**
 * ABall - The cricket ball with realistic physics
 */
UCLASS()
class CRICKETAPEX_API ABall : public AActor
{
    GENERATED_BODY()

public:
    ABall();

    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* BallMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UPhysicsConstraintComponent* PhysicsConstraint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UNiagaraComponent* TrailEffect;

    // Ball Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Properties")
    float BallRadius = 3.67f; // cm (standard cricket ball)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Properties")
    float BallMass = 0.163f; // kg (standard cricket ball)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Properties")
    float Bounciness = 0.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Properties")
    float Friction = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Properties")
    float SwingFactor = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Properties")
    float SpinFactor = 0.5f;

    // Current State
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball|State")
    EDeliveryType DeliveryType = EDeliveryType::Normal;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball|State")
    EBallType BallCondition = EBallType::New;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball|State")
    float CurrentSpeed = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball|State")
    float SwingAmount = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball|State")
    float SpinAmount = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball|State")
    bool bIsInPlay = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball|State")
    bool bHasPitched = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball|State")
    FVector LandingPoint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ball|State")
    FVector PitchContactPoint;

    // Seam properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Seam")
    float SeamAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Seam")
    float SeamOrientation = 0.0f;

    // Wind and Atmosphere
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Environment")
    FVector WindVector = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Environment")
    float AirDensity = 1.225f; // kg/m^3 at sea level

    // Pitch Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Pitch")
    float PitchHardness = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball|Pitch")
    float PitchMoisture = 0.3f;

    // Functions
    UFUNCTION(BlueprintCallable, Category = "Ball|Launch")
    void LaunchBall(FVector Velocity, FVector Spin);

    UFUNCTION(BlueprintCallable, Category = "Ball|Physics")
    void ApplySwing(float SwingAmount, float Angle);

    UFUNCTION(BlueprintCallable, Category = "Ball|Physics")
    void ApplySpin(float SpinAmount);

    UFUNCTION(BlueprintCallable, Category = "Ball|Physics")
    void ApplyWind(FVector WindDirection, float WindSpeed);

    UFUNCTION(BlueprintCallable, Category = "Ball|Physics")
    void OnPitchContact(FVector ContactPoint);

    UFUNCTION(BlueprintCallable, Category = "Ball|Physics")
    void CalculateBounce(FVector HitLocation, FVector SurfaceNormal);

    UFUNCTION(BlueprintCallable, Category = "Ball|State")
    void UpdateBallCondition(float OversBowled);

    UFUNCTION(BlueprintCallable, Category = "Ball|State")
    void ShineBall();

    UFUNCTION(BlueprintCallable, Category = "Ball|State")
    void SetBallType(EBallType NewType);

    UFUNCTION(BlueprintCallable, Category = "Ball|Hit")
    void OnBatContact(FVector BatVelocity, FVector ContactPoint);

    UFUNCTION(BlueprintCallable, Category = "Ball|Hit")
    FVector CalculateShotResult(FVector BatVelocity, float Timing, float Power);

    UFUNCTION(BlueprintCallable, Category = "Ball|State")
    void MarkDead();

    UFUNCTION(BlueprintCallable, Category = "Ball|State")
    void ResetBall();

    // Getters
    UFUNCTION(BlueprintPure, Category = "Ball|State")
    FVector GetVelocity() const;

    UFUNCTION(BlueprintPure, Category = "Ball|State")
    float GetSpeed() const;

    UFUNCTION(BlueprintPure, Category = "Ball|State")
    FVector GetPredictedPath(int32 Steps = 10) const;

    UFUNCTION(BlueprintPure, Category = "Ball|State")
    float GetSwingDirection() const;

    // Event getters for trajectory visualization
    UFUNCTION(BlueprintPure, Category = "Ball|Trajectory")
    TArray<FVector> GetTrajectoryPoints() const { return TrajectoryPoints; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& HitResult) override;

private:
    void InitializeBall();
    void UpdatePhysics(float DeltaTime);
    void CalculateAerodynamics();
    void ApplyMagnusEffect();
    void ApplySeamEffect();
    void RecordTrajectory();

private:
    TArray<FVector> TrajectoryPoints;
    float TimeSinceLaunch = 0.0f;
    FVector PreviousVelocity;
    FVector CurrentSpin;
    bool bReverseSwingActive = false;
    float ShineAmount = 0.0f;

    // Magnus effect parameters
    float LiftCoefficient = 0.2f;
    float DragCoefficient = 0.47f; // Sphere drag coefficient

public:
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPitched);
    UPROPERTY(BlueprintAssignable, Category = "Ball|Events")
    FOnPitched OnPitched;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBatHit);
    UPROPERTY(BlueprintAssignable, Category = "Ball|Events")
    FOnBatHit OnBatHit;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBoundary);
    UPROPERTY(BlueprintAssignable, Category = "Ball|Events")
    FOnBoundary OnBoundary;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWicketHit);
    UPROPERTY(BlueprintAssignable, Category = "Ball|Events")
    FOnWicketHit OnWicketHit;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCatch);
    UPROPERTY(BlueprintAssignable, Category = "Ball|Events")
    FOnCatch OnCatch;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutOfPlay, FVector, Location);
    UPROPERTY(BlueprintAssignable, Category = "Ball|Events")
    FOnOutOfPlay OnOutOfPlay;
};
