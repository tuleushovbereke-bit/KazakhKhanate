// Private/EnemyBase.cpp
#include "EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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

    // Включаем физику на меше — персонаж падает
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    GetMesh()->SetSimulatePhysics(true);

    // Отключаем капсулу чтобы не мешала
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Отключаем движение
    GetCharacterMovement()->DisableMovement();

    // Убираем актора через 3 секунды
    SetLifeSpan(3.f);
}