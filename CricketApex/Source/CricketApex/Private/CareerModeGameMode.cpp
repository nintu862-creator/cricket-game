#include "CareerModeGameMode.h"
#include "CricketPlayerState.h"
#include "Kismet/GameplayStatics.h"

ACareerModeGameMode::ACareerModeGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    CareerProgress.CurrentLeague = ELeagueLevel::SundayLeague;
    CareerProgress.CareerMatches = 0;
    CareerProgress.CareerWins = 0;
    CareerProgress.CareerLosses = 0;
    CurrentTeamID = 0;
    CurrentContract = EContractType::MatchFee;
    ContractValue = 0;
    MatchFee = 1000;
    Season = 1;
}

void ACareerModeGameMode::BeginPlay()
{
    Super::BeginPlay();
    InitializeCareer();
}

void ACareerModeGameMode::InitializeCareer()
{
    CareerProgress.CareerMatches = 0;
    CareerProgress.CareerWins = 0;
    CareerProgress.CareerLosses = 0;
    CareerProgress.TotalRuns = 0;
    CareerProgress.TotalWickets = 0;
    CareerProgress.PlayerOfMatch = 0;
    CareerProgress.SeasonsPlayed = 0;
    CareerProgress.CurrentLeague = ELeagueLevel::SundayLeague;
    Season = 1;
}

void ACareerModeGameMode::StartCareer(FString PlayerName, EPlayerRole Role)
{
    InitializeCareer();
    
    // Create player
    UE_LOG(LogCricket, Log, TEXT("Career started for: %s, Role: %d"), *PlayerName, (int32)Role);
}

void ACareerModeGameMode::CompleteMatch(bool bWin, int32 Runs, int32 Wickets, int32 PlayerRating)
{
    CareerProgress.CareerMatches++;
    
    if (bWin)
        CareerProgress.CareerWins++;
    else
        CareerProgress.CareerLosses++;
    
    CareerProgress.TotalRuns += Runs;
    CareerProgress.TotalWickets += Wickets;
    
    if (PlayerRating >= 8)
        CareerProgress.PlayerOfMatch++;
    
    CalculatePerformanceBonus(PlayerRating);
    UpdateMorale(bWin);
    CheckPromotionEligibility();
    
    UE_LOG(LogCricket, Log, TEXT("Match completed: %s, Rating: %d"), bWin ? TEXT("Win") : TEXT("Loss"), PlayerRating);
}

void ACareerModeGameMode::ProgressLeague()
{
    int32 CurrentLevel = (int32)CareerProgress.CurrentLeague;
    if (CurrentLevel < (int32)ELeagueLevel::International)
    {
        CareerProgress.CurrentLeague = (ELeagueLevel)(CurrentLevel + 1);
        CareerProgress.SeasonsPlayed++;
        Season++;
        
        // Increase contract value
        ContractValue *= 2;
        MatchFee *= 2;
        
        UE_LOG(LogCricket, Log, TEXT("Promoted to: %d"), CurrentLevel + 1);
    }
}

void ACareerModeGameMode::NegotiateContract(EContractType ContractType, int32 Value)
{
    CurrentContract = ContractType;
    ContractValue = Value;
    
    switch (ContractType)
    {
        case EContractType::MatchFee:
            MatchFee = Value;
            break;
        case EContractType::Seasonal:
            MatchFee = Value / 10;
            break;
        case EContractType::Central:
            MatchFee = Value / 20;
            break;
        case EContractType::Franchise:
            MatchFee = Value / 15;
            break;
    }
}

void ACareerModeGameMode::AnswerPressQuestion(int32 ResponseIndex)
{
    FPressConference PressQ = GetRandomPressQuestion();
    
    if (ResponseIndex >= 0 && ResponseIndex < PressQ.Responses.Num())
    {
        int32 MoraleChange = PressQ.MoraleImpact;
        int32 BrandChange = PressQ.BrandImpact;
        
        // Update player state
        TArray<AActor*> Players;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACricketPlayerState::StaticClass(), Players);
        
        if (Players.Num() > 0)
        {
            ACricketPlayerState* PlayerState = Cast<ACricketPlayerState>(Players[0]);
            if (PlayerState)
            {
                int32 NewMorale = FMath::Clamp(PlayerState->Morale + MoraleChange, 0, 100);
                PlayerState->Morale = NewMorale;
                PlayerState->BrandValue += BrandChange;
            }
        }
    }
}

