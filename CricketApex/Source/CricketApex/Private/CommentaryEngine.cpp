#include "CommentaryEngine.h"

ACommentaryEngine::ACommentaryEngine()
{
    PrimaryActorTick.bCanEverTick = true;
    bEnableCommentary = true;

    CommentatorNames.Add("Rashid Latif");
    CommentatorNames.Add("Darren Ganga");
    CommentatorNames.Add("Melanie Jones");
}

void ACommentaryEngine::BeginPlay()
{
    Super::BeginPlay();
}

FCommentaryLine ACommentaryEngine::GetCommentaryForEvent(ECommentaryContext Context, FString PlayerName, int32 Value)
{
    FCommentaryLine Line;
    Line.CommentatorName = CommentatorNames[FMath::RandRange(0, CommentatorNames.Num() - 1)];
    
    TArray<FCommentaryLine> AvailableLines;
    
    switch (Context)
    {
        case ECommentaryContext::PreMatch:
            Line.Text = "Welcome to the match! Both teams are ready.";
            Line.Duration = 4.0f;
            break;
            
        case ECommentaryContext::PowerPlay:
            Line.Text = "Powerplay underway! This is where matches are won or lost.";
            Line.Duration = 3.5f;
            break;
            
        case ECommentaryContext::MiddleOvers:
            Line.Text = "Middle overs, time to consolidate.";
            Line.Duration = 3.0f;
            break;
            
        case ECommentaryContext::DeathOvers:
            Line.Text = "Death overs - this is where the brave thrive!";
            Line.Duration = 3.5f;
            break;
            
        case ECommentaryContext::Wicket:
            Line.Text = "WICKET! " + PlayerName + " has been dismissed for " + FString::FromInt(Value);
            Line.Duration = 4.0f;
            break;
            
        case ECommentaryContext::Boundary:
            if (Value == 6)
                Line.Text = "SIX! " + PlayerName + " with a massive hit!";
            else
                Line.Text = "FOUR! " + PlayerName + " finds the gap!";
            Line.Duration = 3.0f;
            break;
            
        case ECommentaryContext::Milestone:
            if (Value >= 100)
                Line.Text = "CENTURY! " + PlayerName + " brings up a magnificent hundred!";
            else if (Value >= 50)
                Line.Text = "FIFTY! " + PlayerName + " reaches the milestone.";
            else
                Line.Text = PlayerName + " reaches " + FString::FromInt(Value) + " runs.";
            Line.Duration = 4.0f;
            break;
            
        case ECommentaryContext::Celebration:
            Line.Text = "What a moment! The crowd goes wild!";
            Line.Duration = 3.0f;
            break;
    }
    
    return Line;
}

void ACommentaryEngine::TriggerPreMatchCommentary()
{
    FCommentaryLine Line = GetCommentaryForEvent(ECommentaryContext::PreMatch, "", 0);
    UE_LOG(LogCricket, Log, TEXT("Commentary: %s"), *Line.Text);
}

void ACommentaryEngine::TriggerWicketCommentary(FString BatterName, FString BowlerName, int32 WicketType)
{
    FCommentaryLine Line = GetCommentaryForEvent(ECommentaryContext::Wicket, BatterName, WicketType);
    
    if (WicketType == 1) // Bowled
        Line.Text = "BOWLED! " + BowlerName + " castles the batter!";
    else if (WicketType == 2) // Caught
        Line.Text = "CAUGHT! Great take in the field!";
    else if (WicketType == 3) // LBW
        Line.Text = "LBW! That's plum in front!";
    
    UE_LOG(LogCricket, Log, TEXT("Commentary: %s"), *Line.Text);
}

void ACommentaryEngine::TriggerBoundaryCommentary(FString BatterName, int32 Runs)
{
    FCommentaryLine Line = GetCommentaryForEvent(ECommentaryContext::Boundary, BatterName, Runs);
    
    if (Runs == 6 && PlayerSixHistory.Contains(BatterName))
    {
        int32 Sixes = PlayerSixHistory[BatterName];
        if (Sixes >= 3)
            Line.Text = "That was HUGE! " + BatterName + " is on a scroched earth policy!";
    }
    
    UE_LOG(LogCricket, Log, TEXT("Commentary: %s"), *Line.Text);
}

