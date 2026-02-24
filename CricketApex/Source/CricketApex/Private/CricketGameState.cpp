#include "CricketGameState.h"
#include "CricketPlayerState.h"
#include "Net/UnrealNetwork.h"

ACricketGameState::ACricketGameState()
{
    PrimaryActorTick.bCanEverTick = true;

    // Initialize match
    MatchFormat = ECricketFormat::T20;
    TotalOvers = 20;
    Current innings = 0;
    bMatchStarted = false;
    bMatchEnded = false;
    WinningTeam = -1;
    
    // Initialize scores
    Team1Score.TeamID = 0;
    Team1Score.Runs = 0;
    Team1Score.Wickets = 0;
    Team1Score.Overs = 0;
    Team1Score.Balls = 0;
    Team1Score.RunRate = 0.0f;
    Team1Score.Extras = 0;
    
    Team2Score.TeamID = 1;
    Team2Score.Runs = 0;
    Team2Score.Wickets = 0;
    Team2Score.Overs = 0;
    Team2Score.Balls = 0;
    Team2Score.RunRate = 0.0f;
    Team2Score.Extras = 0;
    
    // Power play
    bIsPowerPlay = false;
    PowerPlayOvers = 6;
    
    // Partnership
    CurrentPartnership = 0;
    BallsInPartnership = 0;
    
    // Last 5 overs
    Last5OversRuns = 0;
    
    CurrentBattingTeam = 0;
}

void ACricketGameState::BeginPlay()
{
    Super::BeginPlay();
    InitializeMatch();
}

void ACricketGameState::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (bMatchStarted && !bMatchEnded)
    {
        UpdateStats();
        CheckWinCondition();
    }
}

void ACricketGameState::InitializeMatch()
{
    Team1Score.Runs = 0;
    Team1Score.Wickets = 0;
    Team1Score.Overs = 0;
    Team1Score.Balls = 0;
    Team1Score.RunRate = 0.0f;
    Team1Score.Extras = 0;
    
    Team2Score.Runs = 0;
    Team2Score.Wickets = 0;
    Team2Score.Overs = 0;
    Team2Score.Balls = 0;
    Team2Score.RunRate = 0.0f;
    Team2Score.Extras = 0;
    
    bMatchStarted = false;
    bMatchEnded = false;
    WinningTeam = -1;
    Current innings = 0;
    TargetScore = 0;
    bIsChasing = false;
    CurrentPartnership = 0;
    BallsInPartnership = 0;
    OverHistory.Empty();
}

void ACricketGameState::AddRun(int32 Runs)
{
    if (CurrentBattingTeam == 0)
        Team1Score.Runs += Runs;
    else
        Team2Score.Runs += Runs;
    
    CurrentPartnership += Runs;
    OnScoreUpdate.Broadcast();
}

void ACricketGameState::AddWicket()
{
    if (CurrentBattingTeam == 0)
        Team1Score.Wickets++;
    else
        Team2Score.Wickets++;
    
    CurrentPartnership = 0;
    BallsInPartnership = 0;
    OnWicket.Broadcast();
}

void ACricketGameState::AddExtra(EExtraType ExtraType, int32 Runs)
{
    if (CurrentBattingTeam == 0)
    {
        Team1Score.Extras += Runs;
        if (ExtraType == EExtraType::Wide) Team1Score.Wides += Runs;
        else if (ExtraType == EExtraType::NoBall) Team1Score.NoBalls += Runs;
        else if (ExtraType == EExtraType::Bye) Team1Score.Byes += Runs;
        else if (ExtraType == EExtraType::LegBye) Team1Score.LegByes += Runs;
    }
    else
    {
        Team2Score.Extras += Runs;
        if (ExtraType == EExtraType::Wide) Team2Score.Wides += Runs;
        else if (ExtraType == EExtraType::NoBall) Team2Score.NoBalls += Runs;
        else if (ExtraType == EExtraType::Bye) Team2Score.Byes += Runs;
        else if (ExtraType == EExtraType::LegBye) Team2Score.LegByes += Runs;
    }
    OnScoreUpdate.Broadcast();
}

