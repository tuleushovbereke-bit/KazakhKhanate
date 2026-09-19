#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Engine/Engine.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    HitDamage = 20.f;
    HitRange = 150.f;
    HitRadius = 60.f;
    
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(4, 3.f, FColor::Green,
            FString::Printf(TEXT("%s spawned, HP: %.0f"), *GetName(), Health));
    }

    GetWorldTimerManager().SetTimer(
        AttackTimerHandle, this, &AEnemyBase::TryAttackPlayer,
        AttackCooldown, true);

    GetWorldTimerManager().SetTimer(
        MoveTimerHandle, this, &AEnemyBase::TryMoveToPlayer,
        0.5f, true);
}

void AEnemyBase::OnDeath()
{
    Super::OnDeath();

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    GetMesh()->SetSimulatePhysics(true);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->DisableMovement();
    SetLifeSpan(3.f);
}

void AEnemyBase::TryAttackPlayer()
{
    if (!IsAlive()) return;

    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    if (Distance > AttackRange) return;

    // Играем анимацию удара
    PlayAnimMontage(AttackMontage);
}

void AEnemyBase::TryMoveToPlayer()
{
    if (!IsAlive()) return;

    AActor* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player) return;

    float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    if (Distance > DetectionRange) return;

    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        AI->MoveToActor(Player, 100.f);
    }
}