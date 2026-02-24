#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CricketGameMode.generated.h"

class ACricketPlayerState;
class ACricketCharacter;
class ABall;
class ACricketPlayerController;
class ACricketGameState;
enum class ECricketFormat : uint8;
enum class EInningsState : uint8;

/**
 * ECricketFormat - Different formats of cricket
 */
UENUM(BlueprintType)
enum class ECricketFormat : uint8
{
    Test UMETA(DisplayName = "Test Cricket"),
    ODI UMETA(DisplayName = "One Day International"),
    T20 UMETA(DisplayName = "T20"),
    TheHundred UMETA(DisplayName = "The Hundred")
};

/**
 * EInningsState - Current state of the innings
 */
UENUM(BlueprintType)
enum class EInningsState : uint8
{
    NotStarted UMETA(DisplayName = "Not Started"),
    FirstInnings UMETA(DisplayName = "First Innings"),
    Interval UMETA(DisplayName = "Interval"),
    SecondInnings UMETA(DisplayName = "Second Innings"),
    Declared UMETA(DisplayName = "Declared"),
    Completed UMETA(DisplayName = "Completed")
};

/**
 * EMatchState - Current state of the match
 */
UENUM(BlueprintType)
enum class EMatchState : uint8
{
    PreMatch UMETA(DisplayName = "Pre Match"),
    Toss UMETA(DisplayName = "Toss"),
    TeamSelect UMETA(DisplayName = "Team Selection"),
    PowerPlay UMETA(DisplayName = "Power Play"),
    MiddleOvers UMETA(DisplayName = "Middle Overs"),
    DeathOvers UMETA(DisplayName = "Death Overs"),
    InningsBreak UMETA(DisplayName = "Innings Break"),
    MatchEnd UMETA(DisplayName = "Match End"),
    SuperOver UMETA(DisplayName = "Super Over")
};

/**
 * ACricketGameMode - Main game mode for cricket matches
 */
UCLASS()
class CRICKETAPEX_API ACricketGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACricketGameMode();

    // Game Mode Interface
    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
    virtual void InitGameState() override;
    virtual void HandleMatchHasStarted() override;
    virtual void HandleMatchIsWaitingToStart() override;
    virtual void ReadyToStartMatch_Implementation() override;
    virtual void SpawnDefaultPawnFor_Implementation(AController* NewPlayer, FVector SpawnLocation, FRotator SpawnRotation) override;

    // Cricket Specific Functions
    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void StartMatch();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void EndMatch();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void StartInnings(int32 TeamIndex);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void EndInnings();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void HandleWicket(ACricketCharacter* WicketTaker, ACricketCharacter* Batter);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void AddRun(int32 Runs, bool IsExtra = false);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void SetBattingTeam(int32 TeamIndex);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void SetBowlingTeam(int32 TeamIndex);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void ChangeBowler();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void StartNewOver();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void CompleteBall();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void DeclareInnings();

    // Getters
    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    ECricketFormat GetCricketFormat() const { return MatchFormat; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    EInningsState GetInningsState() const { return CurrentInningsState; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    EMatchState GetMatchState() const { return CurrentMatchState; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    int32 GetCurrentOver() const { return CurrentOver; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    int32 GetCurrentBall() const { return CurrentBall; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    int32 GetTotalOvers() const;

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    int32 GetTargetScore() const { return TargetScore; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    int32 GetCurrentScore(int32 TeamIndex) const;

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    int32 GetWicketsLost(int32 TeamIndex) const;

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    ACricketCharacter* GetCurrentBowler() const { return CurrentBowler; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    ACricketCharacter* GetCurrentBatter() const { return CurrentBatter; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    ABall* GetCurrentBall() const { return CurrentBallActor; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    int32 GetBattingTeamIndex() const { return BattingTeamIndex; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    int32 GetBowlingTeamIndex() const { return BowlingTeamIndex; }

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    float GetRunRate() const;

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    float GetRequiredRunRate() const;

    UFUNCTION(BlueprintPure, Category = "Cricket|Match")
    int32 GetBallsRemaining() const;

    // Power Play
    UFUNCTION(BlueprintCallable, Category = "Cricket|PowerPlay")
    void SetPowerPlayState(EMatchState PowerState) { CurrentMatchState = PowerState; }

    UFUNCTION(BlueprintPure, Category = "Cricket|PowerPlay")
    bool IsPowerPlay() const { return CurrentMatchState == EMatchState::PowerPlay; }

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    void InitializeMatch();
    void UpdateMatchState();
    void CalculateGameState();
    void CheckWinCondition();
    void UpdateScoreboard();

public:
    // Match Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Config")
    ECricketFormat MatchFormat = ECricketFormat::T20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Config")
    int32 MaxOvers = 20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Config")
    int32 MaxWickets = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Config")
    bool bEnableDuckworthLewis = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Config")
    int32 PowerPlayOvers = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cricket|Config")
    int32 DeathOverStart = 16;

    // Match State
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Match")
    EInningsState CurrentInningsState = EInningsState::NotStarted;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Match")
    EMatchState CurrentMatchState = EMatchState::PreMatch;

    // Teams
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Teams")
    int32 BattingTeamIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Teams")
    int32 BowlingTeamIndex = 1;

    // Current Innings Stats
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Score")
    int32 CurrentScore = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Score")
    int32 CurrentWickets = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Score")
    int32 CurrentOver = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Score")
    int32 CurrentBall = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Score")
    int32 TargetScore = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Score")
    float CurrentRunRate = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Score")
    float RequiredRunRate = 0.0f;

    // Current Players
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Players")
    ACricketCharacter* CurrentBowler;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Players")
    ACricketCharacter* CurrentWicket;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Players")
    ACricketCharacter* CurrentBatter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Players")
    ACricketCharacter* CurrentBatter2;

    // Ball
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Ball")
    ABall* CurrentBallActor;

    // Delegate for score updates
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreUpdate);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnScoreUpdate OnScoreUpdate;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWicket, ACricketCharacter*, Bowler, ACricketCharacter*, Batter);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnWicket OnWicket;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOverComplete);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnOverComplete OnOverComplete;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInningsComplete);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnInningsComplete OnInningsComplete;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchComplete);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnMatchComplete OnMatchComplete;

private:
    FTimerHandle MatchTimerHandle;
    bool bMatchStarted = false;
};