void ACareerModeGameMode::SelectNextMatch()
{
    // Schedule next match based on league
    UE_LOG(LogCricket, Log, TEXT("Next match scheduled"));
}

void ACareerModeGameMode::RestPlayer(int32 Days)
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACricketPlayerState::StaticClass(), Players);
    
    if (Players.Num() > 0)
    {
        ACricketPlayerState* PlayerState = Cast<ACricketPlayerState>(Players[0]);
        if (PlayerState)
        {
            PlayerState->RestPlayer();
            PlayerState->UseEnergy(-Days * 5);
        }
    }
}

void ACareerModeGameMode::TrainPlayer(FString TrainingType)
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACricketPlayerState::StaticClass(), Players);
    
    if (Players.Num() > 0)
    {
        ACricketPlayerState* PlayerState = Cast<ACricketPlayerState>(Players[0]);
        if (PlayerState)
        {
            PlayerState->UseEnergy(20);
            
            if (TrainingType == "Batting")
                PlayerState->BattingSkill = FMath::Clamp(PlayerState->BattingSkill + 2.0f, 0.0f, 99.0f);
            else if (TrainingType == "Bowling")
                PlayerState->BowlingSkill = FMath::Clamp(PlayerState->BowlingSkill + 2.0f, 0.0f, 99.0f);
            else if (TrainingType == "Fielding")
                PlayerState->FieldingSkill = FMath::Clamp(PlayerState->FieldingSkill + 2.0f, 0.0f, 99.0f);
        }
    }
}

float ACareerModeGameMode::GetWinRate() const
{
    if (CareerProgress.CareerMatches == 0) return 0.0f;
    return (float)CareerProgress.CareerWins / (float)CareerProgress.CareerMatches * 100.0f;
}

int32 ACareerModeGameMode::GetNextLeaguePromotion() const
{
    switch (CareerProgress.CurrentLeague)
    {
        case ELeagueLevel::SundayLeague: return 10;
        case ELeagueLevel::CountyDivision2: return 8;
        case ELeagueLevel::CountyDivision1: return 6;
        case ELeagueLevel::Regional: return 4;
        default: return 0;
    }
}

FPressConference ACareerModeGameMode::GetRandomPressQuestion() const
{
    FPressConference PressQ;
    
    int32 QuestionType = FMath::RandRange(0, 4);
    
    switch (QuestionType)
    {
        case 0:
            PressQ.Question = "How do you feel about today's performance?";
            PressQ.Responses = {"I'm proud of the team effort", "We need to improve more", "Disappointed with the result"};
            PressQ.MoraleImpact = 5;
            PressQ.BrandImpact = 1000;
            break;
        case 1:
            PressQ.Question = "What's your target for this season?";
            PressQ.Responses = {"Win the championship", "Improve personally", "Just play good cricket"};
            PressQ.MoraleImpact = 10;
            PressQ.BrandImpact = 2000;
            break;
        case 2:
            PressQ.Question = "How do you handle pressure?";
            PressQ.Responses = {"Embrace it", "Stay calm and focused", "It motivates me"};
            PressQ.MoraleImpact = 15;
            PressQ.BrandImpact = 3000;
            break;
        default:
            PressQ.Question = "Any message for your fans?";
            PressQ.Responses = {"Thank you for the support", "Keep cheering us", "We're working hard"};
            PressQ.MoraleImpact = 8;
            PressQ.BrandImpact = 1500;
            break;
    }
    
    return PressQ;
}

void ACareerModeGameMode::CalculatePerformanceBonus(int32 Rating)
{
    if (Rating >= 8)
    {
        MatchFee *= 2;
    }
    else if (Rating >= 6)
    {
        MatchFee = (MatchFee * 150) / 100;
    }
}

void ACareerModeGameMode::UpdateMorale(bool bWin)
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACricketPlayerState::StaticClass(), Players);
    
    if (Players.Num() > 0)
    {
        ACricketPlayerState* PlayerState = Cast<ACricketPlayerState>(Players[0]);
        if (PlayerState)
        {
            int32 MoraleChange = bWin ? 10 : -5;
            PlayerState->Morale = FMath::Clamp(PlayerState->Morale + MoraleChange, 0, 100);
        }
    }
}

void ACareerModeGameMode::CheckPromotionEligibility()
{
    int32 RequiredWins = GetNextLeaguePromotion();
    
    if (CareerProgress.CareerWins >= RequiredWins)
    {
        ProgressLeague();
    }
}
