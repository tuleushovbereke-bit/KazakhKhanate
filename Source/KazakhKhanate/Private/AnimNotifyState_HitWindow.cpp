#include "AnimNotifyState_HitWindow.h"
#include "ABaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_HitWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (MeshComp == nullptr) return;

    if (ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner()))
    {
        Character->SetHitWindowOpen(true);
    }
}

void UAnimNotifyState_HitWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);

    if (MeshComp == nullptr) return;

    if (ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner()))
    {
        Character->SetHitWindowOpen(false);
    }
}