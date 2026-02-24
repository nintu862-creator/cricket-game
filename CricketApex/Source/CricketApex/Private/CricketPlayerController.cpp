#include "CricketPlayerController.h"
#include "CricketCharacter.h"
#include "Ball.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ACricketPlayerController::ACricketPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;

    // Default camera settings
    CurrentCameraSettings.FieldOfView = 90.0f;
    CurrentCameraSettings.Distance = 300.0f;
    CurrentCameraSettings.Height = 50.0f;
    CurrentCameraSettings.CameraLag = 10.0f;
    CurrentCameraSettings.bEnableCameraShake = true;

    // Input state
    bIsBatting = false;
    bIsBowling = false;
    bCanInput = false;

    // Batting state
    FootworkInput = FVector2D::ZeroVector;
    TimingInput = FVector2D::ZeroVector;
    bIsDefensive = false;

    // Bowling state
    PaceControl = 0.5f;
    SpinControl = FVector2D::ZeroVector;
    WristAngle = 0.0f;
    bInRunUp = false;
    RunUpProgress = 0.0f;

    // Camera
    CameraShakeIntensity = 1.0f;
    CameraInterpolationSpeed = 5.0f;
    CurrentCameraMode = ECameraMode::BehindBatsman;
}

void ACricketPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Get controlled character
    ControlledCricketCharacter = Cast<ACricketCharacter>(GetPawn());

    // Setup input
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        // Would load mapping contexts here
    }

    // Apply control scheme
    ApplyControlSchemeSettings();
}

void ACricketPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update camera
    if (IsValid(ControlledCricketCharacter))
    {
        UpdateCameraPosition(DeltaTime);
    }

    // Update input state
    UpdateInputState(DeltaTime);
}

void ACricketPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Get enhanced input component
    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    if (EnhancedInput)
    {
        // Batting Inputs - Pro Sim
        // Footwork (Left Stick)
        // EnhancedInput->BindAction(BattingMoveAction, ETriggerEvent::Triggered, this, &ACricketPlayerController::HandleFootworkInput);
        
        // Timing (Right Stick)  
        // EnhancedInput->BindAction(BattingTimingAction, ETriggerEvent::Triggered, this, &ACricketPlayerController::HandleTimingInput);
        
        // Defensive (X button)
        // EnhancedInput->BindAction(DefensiveAction, ETriggerEvent::Started, this, &ACricketPlayerController::HandleDefensiveInput);
        
        // Bowling Inputs - Pro Sim
        // Pace (Left Trigger - Analog)
        // EnhancedInput->BindAction(BowlPaceAction, ETriggerEvent::Triggered, this, &ACricketPlayerController::HandlePaceInput);
        
        // Spin (Right Stick)
        // EnhancedInput->BindAction(BowlSpinAction, ETriggerEvent::Triggered, this, &ACricketPlayerController::HandleSpinInput);
        
        // Wrist (Right Trigger - Analog)
        // EnhancedInput->BindAction(BowlWristAction, ETriggerEvent::Triggered, this, &ACricketPlayerController::HandleWristInput);
        
        // Camera Controls
        // EnhancedInput->BindAction(CameraRotateAction, ETriggerEvent::Triggered, this, &ACricketPlayerController::HandleCameraRotationInput);
        // EnhancedInput->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &ACricketPlayerController::HandleCameraZoomInput);
        // EnhancedInput->BindAction(CameraSwitchAction, ETriggerEvent::Triggered, this, &ACricketPlayerController::HandleCameraSwitchInput);
        
        UE_LOG(LogCricket, Log, TEXT("Input component setup complete"));
    }
}

// ==================== CONTROL SCHEME ====================

void ACricketPlayerController::SetControlScheme(EControlScheme Scheme)
{
    CurrentControlScheme = Scheme;
    ApplyControlSchemeSettings();
    
    UE_LOG(LogCricket, Log, TEXT("Control scheme changed to: %d"), (int32)Scheme);
}

