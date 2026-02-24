#include "CricketGameMode.h"
#include "CricketCharacter.h"
#include "CricketPlayerController.h"
#include "CricketPlayerState.h"
#include "CricketGameState.h"
#include "Ball.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACricketGameMode::ACricketGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.1f;

    // Initialize default classes
    GameStateClass = ACricketGameState::StaticClass();
    PlayerControllerClass = ACricketPlayerController::StaticClass();
    DefaultPawnClass = ACricketCharacter::StaticClass();

    // Match defaults
    MatchFormat = ECricketFormat::T20;
    MaxOvers = 20;
    MaxWickets = 10;
    bEnableDuckworthLewis = true;
    PowerPlayOvers = 6;
    DeathOverStart = 16;
}

void ACricketGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    // Parse options
    FString FormatStr = UGameplayStatics::ParseOption(Options, "Format");
    if (!FormatStr.IsEmpty())
    {
        if (FormatStr == "Test") MatchFormat = ECricketFormat::Test;
        else if (FormatStr == "ODI") MatchFormat = ECricketFormat::ODI;
        else if (FormatStr == "T20") MatchFormat = ECricketFormat::T20;
        else if (FormatStr == "Hundred") MatchFormat = ECricketFormat::TheHundred;
    }

    // Set overs based on format
    switch (MatchFormat)
    {
        case ECricketFormat::Test:
            MaxOvers = 450; // 90 overs per day (technically unlimited in real test)
            break;
        case ECricketFormat::ODI:
            MaxOvers = 50;
            break;
        case ECricketFormat::T20:
            MaxOvers = 20;
            break;
        case ECricketFormat::TheHundred:
            MaxOvers = 100; // 100 balls
            break;
    }
}

void ACricketGameMode::InitGameState()
{
    Super::InitGameState();
}

void ACricketGameMode::HandleMatchIsWaitingToStart()
{
    Super::HandleMatchIsWaitingToStart();
    CurrentMatchState = EMatchState::PreMatch;
}

void ACricketGameMode::ReadyToStartMatch_Implementation()
{
    Super::ReadyToStartMatch_Implementation();
    StartMatch();
}

void ACricketGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, FVector SpawnLocation, FRotator SpawnRotation)
{
    Super::SpawnDefaultPawnFor_Implementation(NewPlayer, SpawnLocation, SpawnRotation);
}

void ACricketGameMode::BeginPlay()
{
    Super::BeginPlay();
    InitializeMatch();
}

void ACricketGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bMatchStarted && CurrentInningsState != EInningsState::Completed)
    {
        UpdateMatchState();
    }
}

void ACricketGameMode::InitializeMatch()
{
    CurrentScore = 0;
    CurrentWickets = 0;
    CurrentOver = 0;
    CurrentBall = 0;
    TargetScore = 0;
    CurrentRunRate = 0.0f;
    RequiredRunRate = 0.0f;
    CurrentInningsState = EInningsState::NotStarted;
    CurrentMatchState = EMatchState::PreMatch;
}

void ACricketGameMode::StartMatch()
{
    bMatchStarted = true;
    CurrentMatchState = EMatchState::Toss;
    
    // Start first innings
    StartInnings(BattingTeamIndex);
}

void ACricketGameMode::EndMatch()
{
    bMatchStarted = false;
    CurrentMatchState = EMatchState::MatchEnd;
    OnMatchComplete.Broadcast();
}

void ACricketGameMode::StartInnings(int32 TeamIndex)
{
    BattingTeamIndex = TeamIndex;
    BowlingTeamIndex = 1 - TeamIndex;
    
    CurrentInningsState = EInningsState::FirstInnings;
    CurrentScore = 0;
    CurrentWickets = 0;
    CurrentOver = 0;
    CurrentBall = 0;
    
    // Start power play
    CurrentMatchState = EMatchState::PowerPlay;
    
    // Spawn bowler
    ChangeBowler();
    
    // Spawn batters
    // This would typically spawn character actors at the crease
    
    UE_LOG(LogCricket, Log, TEXT("Innings Started - Batting Team: %d"), BattingTeamIndex);
}

void ACricketGameMode::EndInnings()
{
    CurrentInningsState = EInningsState::Completed;
    CurrentMatchState = EMatchState::InningsBreak;
    OnInningsComplete.Broadcast();
    
    UE_LOG(LogCricket, Log, TEXT("Innings Ended - Final Score: %d/%d"), CurrentScore, CurrentWickets);
}

void ACricketGameMode::HandleWicket(ACricketCharacter* WicketTaker, ACricketCharacter* Batter)
{
    CurrentWickets++;
    CurrentWicket = WicketTaker;
    
    OnWicket.Broadcast(WicketTaker, Batter);
    
    // Check if all out
    if (CurrentWickets >= MaxWickets)
    {
        EndInnings();
    }
    else
    {
        // Bring in next batter
        // This would typically trigger UI to select next batter
    }
    
    UpdateScoreboard();
}

