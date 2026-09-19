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
    float GetHealthPercent() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsAlive() const { return Health > 0.f; }

    // Окно удара: вызывается из AnimNotifyState в ударной фазе анимации
    void SetHitWindowOpen(bool bOpen);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsHitWindowOpen() const { return bHitWindowOpen; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    float Health = 100.f;

    // Параметры удара (каждый класс задаёт свои)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float HitDamage = 20.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float HitRange = 150.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float HitRadius = 60.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bHitWindowOpen = false;

    // Вызывается когда здоровье упало до нуля
    virtual void OnDeath();
};