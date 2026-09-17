// Private/EnemyBase.cpp
#include "EnemyBase.h"
#include "Engine/Engine.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(4, 3.f, FColor::Green,
            FString::Printf(TEXT("%s spawned, HP: %.0f"), *GetName(), Health));
    }
}

void AEnemyBase::OnDeath()
{
    Super::OnDeath();

    // ѕока просто убираем с уровн€ через 2 секунды
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}