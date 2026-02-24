#include "CricketPlayerState.h"

ACricketPlayerState::ACricketPlayerState()
{
    PlayerID = 0;
    TeamID = 0;
    PlayerRole = EPlayerRole::Batsman;
    PlayerGrade = EPlayerGrade::Silver;
    Age = 25;
    BattingSkill = 50.0f;
    Form = 50.0f;
    BowlingSkill = 50.0f;
    BowlingForm = 50.0f;
    FieldingSkill = 50.0f;
    ContractValue = 100000;
    Energy = 100;
    Morale = 50;
    BrandValue = 0;
    Experience = 0;
    MatchRuns = 0;
    MatchBallsFaced = 0;
    MatchWickets = 0;
    MatchRunsConceded = 0;
}

void ACricketPlayerState::BeginPlay()
{
    Super::BeginPlay();
}

void ACricketPlayerState::AddMatchRuns(int32 Runs)
{
    MatchRuns += Runs;
    BattingStats.Runs += Runs;
    BattingStats.BallsFaced += MatchBallsFaced;
    
    if (Runs > BattingStats.HighestScore)
        BattingStats.HighestScore = Runs;
    
    if (Runs >= 100) BattingStats.Hundreds++;
    else if (Runs >= 50) BattingStats.Fifties++;
}

void ACricketPlayerState::AddMatchBalls(int32 Balls)
{
    MatchBallsFaced += Balls;
    BattingStats.BallsFaced += Balls;
}

void ACricketPlayerState::AddWicket()
{
    MatchWickets++;
    BowlingStats.Wickets++;
    
    if (BowlingStats.BestBowlingWickets < MatchWickets ||
        (BowlingStats.BestBowlingWickets == MatchWickets && BowlingStats.BestBowlingRuns > MatchRunsConceded))
    {
        BowlingStats.BestBowlingWickets = MatchWickets;
        BowlingStats.BestBowlingRuns = MatchRunsConceded;
    }
    
    if (MatchWickets >= 5)
        BowlingStats.FiveWicketHauls++;
}

void ACricketPlayerState::AddRunsConceded(int32 Runs)
{
    MatchRunsConceded += Runs;
    BowlingStats.RunsConceded += Runs;
}

void ACricketPlayerState::UpdateCareerStats()
{
    BattingStats.Matches++;
    BattingStats.Innings++;
    BowlingStats.Matches++;
    BowlingStats.Innings++;
    Experience += 10;
    
    // Update skills based on performance
    if (MatchRuns > 50)
        BattingSkill = FMath::Clamp(BattingSkill + 1.0f, 0.0f, 99.0f);
    
    if (MatchWickets > 2)
        BowlingSkill = FMath::Clamp(BowlingSkill + 1.0f, 0.0f, 99.0f);
}

void ACricketPlayerState::SetEnergy(int32 NewEnergy)
{
    Energy = FMath::Clamp(NewEnergy, 0, 100);
}

void ACricketPlayerState::UseEnergy(int32 Amount)
{
    Energy = FMath::Clamp(Energy - Amount, 0, 100);
    
    // Low energy affects form
    if (Energy < 30)
        Form = FMath::Clamp(Form - 5.0f, 0.0f, 100.0f);
}

void ACricketPlayerState::RestPlayer()
{
    Energy = FMath::Clamp(Energy + 30, 0, 100);
    Form = FMath::Clamp(Form + 5.0f, 0.0f, 100.0f);
}

float ACricketPlayerState::GetOverallRating() const
{
    float Batting = BattingSkill * 0.5f;
    float Bowling = (PlayerRole == EPlayerRole::Batsman) ? 0.0f : BowlingSkill * 0.5f;
    float Fielding = FieldingSkill * 0.2f;
    float FormFactor = Form / 100.0f;
    
    return (Batting + Bowling + Fielding) * FormFactor;
}

void ACricketPlayerState::ResetForNewMatch()
{
    MatchRuns = 0;
    MatchBallsFaced = 0;
    MatchWickets = 0;
    MatchRunsConceded = 0;
}
