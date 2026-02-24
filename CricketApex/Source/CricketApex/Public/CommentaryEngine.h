#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CommentaryEngine.generated.h"

class USoundBase;

UENUM(BlueprintType)
enum class ECommentaryContext : uint8
{
    PreMatch UMETA(DisplayName = "Pre Match"),
    PowerPlay UMETA(DisplayName = "Power Play"),
    MiddleOvers UMETA(DisplayName = "Middle Overs"),
    DeathOvers UMETA(DisplayName = "Death Overs"),
    Wicket UMETA(DisplayName = "Wicket"),
    Boundary UMETA(DisplayName = "Boundary"),
    Milestone UMETA(DisplayName = "Milestone"),
    Celebration UMETA(DisplayName = "Celebration")
};

USTRUCT(BlueprintType)
struct FCommentaryLine
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Text;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 3.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CommentatorName;
};

UCLASS()
class CRICKETAPEX_API ACommentaryEngine : public AActor
{
    GENERATED_BODY()

public:
    ACommentaryEngine();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commentary")
    TArray<FString> CommentatorNames;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Commentary")
    bool bEnableCommentary = true;

    UFUNCTION(BlueprintCallable, Category = "Commentary")
    FCommentaryLine GetCommentaryForEvent(ECommentaryContext Context, FString PlayerName, int32 Value);

    UFUNCTION(BlueprintCallable, Category = "Commentary")
    void TriggerPreMatchCommentary();

    UFUNCTION(BlueprintCallable, Category = "Commentary")
    void TriggerWicketCommentary(FString BatterName, FString BowlerName, int32 WicketType);

    UFUNCTION(BlueprintCallable, Category = "Commentary")
    void TriggerBoundaryCommentary(FString BatterName, int32 Runs);

    UFUNCTION(BlueprintCallable, Category = "Commentary")
    void TriggerMilestoneCommentary(FString PlayerName, int32 Runs);

    UFUNCTION(BlueprintCallable, Category = "Commentary")
    void TriggerOverCommentary(int32 OverNumber);

    UFUNCTION(BlueprintCallable, Category = "Commentary")
    FString GenerateContextualComment();

    UFUNCTION(BlueprintCallable, Category = "Match")
    void UpdateMatchContext(int32 Score, int32 Wickets, int32 Over, bool bPowerPlay);

    UFUNCTION(BlueprintCallable, Category = "History")
    void RecordPlayerHistory(FString PlayerName, int32 Sixes, int32 Fours, int32 HighScore);

protected:
    virtual void BeginPlay() override;

private:
    FCommentaryLine SelectCommentaryLine(TArray<FCommentaryLine> Lines);
    FString GeneratePlayerCommentary(FString PlayerName);
    void AnalyzeRecentForm();

    int32 CurrentScore = 0;
    int32 CurrentWickets = 0;
    int32 CurrentOver = 0;
    bool bIsPowerPlay = false;
    
    TMap<FString, int32> PlayerSixHistory;
    TMap<FString, int32> PlayerFourHistory;
    TMap<FString, int32> PlayerHighScore;
};
