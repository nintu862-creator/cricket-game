#include "StadiumActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"

AStadiumActor::AStadiumActor()
{
    PrimaryActorTick.bCanEverTick = true;

    StadiumMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StadiumMesh"));
    RootComponent = StadiumMesh;

    PitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PitchMesh"));
    PitchMesh->SetupAttachment(RootComponent);
    PitchMesh->SetRelativeLocation(FVector(0, 0, 0));

    CrowdMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("CrowdMesh"));
    CrowdMesh->SetupAttachment(RootComponent);
    CrowdMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    FloodlightMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloodlightMesh"));
    FloodlightMesh->SetupAttachment(RootComponent);

    StadiumName = "Default Stadium";
    StadiumType = EStadiumType::T20;
    Capacity = 25000;
    Location = "Unknown";
    bHasFloodlights = true;
    bIsDayNight = true;
    AmbientNoise = 0.5f;
    CrowdDensity = 0.8f;
    TimeOfDay = 12.0f;
    BoundaryRadius = 65.0f;
    BoundaryHeight = 3.0f;
}

void AStadiumActor::BeginPlay()
{
    Super::BeginPlay();
    InitializeStadium();
    SpawnCrowd();
}

void AStadiumActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateCrowdAnimation(DeltaTime);
    UpdateLighting();
}

void AStadiumActor::InitializeStadium()
{
    switch (StadiumType)
    {
        case EStadiumType::Test:
            Capacity = 50000;
            BoundaryRadius = 75.0f;
            break;
        case EStadiumType::ODI:
            Capacity = 30000;
            BoundaryRadius = 70.0f;
            break;
        case EStadiumType::T20:
            Capacity = 25000;
            BoundaryRadius = 65.0f;
            break;
        case EStadiumType::Indoor:
            Capacity = 20000;
            BoundaryRadius = 60.0f;
            bHasFloodlights = true;
            break;
    }
}

void AStadiumActor::SpawnCrowd()
{
    if (!CrowdMesh) return;

    int32 CrowdCount = (int32)(Capacity * CrowdDensity * 0.1f);
    float AngleStep = 360.0f / CrowdCount;
    float HeightStep = 5.0f;
    int32 Rings = 10;

    for (int32 Ring = 0; Ring < Rings; Ring++)
    {
        float Radius = 30.0f + (Ring * 5.0f);
        float Height = Ring * HeightStep;
        int32 ThisRingCount = (int32)(CrowdCount / Rings);

        for (int32 i = 0; i < ThisRingCount; i++)
        {
            float Angle = i * AngleStep;
            FVector Position(
                FMath::Cos(FMath::DegreesToRadians(Angle)) * Radius,
                FMath::Sin(FMath::DegreesToRadians(Angle)) * Radius,
                Height
            );

            FRotator Rotation = FRotator(0, Angle + 180, 0);
            CrowdMesh->AddInstance(FTransform(Rotation, Position));
        }
    }
}

void AStadiumActor::UpdateCrowdAnimation(float DeltaTime)
{
    // Update crowd instances for dynamic animation
    // Would use material parameters or transform updates
}

void AStadiumActor::UpdateLighting()
{
    // Update time-based lighting
    if (bIsDayNight)
    {
        float SunAngle = (TimeOfDay - 6.0f) * 15.0f;
        // Adjust directional light based on time
    }
}

void AStadiumActor::SetTimeOfDay(float Hour)
{
    TimeOfDay = FMath::Clamp(Hour, 0.0f, 24.0f);
}

void AStadiumActor::SetWeather(FString WeatherType)
{
    // Update weather effects
    UE_LOG(LogCricket, Log, TEXT("Weather set to: %s"), *WeatherType);
}

void AStadiumActor::UpdatePitchCondition(float OverNumber)
{
    // Degrade pitch as overs progress
    PitchProperties.Hardness = FMath::Clamp(0.5f - (OverNumber * 0.002f), 0.2f, 1.0f);
    PitchProperties.Cracks = FMath::Clamp(0.2f + (OverNumber * 0.01f), 0.0f, 1.0f);
    
    if (OverNumber > 60)
        PitchProperties.Moisture = FMath::Clamp(PitchProperties.Moisture - 0.001f, 0.0f, 1.0f);
}

void AStadiumActor::SetCrowdDensity(float Density)
{
    CrowdDensity = FMath::Clamp(Density, 0.0f, 1.0f);
    SpawnCrowd();
}

void AStadiumActor::TriggerCrowdReaction(FString ReactionType)
{
    if (ReactionType == "Six")
    {
        // Celebrate six
    }
    else if (ReactionType == "Wicket")
    {
        // Cheer for wicket
    }
    else if (ReactionType == "Boundary")
    {
        // Applaud boundary
    }
}

void AStadiumActor::UpdateWind(FVector Direction, float Speed)
{
    WindDir = Direction.GetSafeNormal();
    WindSpeed = FMath::Clamp(Speed, 0.0f, 50.0f);
}

float AStadiumActor::GetBoundaryDistance() const
{
    return BoundaryRadius;
}

FVector AStadiumActor::GetBoundaryCenter() const
{
    return FVector(0, 0, BoundaryHeight);
}
