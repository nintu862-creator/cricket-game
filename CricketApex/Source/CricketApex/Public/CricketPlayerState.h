#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CricketPlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
    Batsman UMETA(DisplayName = "Batsman"),
    Bowler UMETA(DisplayName = "Bowler"),
    AllRounder UMETA(DisplayName = "All-Rounder"),
    WicketKeeper UMETA(DisplayName = "Wicket Keeper")
};

UENUM(BlueprintType)
enum class EPlayerGrade : uint8
{
    Bronze UMETA(DisplayName = "Bronze"),
    Silver UMETA(DisplayName = "Silver"),
    Gold UMETA(DisplayName = "Gold"),
    Platinum UMETA(DisplayName = "Platinum"),
    Diamond UMETA(DisplayName = "Diamond")
};

USTRUCT(BlueprintType)
struct FBattingStats
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Matches = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Innings = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NotOuts = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Runs = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HighestScore = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Hundreds = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Fifties = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Fours = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Sixes = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BallsFaced = 0;
    
    float GetAverage() const { return (Innings - NotOuts) > 0 ? (float)Runs / (float)(Innings - NotOuts) : (float)Runs; }
    float GetStrikeRate() const { return BallsFaced > 0 ? ((float)Runs / (float)BallsFaced) * 100.0f : 0.0f; }
};

USTRUCT(BlueprintType)
struct FBowlingStats
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Matches = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Innings = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Balls = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RunsConceded = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Wickets = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaidenOvers = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BestBowlingWickets = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BestBowlingRuns = 0;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 FiveWicketHauls = 0;
    
    float GetAverage() const { return Wickets > 0 ? (float)RunsConceded / (float)Wickets : 0.0f; }
    float GetEconomy() const { float Overs = Balls / 6.0f; return Overs > 0 ? (float)RunsConceded / Overs : 0.0f; }
    float GetStrikeRate() const { return Wickets > 0 ? (float)Balls / (float)Wickets : 0.0f; }
};

UCLASS()
class CRICKETAPEX_API ACricketPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    ACricketPlayerState();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    FString PlayerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    int32 PlayerID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    int32 TeamID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    EPlayerRole PlayerRole = EPlayerRole::Batsman;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    EPlayerGrade PlayerGrade = EPlayerGrade::Silver;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    int32 Age = 25;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Info")
    FString Nationality = "Unknown";

    // Batting
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Batting")
    FBattingStats BattingStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Batting")
    float BattingSkill = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Batting")
    float Form = 50.0f;

    // Bowling
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Bowling")
    FBowlingStats BowlingStats;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Bowling")
    float BowlingSkill = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Bowling")
    float BowlingForm = 50.0f;

    // Fielding
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Fielding")
    float FieldingSkill = 50.0f;

    // Career Mode Specific
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Career")
    int32 ContractValue = 100000;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Career")
    int32 Energy = 100;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Career")
    int32 Morale = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Career")
    int32 BrandValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Career")
    int32 Experience = 0;

    // Match Performance
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Match")
    int32 MatchRuns = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Match")
    int32 MatchBallsFaced = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Match")
    int32 MatchWickets = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Match")
    int32 MatchRunsConceded = 0;

    UFUNCTION(BlueprintCallable, Category = "Player|Stats")
    void AddMatchRuns(int32 Runs);

    UFUNCTION(BlueprintCallable, Category = "Player|Stats")
    void AddMatchBalls(int32 Balls);

    UFUNCTION(BlueprintCallable, Category = "Player|Stats")
    void AddWicket();

    UFUNCTION(BlueprintCallable, Category = "Player|Stats")
    void AddRunsConceded(int32 Runs);

    UFUNCTION(BlueprintCallable, Category = "Player|Stats")
    void UpdateCareerStats();

    UFUNCTION(BlueprintCallable, Category = "Player|State")
    void SetEnergy(int32 NewEnergy);

    UFUNCTION(BlueprintCallable, Category = "Player|State")
    void UseEnergy(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Player|State")
    void RestPlayer();

    UFUNCTION(BlueprintPure, Category = "Player|Stats")
    float GetOverallRating() const;

    virtual void ResetForNewMatch() override;

protected:
    virtual void BeginPlay() override;
};
