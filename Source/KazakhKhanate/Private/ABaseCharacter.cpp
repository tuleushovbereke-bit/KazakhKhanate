// Private/BaseCharacter.cpp
#include "ABaseCharacter.h"
#include "Engine/Engine.h"

ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Red,
            FString::Printf(TEXT("%s took %.0f damage, HP: %.0f"), *GetName(), DamageAmount, Health));
    }

    if (Health <= 0.f)
    {
        OnDeath();
    }

    return DamageAmount;
}

void ABaseCharacter::OnDeath()
{
    // Пока просто логируем — анимация смерти будет в этапе 4 конец
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Orange,
            FString::Printf(TEXT("%s died!"), *GetName()));
    }
}