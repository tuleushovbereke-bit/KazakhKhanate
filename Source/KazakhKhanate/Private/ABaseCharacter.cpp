// Private/BaseCharacter.cpp
#include "ABaseCharacter.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"


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

void ABaseCharacter::SetHitWindowOpen(bool bOpen)
{
    bHitWindowOpen = bOpen;

    if (!bOpen) return;

    // Свип вперёд от персонажа в момент удара
    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * HitRange;

    TArray<FHitResult> Hits;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(HitRadius);

    bool bHit = GetWorld()->SweepMultiByChannel(
        Hits, Start, End, FQuat::Identity, ECC_Pawn, Sphere);

    if (bHit)
    {
        for (FHitResult& Hit : Hits)
        {
            AActor* HitActor = Hit.GetActor();
            if (HitActor && HitActor != this)
            {
                UGameplayStatics::ApplyDamage(
                    HitActor, HitDamage, GetController(), this, nullptr);
                break;
            }
        }
    }
}