void ACricketGameState::CompleteBall(int32 BallResult)
{
    if (OverHistory.Num() == 0 || OverHistory.Last().BallResults.Num() >= 6)
    {
        FOverHistory NewOver;
        NewOver.OverNumber = GetBattingTeamScore().Overs;
        OverHistory.Add(NewOver);
    }
    
    FOverHistory& CurrentOver = OverHistory.Last();
    CurrentOver.BallResults.Add(BallResult);
    
    if (CurrentBattingTeam == 0)
        Team1Score.Balls++;
    else
        Team2Score.Balls++;
    
    BallsInPartnership++;
    
    switch (BallResult)
    {
        case 0: break;
        case 1: case 2: case 3: AddRun(BallResult); break;
        case 4: AddRun(4); break;
        case 6: AddRun(6); break;
        case -1: AddExtra(EExtraType::Wide, 1); break;
        case -2: AddExtra(EExtraType::NoBall, 1); break;
        case -3: AddWicket(); break;
    }
    
    if (GetBattingTeamScore().Balls >= 6)
        CompleteOver();
    
    CalculateRunRate();
}

void ACricketGameState::CompleteOver()
{
    if (CurrentBattingTeam == 0)
    {
        Team1Score.Overs++;
        Team1Score.Balls = 0;
    }
    else
    {
        Team2Score.Overs++;
        Team2Score.Balls = 0;
    }
    
    CurrentPartnership = 0;
    BallsInPartnership = 0;
    UpdatePowerPlayState();
    OnOverComplete.Broadcast();
}

void ACricketGameState::SetTargetScore(int32 Target)
{
    TargetScore = Target;
    bIsChasing = true;
}

void ACricketGameState::StartSecondInnings()
{
    Current innings = 1;
    CurrentBattingTeam = 1;
    bIsChasing = true;
    TargetScore = Team1Score.Runs + 1;
}

void ACricketGameState::EndMatch(int32 Winner)
{
    bMatchEnded = true;
    WinningTeam = Winner;
    OnMatchEnd.Broadcast();
}

void ACricketGameState::UpdatePowerPlayState()
{
    int32 CurrentOverNum = GetBattingTeamScore().Overs;
    bIsPowerPlay = (CurrentOverNum < PowerPlayOvers);
}

void ACricketGameState::UpdateStats()
{
    CalculateRunRate();
    if (bIsChasing)
        CalculateRequiredRunRate();
}

FTeamScore& ACricketGameState::GetBattingTeamScore()
{
    return (CurrentBattingTeam == 0) ? Team1Score : Team2Score;
}

FTeamScore& ACricketGameState::GetBowlingTeamScore()
{
    return (CurrentBattingTeam == 0) ? Team2Score : Team1Score;
}

float ACricketGameState::GetCurrentRunRate()
{
    return GetBattingTeamScore().RunRate;
}

float ACricketGameState::GetRequiredRunRate()
{
    if (TargetScore > 0)
    {
        int32 BallsRemaining = GetBallsRemaining();
        if (BallsRemaining > 0)
        {
            float RunsNeeded = (float)(TargetScore - GetBattingTeamScore().Runs);
            return RunsNeeded / (BallsRemaining / 6.0f);
        }
    }
    return 0.0f;
}

int32 ACricketGameState::GetBallsRemaining()
{
    int32 TotalBalls = TotalOvers * 6;
    int32 BallsBowled = (GetBattingTeamScore().Overs * 6) + GetBattingTeamScore().Balls;
    return FMath::Max(0, TotalBalls - BallsBowled);
}

int32 ACricketGameState::GetRunsLast5Overs()
{
    int32 Runs = 0;
    int32 StartOver = FMath::Max(0, GetBattingTeamScore().Overs - 5);
    
    for (int32 i = StartOver; i < OverHistory.Num() && i < GetBattingTeamScore().Overs; i++)
    {
        for (int32 Result : OverHistory[i].BallResults)
        {
            if (Result > 0)
                Runs += Result;
        }
    }
    return Runs;
}

void ACricketGameState::CalculateRunRate()
{
    FTeamScore& Score = GetBattingTeamScore();
    float Overs = Score.GetOversAsFloat();
    if (Overs > 0)
        Score.RunRate = (float)Score.Runs / Overs;
}

void ACricketGameState::CalculateRequiredRunRate()
{
    // Implemented in GetRequiredRunRate()
}

void ACricketGameState::CheckWinCondition()
{
    if (!bIsChasing) return;
    
    if (GetBattingTeamScore().Runs >= TargetScore)
    {
        EndMatch(CurrentBattingTeam);
    }
    else if (GetBattingTeamScore().Wickets >= 10 || GetBallsRemaining() <= 0)
    {
        EndMatch(1 - CurrentBattingTeam);
    }
}