void ACommentaryEngine::TriggerMilestoneCommentary(FString PlayerName, int32 Runs)
{
    FCommentaryLine Line = GetCommentaryForEvent(ECommentaryContext::Milestone, PlayerName, Runs);
    UE_LOG(LogCricket, Log, TEXT("Commentary: %s"), *Line.Text);
}

void ACommentaryEngine::TriggerOverCommentary(int32 OverNumber)
{
    FCommentaryLine Line;
    Line.CommentatorName = CommentatorNames[FMath::RandRange(0, CommentatorNames.Num() - 1)];
    
    if (OverNumber == 1)
        Line.Text = "First over of the innings, plenty of pressure on the bowler.";
    else if (OverNumber == 6)
        Line.Text = "End of the powerplay, " + FString::FromInt(CurrentScore) + " runs on the board.";
    else if (OverNumber == 10)
        Line.Text = "Halfway point, need to accelerate now.";
    else if (OverNumber == 15)
        Line.Text = "Death overs approaching, time to push the button.";
    else if (OverNumber == 20)
        Line.Text = "Final over, give it everything!";
    else
        Line.Text = "Over " + FString::FromInt(OverNumber) + " complete.";
    
    Line.Duration = 3.0f;
    UE_LOG(LogCricket, Log, TEXT("Commentary: %s"), *Line.Text);
}

FString ACommentaryEngine::GenerateContextualComment()
{
    FString Context = "";
    
    if (bIsPowerPlay)
        Context = "Powerplay: ";
    else if (CurrentOver >= 15)
        Context = "Death overs: ";
    else
        Context = "Middle overs: ";
    
    // Add match situation
    if (CurrentWickets >= 8)
        Context += "They need miracles with " + FString::FromInt(10 - CurrentWickets) + " down.";
    else if (CurrentScore > 150)
        Context += "Excellent batting, run rate is healthy.";
    else
        Context += "Building partnerships is key.";
    
    return Context;
}

void ACommentaryEngine::UpdateMatchContext(int32 Score, int32 Wickets, int32 Over, bool PowerPlay)
{
    CurrentScore = Score;
    CurrentWickets = Wickets;
    CurrentOver = Over;
    bIsPowerPlay = PowerPlay;
}

void ACommentaryEngine::RecordPlayerHistory(FString PlayerName, int32 Sixes, int32 Fours, int32 HighScore)
{
    if (Sixes > 0)
    {
        int32 Current = PlayerSixHistory.Contains(PlayerName) ? PlayerSixHistory[PlayerName] : 0;
        PlayerSixHistory[PlayerName] = Current + Sixes;
    }
    
    if (Fours > 0)
    {
        int32 Current = PlayerFourHistory.Contains(PlayerName) ? PlayerFourHistory[PlayerName] : 0;
        PlayerFourHistory[PlayerName] = Current + Fours;
    }
    
    if (HighScore > 0)
    {
        int32 Current = PlayerHighScore.Contains(PlayerName) ? PlayerHighScore[PlayerName] : 0;
        if (HighScore > Current)
            PlayerHighScore[PlayerName] = HighScore;
    }
}

FCommentaryLine ACommentaryEngine::SelectCommentaryLine(TArray<FCommentaryLine> Lines)
{
    if (Lines.Num() == 0)
    {
        FCommentaryLine Empty;
        return Empty;
    }
    return Lines[FMath::RandRange(0, Lines.Num() - 1)];
}

FString ACommentaryEngine::GeneratePlayerCommentary(FString PlayerName)
{
    if (PlayerSixHistory.Contains(PlayerName) && PlayerSixHistory[PlayerName] >= 5)
        return PlayerName + " has been destructive with the bat!";
    if (PlayerHighScore.Contains(PlayerName) && PlayerHighScore[PlayerName] >= 150)
        return PlayerName + " has proven class with a big score!";
    return PlayerName + " looks in good touch.";
}

void ACommentaryEngine::AnalyzeRecentForm()
{
    // Analyze recent performance for contextual commentary
}
