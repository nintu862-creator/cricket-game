#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CricketCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class UAnimMontage;
class ABall;
enum class EBatterState : uint8;
enum class EBowlerType : uint8;
enum class EBallType : uint8;
enum class EDismissalType : uint8;

/**
 * EBatterState - Current state of the batter
 */
UENUM(BlueprintType)
enum class EBatterState : uint8
{
    Idle UMETA(DisplayName = "Idle"),
    Ready UMETA(DisplayName = "Ready"),
    InShot UMETA(DisplayName = "In Shot"),
    Defending UMETA(DisplayName = "Defending"),
    Played UMETA(DisplayName = "Played Shot"),
    Dismissed UMETA(DisplayName = "Dismissed")
};

/**
 * EBowlerType - Type of bowler
 */
UENUM(BlueprintType)
enum class EBowlerType : uint8
{
    Pace UMETA(DisplayName = "Pace Bowler"),
    Seam UMETA(DisplayName = "Seam Bowler"),
    OffSpin UMETA(DisplayName = "Off Spinner"),
    LegSpin UMETA(DisplayName = "Leg Spinner"),
    LeftArmSpin UMETA(DisplayName = "Left Arm Spin"),
    LeftArmPace UMETA(DisplayName = "Left Arm Pace")
};

/**
 * EBallType - Type of delivery
 */
UENUM(BlueprintType)
enum class EBallType : uint8
{
    Normal UMETA(DisplayName = "Normal"),
    Yorker UMETA(DisplayName = "Yorker"),
    Bouncer UMETA(DisplayName = "Bouncer"),
    FullToss UMETA(DisplayName = "Full Toss"),
    NoBall UMETA(DisplayName = "No Ball"),
    Wide UMETA(DisplayName = "Wide"),
    FreeHit UMETA(DisplayName = "Free Hit")
};

/**
 * EDismissalType - Type of wicket dismissal
 */
UENUM(BlueprintType)
enum class EDismissalType : uint8
{
    Bowled UMETA(DisplayName = "Bowled"),
    Caught UMETA(DisplayName = "Caught"),
    LBW UMETA(DisplayName = "LBW"),
    Stumped UMETA(DisplayName = "Stumped"),
    RunOut UMETA(DisplayName = "Run Out"),
    HitWicket UMETA(DisplayName = "Hit Wicket"),
    TimedOut UMETA(DisplayName = "Timed Out"),
    Retired UMETA(DisplayName = "Retired"),
    Obstructing UMETA(DisplayName = "Obstructing the Field"),
    Handled UMETA(DisplayName = "Handled the Ball")
};

/**
 * FDeliveryInfo - Information about a delivery
 */
USTRUCT(BlueprintType)
struct FDeliveryInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Speed = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Swing = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Spin = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Deviation = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBallType BallType = EBallType::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector StartLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector EndLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector LandingPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float VerticalAngle = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HorizontalAngle = 0.0f;
};

/**
 * FShotInfo - Information about a shot
 */
USTRUCT(BlueprintType)
struct FShotInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Direction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Power = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Timing = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector ImpactLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Runs = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsBoundary = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsSix = false;
};

/**
 * ACricketCharacter - Main character class for cricket players
 */
