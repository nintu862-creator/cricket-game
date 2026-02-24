#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StadiumActor.generated.h"

class UStaticMeshComponent;
class UInstancedStaticMeshComponent;

UENUM(BlueprintType)
enum class EStadiumType : uint8
{
    Test UMETA(DisplayName = "Test Venue"),
    ODI UMETA(DisplayName = "ODI Venue"),
    T20 UMETA(DisplayName = "T20 Venue"),
    Indoor UMETA(DisplayName = "Indoor")
};

USTRUCT(BlueprintType)
struct FPitchProperties
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Hardness = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Moisture = 0.3f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GrassLength = 0.5f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cracks = 0.2f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor PitchColor = FLinearColor(0.6f, 0.5f, 0.3f);
};

UCLASS()
class CRICKETAPEX_API AStadiumActor : public AActor
{
    GENERATED_BODY()

public:
    AStadiumActor();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* StadiumMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* PitchMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* CrowdMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UInstancedStaticMeshComponent* FloodlightMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stadium")
    FString StadiumName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stadium")
    EStadiumType StadiumType = EStadiumType::T20;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stadium")
    int32 Capacity = 25000;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stadium")
    FString Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stadium")
    bool bHasFloodlights = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stadium")
    bool bIsDayNight = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pitch")
    FPitchProperties PitchProperties;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
    float AmbientNoise = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
    float CrowdDensity = 0.8f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Environment")
    float TimeOfDay = 12.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Environment")
    FVector WindDir = FVector::ZeroVector;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Environment")
    float WindSpeed = 0.0f;

    UFUNCTION(BlueprintCallable, Category = "Environment")
    void SetTimeOfDay(float Hour);

    UFUNCTION(BlueprintCallable, Category = "Environment")
    void SetWeather(FString WeatherType);

    UFUNCTION(BlueprintCallable, Category = "Pitch")
    void UpdatePitchCondition(float OverNumber);

    UFUNCTION(BlueprintCallable, Category = "Crowd")
    void SetCrowdDensity(float Density);

    UFUNCTION(BlueprintCallable, Category = "Crowd")
    void TriggerCrowdReaction(FString ReactionType);

    UFUNCTION(BlueprintCallable, Category = "Environment")
    void UpdateWind(FVector Direction, float Speed);

    UFUNCTION(BlueprintPure, Category = "Environment")
    FVector GetWindVector() const { return WindDir * WindSpeed; }

    UFUNCTION(BlueprintPure, Category = "Boundary")
    float GetBoundaryDistance() const;

    UFUNCTION(BlueprintPure, Category = "Boundary")
    FVector GetBoundaryCenter() const;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    void InitializeStadium();
    void SpawnCrowd();
    void UpdateCrowdAnimation(float DeltaTime);
    void UpdateLighting();

    float BoundaryRadius;
    float BoundaryHeight;
};
