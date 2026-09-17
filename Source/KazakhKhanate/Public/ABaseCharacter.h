// Public/BaseCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABaseCharacter.generated.h"

UCLASS()
class KAZAKHKHANATE_API ABaseCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABaseCharacter();

    // Принять урон — стандартный метод движка
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsAlive() const { return Health > 0.f; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    float Health = 100.f;

    // Вызывается когда здоровье упало до нуля
    virtual void OnDeath();
};