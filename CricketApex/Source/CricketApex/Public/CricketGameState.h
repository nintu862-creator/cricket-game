#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CricketGameState.generated.h"

class ACricketPlayerState;
class ABall;
enum class ECricketFormat : uint8;
enum class EMatchState : uint8;

/**
 * FTeamScore - Team score structure
 */
USTRUCT(BlueprintType)
struct FTeamScore
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TeamID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Runs = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Wickets = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Overs = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Balls = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RunRate = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Extras = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Wides = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NoBalls = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Byes = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 LegByes = 0;

    float GetOversAsFloat() const
    {
        return Overs + (Balls / 6.0f);
    }
};

/**
 * FOverHistory - History of each ball
 */
USTRUCT(BlueprintType)
struct FOverHistory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 OverNumber = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> BallResults; // 0=dot, 1=1 run, 2=2 runs, 3=3 runs, 4=four, 6=six, -1=wide, -2=no ball, -3=wicket
};

/**
 * ACricketGameState - Game state for cricket match
 */
UCLASS()
class CRICKETAPEX_API ACricketGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ACricketGameState();

    // Team Scores
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Score")
    FTeamScore Team1Score;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Score")
    FTeamScore Team2Score;

    // Match Info
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Match")
    ECricketFormat MatchFormat = ECricketFormat::T20;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Match")
    int32 CurrentInnings = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Match")
    int32 TotalOvers = 20;

    // Target for second innings
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Match")
    int32 TargetScore = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Match")
    bool bIsChasing = false;

    // Match State
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|State")
    bool bMatchStarted = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|State")
    bool bMatchEnded = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|State")
    int32 WinningTeam = -1;

    // Power Play
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|PowerPlay")
    bool bIsPowerPlay = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|PowerPlay")
    int32 PowerPlayOvers = 6;

    // Partnership
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Partnership")
    int32 CurrentPartnership = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Partnership")
    int32 BallsInPartnership = 0;

    // Last 5 overs stats
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cricket|Stats")
    int32 Last5OversRuns = 0;

    // Functions
    UFUNCTION(BlueprintCallable, Category = "Cricket|Score")
    void AddRun(int32 Runs);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Score")
    void AddWicket();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Score")
    void AddExtra(EExtraType ExtraType, int32 Runs);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Score")
    void CompleteBall(int32 BallResult);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Score")
    void CompleteOver();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void SetTargetScore(int32 Target);

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void StartSecondInnings();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Match")
    void EndMatch(int32 Winner);

    UFUNCTION(BlueprintCallable, Category = "Cricket|PowerPlay")
    void UpdatePowerPlayState();

    UFUNCTION(BlueprintCallable, Category = "Cricket|Stats")
    void UpdateStats();

    // Getters
    UFUNCTION(BlueprintPure, Category = "Cricket|Score")
    FTeamScore& GetBattingTeamScore();

    UFUNCTION(BlueprintPure, Category = "Cricket|Score")
    FTeamScore& GetBowlingTeamScore();

    UFUNCTION(BlueprintPure, Category = "Cricket|Stats")
    float GetCurrentRunRate();

    UFUNCTION(BlueprintPure, Category = "Cricket|Stats")
    float GetRequiredRunRate();

    UFUNCTION(BlueprintPure, Category = "Cricket|Stats")
    int32 GetBallsRemaining();

    UFUNCTION(BlueprintPure, Category = "Cricket|Stats")
    int32 GetRunsLast5Overs();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    void InitializeMatch();
    void CalculateRunRate();
    void CalculateRequiredRunRate();
    void CheckWinCondition();

public:
    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreUpdate);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnScoreUpdate OnScoreUpdate;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWicket);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnWicket OnWicket;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOverComplete);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnOverComplete OnOverComplete;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMatchEnd);
    UPROPERTY(BlueprintAssignable, Category = "Cricket|Events")
    FOnMatchEnd OnMatchEnd;

private:
    UPROPERTY()
    TArray<FOverHistory> OverHistory;

    int32 CurrentBattingTeam = 0;
};

/**
 * EExtraType - Types of extras
 */
UENUM(BlueprintType)
enum class EExtraType : uint8
{
    Wide UMETA(DisplayName = "Wide"),
    NoBall UMETA(DisplayName = "No Ball"),
    Bye UMETA(DisplayName = "Bye"),
    LegBye UMETA(DisplayName = "Leg Bye")
};