void ACricketGameMode::AddRun(int32 Runs, bool IsExtra)
{
    CurrentScore += Runs;
    
    if (!IsExtra)
    {
        OnScoreUpdate.Broadcast();
    }
    
    UpdateScoreboard();
    CheckWinCondition();
}

void ACricketGameMode::SetBattingTeam(int32 TeamIndex)
{
    BattingTeamIndex = TeamIndex;
    BowlingTeamIndex = 1 - TeamIndex;
}

void ACricketGameMode::SetBowlingTeam(int32 TeamIndex)
{
    BowlingTeamIndex = TeamIndex;
    BattingTeamIndex = 1 - TeamIndex;
}

void ACricketGameMode::ChangeBowler()
{
    // In a real implementation, this would select the next bowler from the bowling team
    // based on bowling order,疲劳度, and match situation
    UE_LOG(LogCricket, Log, TEXT("Bowler changed"));
}

void ACricketGameMode::StartNewOver()
{
    CurrentOver++;
    CurrentBall = 0;
    
    // Change bowler at end of over
    if (CurrentOver > 0 && CurrentOver % 1 == 0)
    {
        ChangeBowler();
    }
    
    // Update match state based on over number
    if (CurrentOver >= PowerPlayOvers && CurrentOver < DeathOverStart)
    {
        CurrentMatchState = EMatchState::MiddleOvers;
    }
    else if (CurrentOver >= DeathOverStart)
    {
        CurrentMatchState = EMatchState::DeathOvers;
    }
    
    UE_LOG(LogCricket, Log, TEXT("Over %d Started"), CurrentOver);
}

void ACricketGameMode::CompleteBall()
{
    CurrentBall++;
    
    // Calculate run rate
    float TotalBalls = (CurrentOver * 6) + CurrentBall;
    if (TotalBalls > 0)
    {
        CurrentRunRate = (float)CurrentScore / (TotalBalls / 6.0f);
    }
    
    // Check if over is complete
    if (CurrentBall >= 6)
    {
        CurrentBall = 0;
        StartNewOver();
        OnOverComplete.Broadcast();
    }
    
    // Check for innings end
    if (CurrentOver >= GetTotalOvers())
    {
        EndInnings();
    }
    
    UpdateScoreboard();
}

void ACricketGameMode::DeclareInnings()
{
    CurrentInningsState = EInningsState::Declared;
    EndInnings();
}

int32 ACricketGameMode::GetTotalOvers() const
{
    switch (MatchFormat)
    {
        case ECricketFormat::Test:
            return 450;
        case ECricketFormat::ODI:
            return 50;
        case ECricketFormat::T20:
            return 20;
        case ECricketFormat::TheHundred:
            return 100;
        default:
            return MaxOvers;
    }
}

int32 ACricketGameMode::GetCurrentScore(int32 TeamIndex) const
{
    // In a full implementation, each team would have their own score tracking
    if (TeamIndex == BattingTeamIndex)
    {
        return CurrentScore;
    }
    return 0;
}

int32 ACricketGameMode::GetWicketsLost(int32 TeamIndex) const
{
    if (TeamIndex == BattingTeamIndex)
    {
        return CurrentWickets;
    }
    return 0;
}

float ACricketGameMode::GetRunRate() const
{
    return CurrentRunRate;
}

float ACricketGameMode::GetRequiredRunRate() const
{
    if (TargetScore > 0)
    {
        int32 BallsRemaining = GetBallsRemaining();
        if (BallsRemaining > 0)
        {
            float RunsNeeded = (float)(TargetScore - CurrentScore);
            return RunsNeeded / (BallsRemaining / 6.0f);
        }
    }
    return 0.0f;
}

int32 ACricketGameMode::GetBallsRemaining() const
{
    int32 TotalBalls = GetTotalOvers() * 6;
    int32 BallsBowled = (CurrentOver * 6) + CurrentBall;
    return FMath::Max(0, TotalBalls - BallsBowled);
}

void ACricketGameMode::UpdateMatchState()
{
    CalculateGameState();
}

void ACricketGameMode::CalculateGameState()
{
    // Calculate required run rate if chasing
    if (TargetScore > 0)
    {
        RequiredRunRate = GetRequiredRunRate();
    }
}

void ACricketGameMode::CheckWinCondition()
{
    // Check if target reached in second innings
    if (CurrentInningsState == EInningsState::SecondInnings && TargetScore > 0)
    {
        if (CurrentScore >= TargetScore)
        {
            EndMatch();
        }
    }
}

void ACricketGameMode::UpdateScoreboard()
{
    // Broadcast score update for UI
    OnScoreUpdate.Broadcast();
}