void ACricketPlayerController::ApplyControlSchemeSettings()
{
    switch (CurrentControlScheme)
    {
        case EControlScheme::ProSim:
            // Full analog control
            CurrentCameraSettings.FieldOfView = 90.0f;
            CurrentCameraSettings.Distance = 300.0f;
            break;
            
        case EControlScheme::Classic:
            // Simplified timing
            CurrentCameraSettings.FieldOfView = 85.0f;
            CurrentCameraSettings.Distance = 350.0f;
            break;
            
        case EControlScheme::Assist:
            // Assisted gameplay
            CurrentCameraSettings.FieldOfView = 95.0f;
            CurrentCameraSettings.Distance = 400.0f;
            break;
    }
}

// ==================== INPUT ENABLE/DISABLE ====================

void ACricketPlayerController::EnableBattingInput()
{
    bIsBatting = true;
    bIsBowling = false;
    bCanInput = true;
    
    // Switch to batting camera
    SetCameraMode(ECameraMode::BehindBatsman);
    
    UE_LOG(LogCricket, Log, TEXT("Batting input enabled"));
}

void ACricketPlayerController::EnableBowlingInput()
{
    bIsBatting = false;
    bIsBowling = true;
    bCanInput = true;
    
    // Switch to bowler camera
    SetCameraMode(ECameraMode::BowlerView);
    
    UE_LOG(LogCricket, Log, TEXT("Bowling input enabled"));
}

void ACricketPlayerController::DisableInput()
{
    bCanInput = false;
    bIsBatting = false;
    bIsBowling = false;
    
    // Reset inputs
    FootworkInput = FVector2D::ZeroVector;
    TimingInput = FVector2D::ZeroVector;
    PaceControl = 0.5f;
    SpinControl = FVector2D::ZeroVector;
    
    UE_LOG(LogCricket, Log, TEXT("All input disabled"));
}

// ==================== BATTING INPUT ====================

void ACricketPlayerController::HandleFootworkInput(FVector2D Value)
{
    if (!bCanInput || !bIsBatting) return;
    
    FootworkInput = Value;
    
    // Apply deadzone
    if (FootworkInput.Size() < 0.1f)
    {
        FootworkInput = FVector2D::ZeroVector;
    }
    
    // Send to character
    if (ControlledCricketCharacter)
    {
        ControlledCricketCharacter->SetBattingInput(FootworkInput, TimingInput, bIsDefensive);
    }
}

void ACricketPlayerController::HandleTimingInput(FVector2D Value)
{
    if (!bCanInput || !bIsBatting) return;
    
    TimingInput = Value;
    
    // Apply deadzone
    if (TimingInput.Size() < 0.1f)
    {
        TimingInput = FVector2D::ZeroVector;
    }
    
    // Send to character
    if (ControlledCricketCharacter)
    {
        ControlledCricketCharacter->SetBattingInput(FootworkInput, TimingInput, bIsDefensive);
    }
}

void ACricketPlayerController::HandleDefensiveInput(bool bPressed)
{
    if (!bCanInput || !bIsBatting) return;
    
    bIsDefensive = bPressed;
    
    if (ControlledCricketCharacter)
    {
        ControlledCricketCharacter->SetBattingInput(FootworkInput, TimingInput, bIsDefensive);
    }
}

void ACricketPlayerController::ExecuteShot()
{
    if (!bCanInput || !bIsBatting) return;
    
    if (ControlledCricketCharacter && CurrentBall)
    {
        // Get delivery info
        // Execute shot based on current inputs
        // ControlledCricketCharacter->ExecuteShot(Delivery, FootworkInput, TimingInput);
        
        UE_LOG(LogCricket, Log, TEXT("Shot executed: Footwork=%s, Timing=%s"), 
            *FootworkInput.ToString(), *TimingInput.ToString());
    }
}

void ACricketPlayerController::CancelShot()
{
    if (!bCanInput || !bIsBatting) return;
    
    FootworkInput = FVector2D::ZeroVector;
    TimingInput = FVector2D::ZeroVector;
    
    if (ControlledCricketCharacter)
    {
        ControlledCricketCharacter->SetBattingInput(FootworkInput, TimingInput, false);
    }
}

// ==================== BOWLING INPUT ====================

void ACricketPlayerController::HandlePaceInput(float Value)
{
    if (!bCanInput || !bIsBowling) return;
    
    // Value from 0.0 to 1.0
    // 0.0 = gentle (seam up)
    // 1.0 = jam (bouncer)
    PaceControl = FMath::Clamp(Value, 0.0f, 1.0f);
    
    if (ControlledCricketCharacter)
    {
        ControlledCricketCharacter->SetBowlingInput(PaceControl, SpinControl, WristAngle);
    }
}