UCLASS()
class CRICKETAPEX_API ACricketCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACricketCharacter();

    // Character Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* BatComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* HelmetComponent;

    // Player Info
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    FString PlayerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    int32 PlayerID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    int32 TeamID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    bool IsBatsman = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    bool IsBowler = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    EBowlerType BowlerType = EBowlerType::Pace;

    // Batting Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Batting")
    int32 TotalRuns = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Batting")
    int32 BallsFaced = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Batting")
    int32 Dismissals = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Batting")
    float BattingSkill = 50.0f;

    // Bowling Stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Bowling")
    int32 WicketsTaken = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Bowling")
    int32 BallsBowled = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Bowling")
    int32 RunsConceded = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Bowling")
    float BowlingSkill = 50.0f;

    // Current State
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
    EBatterState BatterState = EBatterState::Idle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|State")
    bool IsOnStrike = false;

    // Physics Parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Physics")
    float MaxBowlingSpeed = 160.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Physics")
    float MinBowlingSpeed = 80.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Physics")
    float MaxSpinRate = 300.0f;

    // Shot Timing Window (in seconds)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Batting")
    float PerfectTimingWindow = 0.15f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Batting")
    float GoodTimingWindow = 0.3f;

    // Ball Reference
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Ball")
    ABall* CurrentDelivery;

    // Input Functions
    UFUNCTION(BlueprintCallable, Category = "Cricket|Input")
    void SetBattingInput(FVector2D Footwork, FVector2D Timing, bool bIsDefensive);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Input")
    void SetBowlingInput(float PaceControl, FVector2D SpinControl, float WristAngle);

    // Batting Functions
    UFUNCTION(BlueprintCallable, Category = "Cricket|Batting")
    void PrepareForDelivery();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Batting")
    FShotInfo ExecuteShot(const FDeliveryInfo& Delivery, const FVector2D& Footwork, const FVector2D& Timing);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Batting")
    void UpdateShotResult(FShotInfo& Shot, const FDeliveryInfo& Delivery);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Batting")
    EDismissalType CheckDismissal(const FDeliveryInfo& Delivery, const FVector& ImpactLocation);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Batting")
    void HandleDismissal(EDismissalType Dismissal);

    // Bowling Functions
    UFUNCTION(BlueprintCallable, Category = "Cricket|Bowling")
    FDeliveryInfo PrepareDelivery();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Bowling")
    void ReleaseBall();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Bowling")
    void SetBowlerRunUp();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Bowling")
    void CompleteFollowThrough();

    // Animation
    UFUNCTION(BlueprintCallable, Category = "Cricket|Animation")
    void PlayBattingAnimation(UAnimMontage* Montage, bool bIsOnStrike);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Animation")
    void PlayBowlingAnimation(UAnimMontage* Montage);

    // Getters
    UFUNCTION(BlueprintPure, Category = "Player|Stats")
    float GetBattingAverage() const;

    UFUNCTION(BlueprintPure, Category = "Player|Stats")
    float GetBowlingAverage() const;

    UFUNCTION(BlueprintPure, Category = "Player|Stats")
    float GetStrikeRate() const;

    UFUNCTION(BlueprintPure, Category = "Player|Stats")
    float GetEconomy() const;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    // Pure Shot System
    FVector CalculateShotDirection(FVector2D Footwork, FVector2D Timing);
    float CalculateShotPower(FVector2D Timing);
    float CalculateTiming(FVector2D Timing, float BallArrivalTime);
    
    // Fingerprint Bowling System
    FDeliveryInfo CalculateDelivery(float PaceControl, FVector2D SpinControl, float WristAngle);
    float CalculateSwing(float PaceControl, float BallWear);
    float CalculateSpin(FVector2D SpinControl, float WristAngle);
    
    // Hit detection
    bool CheckEdgeShot(float Timing, float BallX, float BallZ);
    FVector CalculateDeflection(const FVector& BallVelocity, const FVector& BatVelocity);

public:
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShotPlayed, FShotInfo, ShotInfo);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnShotPlayed OnShotPlayed;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDismissal, EDismissalType, DismissalType);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnDismissal OnDismissal;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWicketTaken, ACricketCharacter*, Bowler);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnWicketTaken OnWicketTaken;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBallDelivered, FDeliveryInfo, Delivery);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnBallDelivered OnBallDelivered;

private:
    // Timing state
    float BallArrivalTime = 0.0f;
    bool bBallInPlay = false;
    float ShotExecutionTime = 0.0f;
    
    // Bowling state
    bool bInRunUp = false;
    float RunUpProgress = 0.0f;
    float BallWear = 0.0f; // 0.0 = new ball, 1.0 = old ball
    bool bReverseSwingEnabled = false;
    
    // Shot state
    FVector2D CurrentFootwork;
    FVector2D CurrentTiming;
    bool bIsDefensive = false;
};
