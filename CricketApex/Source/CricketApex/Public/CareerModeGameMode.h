#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CareerModeGameMode.generated.h"

class ACricketPlayerState;
class ACricketCharacter;
enum class ELeagueLevel : uint8;
enum class EContractType : uint8;

UENUM(BlueprintType)
enum class ELeagueLevel : uint8
{
    SundayLeague UMETA(DisplayName = "Sunday League"),
    CountyDivision2 UMETA(DisplayName = "County Division 2"),
    CountyDivision1 UMETA(DisplayName = "County Division 1"),
    Regional UMETA(DisplayName = "Regional"),
    International UMETA(DisplayName = "International")
};

UENUM(BlueprintType)
enum class EContractType : uint8
{
    MatchFee UMETA(DisplayName = "Match Fee"),
    Seasonal UMETA(DisplayName = "Seasonal Contract"),
    Central UMETA(DisplayName = "Central Contract"),
    Franchise UMETA(DisplayName = "Franchise T20")
};

USTRUCT(BlueprintType)
struct FCareerProgress
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ELeagueLevel CurrentLeague = ELeagueLevel::SundayLeague;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CareerMatches = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CareerWins = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CareerLosses = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TotalRuns = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TotalWickets = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 PlayerOfMatch = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SeasonsPlayed = 0;
};

USTRUCT(BlueprintType)
struct FPressConference
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Question;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> Responses;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SelectedResponse = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MoraleImpact = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BrandImpact = 0;
};

UCLASS()
class CRICKETAPEX_API ACareerModeGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ACareerModeGameMode();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Career")
    FCareerProgress CareerProgress;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Career")
    int32 CurrentTeamID = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Career")
    EContractType CurrentContract = EContractType::MatchFee;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Career")
    int32 ContractValue = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Career")
    int32 MatchFee = 1000;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Career")
    int32 Season = 1;

    UFUNCTION(BlueprintCallable, Category = "Career")
    void StartCareer(FString PlayerName, EPlayerRole Role);

    UFUNCTION(BlueprintCallable, Category = "Career")
    void CompleteMatch(bool bWin, int32 Runs, int32 Wickets, int32 PlayerRating);

    UFUNCTION(BlueprintCallable, Category = "Career")
    void ProgressLeague();

    UFUNCTION(BlueprintCallable, Category = "Career")
    void NegotiateContract(EContractType ContractType, int32 Value);

    UFUNCTION(BlueprintCallable, Category = "Career")
    void AnswerPressQuestion(int32 ResponseIndex);

    UFUNCTION(BlueprintCallable, Category = "Career")
    void SelectNextMatch();

    UFUNCTION(BlueprintCallable, Category = "Career")
    void RestPlayer(int32 Days);

    UFUNCTION(BlueprintCallable, Category = "Career")
    void TrainPlayer(FString TrainingType);

    UFUNCTION(BlueprintPure, Category = "Career")
    float GetWinRate() const;

    UFUNCTION(BlueprintPure, Category = "Career")
    int32 GetNextLeaguePromotion() const;

    UFUNCTION(BlueprintPure, Category = "Career")
    FPressConference GetRandomPressQuestion() const;

protected:
    virtual void BeginPlay() override;

private:
    void InitializeCareer();
    void CalculatePerformanceBonus(int32 Rating);
    void UpdateMorale(bool bWin);
    void CheckPromotionEligibility();
};