void ACricketPlayerController::HandleSpinInput(FVector2D Value)
{
    if (!bCanInput || !bIsBowling) return;
    
    SpinControl = Value;
    
    // Apply deadzone
    if (SpinControl.Size() < 0.1f)
    {
        SpinControl = FVector2D::ZeroVector;
    }
    
    if (ControlledCricketCharacter)
    {
        ControlledCricketCharacter->SetBowlingInput(PaceControl, SpinControl, WristAngle);
    }
}

void ACricketPlayerController::HandleWristInput(float Value)
{
    if (!bCanInput || !bIsBowling) return;
    
    // Value from -1.0 to 1.0
    // -1.0 = counter-clockwise (adds off-spin)
    // 0.0 = neutral
    // +1.0 = clockwise (adds leg-spin)
    WristAngle = FMath::Clamp(Value, -1.0f, 1.0f);
    
    if (ControlledCricketCharacter)
    {
        ControlledCricketCharacter->SetBowlingInput(PaceControl, SpinControl, WristAngle);
    }
}

void ACricketPlayerController::StartRunUp()
{
    if (!bCanInput || !bIsBowling || bInRunUp) return;
    
    bInRunUp = true;
    RunUpProgress = 0.0f;
    
    if (ControlledCricketCharacter)
    {
        ControlledCricketCharacter->SetBowlerRunUp();
    }
    
    UE_LOG(LogCricket, Log, TEXT("Run-up started"));
}

void ACricketPlayerController::StopRunUp()
{
    if (!bCanInput || !bIsBowling) return;
    
    bInRunUp = false;
    RunUpProgress = 0.0f;
    
    UE_LOG(LogCricket, Log, TEXT("Run-up stopped"));
}

void ACricketPlayerController::ReleaseBall()
{
    if (!bCanInput || !bIsBowling) return;
    
    if (ControlledCricketCharacter)
    {
        ControlledCricketCharacter->ReleaseBall();
    }
    
    bInRunUp = false;
    RunUpProgress = 0.0f;
    
    UE_LOG(LogCricket, Log, TEXT("Ball released: Pace=%f, Spin=%s, Wrist=%f"), 
        PaceControl, *SpinControl.ToString(), WristAngle);
}

// ==================== FIELDING ====================

void ACricketPlayerController::ThrowBall()
{
    if (!bCanInput) return;
    
    // Throw ball logic
    UE_LOG(LogCricket, Log, TEXT("Throw executed"));
}

void ACricketPlayerController::AimThrow(FVector2D Direction)
{
    if (!bCanInput) return;
    
    // Aim throw direction
    UE_LOG(LogCricket, Log, TEXT("Aiming throw: %s"), *Direction.ToString());
}

// ==================== CAMERA ====================

void ACricketPlayerController::SetCameraMode(ECameraMode Mode)
{
    CurrentCameraMode = Mode;
    
    // Update target camera position based on mode
    if (!ControlledCricketCharacter) return;
    
    FVector CharacterLocation = ControlledCricketCharacter->GetActorLocation();
    
    switch (Mode)
    {
        case ECameraMode::BehindBatsman:
            TargetCameraLocation = CharacterLocation + FVector(-200, 0, 100);
            TargetCameraRotation = FRotator(-15, 0, 0);
            break;
            
        case ECameraMode::BowlerView:
            TargetCameraLocation = CharacterLocation + FVector(0, 0, 150);
            TargetCameraRotation = FRotator(0, 0, 0);
            break;
            
        case ECameraMode::Wide:
            TargetCameraLocation = CharacterLocation + FVector(0, 0, 500);
            TargetCameraRotation = FRotator(-60, 0, 0);
            break;
            
        case ECameraMode::Drone:
            TargetCameraLocation = CharacterLocation + FVector(0, 100, 300);
            TargetCameraRotation = FRotator(-45, 0, 0);
            break;
            
        case ECameraMode::Chase:
            // Dynamic chase camera
            break;
            
        case ECameraMode::Stump:
            // Behind wicket
            TargetCameraLocation = CharacterLocation + FVector(-50, 0, 150);
            TargetCameraRotation = FRotator(0, 0, 0);
            break;
    }
    
    UE_LOG(LogCricket, Log, TEXT("Camera mode changed to: %d"), (int32)Mode);
}

