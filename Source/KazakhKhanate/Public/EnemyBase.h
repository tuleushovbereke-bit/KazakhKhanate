// Public/EnemyBase.h
#pragma once

#include "CoreMinimal.h"
#include "ABaseCharacter.h"
#include "EnemyBase.generated.h"

UCLASS()
class KAZAKHKHANATE_API AEnemyBase : public ABaseCharacter
{
    GENERATED_BODY()

public:
    AEnemyBase();

protected:
    virtual void BeginPlay() override;
    virtual void OnDeath() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float AttackDamage = 20.f;
};