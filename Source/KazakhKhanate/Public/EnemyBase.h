// Public/EnemyBase.h
#pragma once

#include "CoreMinimal.h"
#include "ABaseCharacter.h"
#include "AIController.h"
#include "EnemyBase.generated.h"

class UAnimMontage;

UCLASS()
class KAZAKHKHANATE_API AEnemyBase : public ABaseCharacter
{
    GENERATED_BODY()

public:
    AEnemyBase();

protected:
    virtual void BeginPlay() override;
    virtual void OnDeath() override;
    // Таймер атаки
    FTimerHandle AttackTimerHandle;
    float AttackRange = 150.f;
    float AttackCooldown = 2.f;

    void TryAttackPlayer();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float AttackDamage = 20.f;
    // Движение к игроку
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    float DetectionRange = 600.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* AttackMontage;

    FTimerHandle MoveTimerHandle;

    void TryMoveToPlayer();
};