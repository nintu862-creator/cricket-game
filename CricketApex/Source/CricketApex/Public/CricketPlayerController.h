#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CricketPlayerController.generated.h"

class ACricketCharacter;
class ABall;
class UInputMappingContext;
class UInputAction;
enum class EControlScheme : uint8;

/**
 * EControlScheme - Available control schemes
 */
UENUM(BlueprintType)
enum class EControlScheme : uint8
{
    ProSim UMETA(DisplayName = "Pro Sim (Default)"),
    Classic UMETA(DisplayName = "Classic"),
    Assist UMETA(DisplayName = "Assist")
};

/**
 * FCameraSettings - Camera configuration
 */
USTRUCT(BlueprintType)
struct FCameraSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FieldOfView = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Distance = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Height = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CameraLag = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEnableCameraShake = true;
};

/**
 * ECameraMode - Available camera modes
 */
UENUM(BlueprintType)
enum class ECameraMode : uint8
{
    Default UMETA(DisplayName = "Default"),
    BehindBatsman UMETA(DisplayName = "Behind Batsman"),
    BowlerView UMETA(DisplayName = "Bowler View"),
    Wide UMETA(DisplayName = "Wide"),
    Drone UMETA(DisplayName = "Drone"),
    Chase UMETA(DisplayName = "Chase Cam"),
    Stump UMETA(DisplayName = "Stump Cam")
};

/**
 * ACricketPlayerController - Handles player input for batting and bowling
 */
UCLASS()
class CRICKETAPEX_API ACricketPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ACricketPlayerController();

    // Input Mapping Contexts
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* BattingMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* BowlingMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* FieldingMappingContext;

    // Control Scheme
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Settings")
    EControlScheme CurrentControlScheme = EControlScheme::ProSim;

    // Camera Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    FCameraSettings CurrentCameraSettings;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    ECameraMode CurrentCameraMode = ECameraMode::BehindBatsman;

    // Player Character
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    ACricketCharacter* ControlledCricketCharacter;

    // Ball Reference
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Ball")
    ABall* CurrentBall;

    // Input State
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|State")
    bool bIsBatting = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|State")
    bool bIsBowling = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|State")
    bool bCanInput = false;

    // Batting Input State
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Batting")
    FVector2D FootworkInput = FVector2D::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Batting")
    FVector2D TimingInput = FVector2D::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Batting")
    bool bIsDefensive = false;

    // Bowling Input State
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Bowling")
    float PaceControl = 0.5f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Bowling")
    FVector2D SpinControl = FVector2D::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Bowling")
    float WristAngle = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Bowling")
    bool bInRunUp = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Bowling")
    float RunUpProgress = 0.0f;

    // Camera Shake
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraShakeIntensity = 1.0f;

    // Input Functions
    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetControlScheme(EControlScheme Scheme);

    UFUNCTION(BlueprintCallable, Category = "Input")
    void EnableBattingInput();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void EnableBowlingInput();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void DisableInput();

    // Camera Functions
    UFUNCTION(BlueprintCallable, Category = "Camera")
    void SetCameraMode(ECameraMode Mode);

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void CycleCamera();

    UFUNCTION(BlueprintCallable, Category = "Camera")
    void TriggerCameraShake();

    // Shot Execution
    UFUNCTION(BlueprintCallable, Category = "Cricket|Batting")
    void ExecuteShot();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Batting")
    void CancelShot();

    // Bowling Execution
    UFUNCTION(BlueprintCallable, Category = "Cricket|Bowling")
    void StartRunUp();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Bowling")
    void StopRunUp();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Bowling")
    void ReleaseBall();

    // Fielding
    UFUNCTION(BlueprintCallable, Category = "Cricket|Fielding")
    void ThrowBall();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Fielding")
    void AimThrow(FVector2D Direction);

    // UI
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowScoreboard();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideScoreboard();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void TogglePauseMenu();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupInputComponent() override;

private:
    // Input Handlers
    void HandleFootworkInput(FVector2D Value);
    void HandleTimingInput(FVector2D Value);
    void HandleDefensiveInput(bool bPressed);
    
    void HandlePaceInput(float Value);
    void HandleSpinInput(FVector2D Value);
    void HandleWristInput(float Value);
    
    void HandleCameraRotationInput(FVector2D Value);
    void HandleCameraZoomInput(float Value);
    void HandleCameraSwitchInput();

    // Helper functions
    void ApplyControlSchemeSettings();
    void UpdateCameraPosition(float DeltaTime);
    void UpdateInputState(float DeltaTime);
    FVector CalculateShotDirection();

    // Camera interpolation
    FVector TargetCameraLocation;
    FRotator TargetCameraRotation;
    float CameraInterpolationSpeed = 5.0f;
};