void ACricketPlayerController::CycleCamera()
{
    int32 CurrentMode = (int32)CurrentCameraMode;
    int32 MaxMode = (int32)ECameraMode::Stump;
    
    CurrentMode = (CurrentMode + 1) % (MaxMode + 1);
    SetCameraMode((ECameraMode)CurrentMode);
}

void ACricketPlayerController::TriggerCameraShake()
{
    if (CurrentCameraSettings.bEnableCameraShake)
    {
        // Play camera shake
        // This would use PlayWorldCameraShake or a custom camera shake
        UE_LOG(LogCricket, Log, TEXT("Camera shake triggered"));
    }
}

void ACricketPlayerController::HandleCameraRotationInput(FVector2D Value)
{
    if (!ControlledCricketCharacter) return;
    
    USpringArmComponent* SpringArm = ControlledCricketCharacter->GetComponentByClass<USpringArmComponent>();
    if (SpringArm)
    {
        SpringArm->AddLocalRotation(FRotator(Value.Y, Value.X, 0));
    }
}

void ACricketPlayerController::HandleCameraZoomInput(float Value)
{
    if (!ControlledCricketCharacter) return;
    
    USpringArmComponent* SpringArm = ControlledCricketCharacter->GetComponentByClass<USpringArmComponent>();
    if (SpringArm)
    {
        float CurrentLength = SpringArm->TargetArmLength;
        float NewLength = FMath::Clamp(CurrentLength + (Value * 50.0f), 100.0f, 600.0f);
        SpringArm->TargetArmLength = NewLength;
    }
}

void ACricketPlayerController::HandleCameraSwitchInput()
{
    CycleCamera();
}

void ACricketPlayerController::UpdateCameraPosition(float DeltaTime)
{
    if (!ControlledCricketCharacter) return;
    
    UCameraComponent* Camera = ControlledCricketCharacter->GetComponentByClass<UCameraComponent>();
    if (!Camera) return;
    
    // Interpolate to target camera position
    FVector CurrentLocation = Camera->GetComponentLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation, DeltaTime, CameraInterpolationSpeed);
    
    Camera->SetWorldLocation(NewLocation);
    
    // Interpolate rotation
    FRotator CurrentRotation = Camera->GetComponentRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetCameraRotation, DeltaTime, CameraInterpolationSpeed);
    
    Camera->SetWorldRotation(NewRotation);
}

// ==================== UI ====================

void ACricketPlayerController::ShowScoreboard()
{
    // Show scoreboard UI
    UE_LOG(LogCricket, Log, TEXT("Scoreboard shown"));
}

void ACricketPlayerController::HideScoreboard()
{
    // Hide scoreboard UI
    UE_LOG(LogCricket, Log, TEXT("Scoreboard hidden"));
}

void ACricketPlayerController::TogglePauseMenu()
{
    // Toggle pause menu
    if (UGameplayStatics::IsGamePaused(this))
    {
        UGameplayStatics::SetGamePaused(this, false);
    }
    else
    {
        UGameplayStatics::SetGamePaused(this, true);
    }
}

// ==================== HELPER FUNCTIONS ====================

void ACricketPlayerController::UpdateInputState(float DeltaTime)
{
    // Update run-up progress
    if (bInRunUp && bIsBowling)
    {
        RunUpProgress += DeltaTime * 0.5f; // 2 second run-up
        
        if (RunUpProgress >= 1.0f)
        {
            // Auto-release when run-up complete
            ReleaseBall();
        }
    }
}

FVector ACricketPlayerController::CalculateShotDirection()
{
    if (!ControlledCricketCharacter) return FVector::ForwardVector;
    
    FVector BaseDirection = ControlledCricketCharacter->GetActorForwardVector();
    
    // Apply footwork input
    float HorizontalAngle = FootworkInput.X * 90.0f;
    float VerticalAngle = FootworkInput.Y * 45.0f;
    
    FVector Direction = BaseDirection.RotateAngleAxis(HorizontalAngle, FVector::UpVector);
    Direction = Direction.RotateAngleAxis(VerticalAngle, ControlledCricketCharacter->GetActorRightVector());
    
    return Direction.GetSafeNormal();
}
