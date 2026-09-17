#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "Engine/OverlapResult.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Animation/AnimMontage.h"

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 300.0f;
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();

    Stamina = MaxStamina;
    DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CombatState == ECombatState::Idle)
    {
        Stamina = FMath::Clamp(Stamina + StaminaRegenPerSecond * DeltaTime, 0.f, MaxStamina);
    }

    if (bIsLockedOn && LockOnTarget)
    {
        if (!LockOnTarget->IsActorBeingDestroyed() && LockOnTarget->IsHidden())
        {
            bIsLockedOn = false;
            LockOnTarget = nullptr;
            bUseControllerRotationYaw = false;
            GetCharacterMovement()->bOrientRotationToMovement = true;
        }
        else
        {
            FVector Direction = (LockOnTarget->GetActorLocation() + FVector(0, 0, 60.f)) - GetActorLocation();
            FRotator LookAt = Direction.Rotation();
            FRotator Current = GetControlRotation();
            FRotator NewRot = FMath::RInterpTo(Current, LookAt, DeltaTime, 10.f);
            GetController()->SetControlRotation(NewRot);
        }
    }

    DrawDebugState();
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
        EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
        EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &AMyCharacter::Attack);
        EIC->BindAction(BlockAction, ETriggerEvent::Started, this, &AMyCharacter::Block);
        EIC->BindAction(BlockAction, ETriggerEvent::Completed, this, &AMyCharacter::StopBlock);
        EIC->BindAction(LockOnAction, ETriggerEvent::Started, this, &AMyCharacter::ToggleLockOn);
    }
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
    if (CombatState == ECombatState::Attacking)
    {
        return;
    }

    const FVector2D Input = Value.Get<FVector2D>();

    if (Controller == nullptr)
    {
        return;
    }

    const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
    const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(ForwardDir, Input.Y);
    AddMovementInput(RightDir, Input.X);
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D Axis = Value.Get<FVector2D>();

    AddControllerYawInput(Axis.X);
    AddControllerPitchInput(Axis.Y);
}

void AMyCharacter::Attack()
{
    if (CombatState != ECombatState::Idle || Stamina < AttackStaminaCost)
    {
        return;
    }

    CombatState = ECombatState::Attacking;
    Stamina -= AttackStaminaCost;

    float Duration = PlayAnimMontage(AttackMontage);
    if (Duration <= 0.f)
    {
        Duration = AttackDuration;
    }

    GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AMyCharacter::StopAttack, Duration, false);
}

void AMyCharacter::StopAttack()
{
    if (CombatState == ECombatState::Attacking)
    {
        CombatState = ECombatState::Idle;
    }
    bHitWindowOpen = false;
}

void AMyCharacter::Block()
{
    if (CombatState != ECombatState::Idle)
    {
        return;
    }

    CombatState = ECombatState::Blocking;
    GetCharacterMovement()->MaxWalkSpeed = BlockWalkSpeed;
}

void AMyCharacter::StopBlock()
{
    if (CombatState == ECombatState::Blocking)
    {
        CombatState = ECombatState::Idle;
    }
    GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
}

void AMyCharacter::DrawDebugState() const
{
    if (GEngine == nullptr)
    {
        return;
    }

    const FString StateName = UEnum::GetDisplayValueAsText(CombatState).ToString();
    const FString Msg = FString::Printf(
        TEXT("State: %s   Stamina: %.0f / %.0f   HitWindow: %s   HP: %.0f"),
        *StateName, Stamina, MaxStamina,
        bHitWindowOpen ? TEXT("OPEN") : TEXT("-"),
        Health);

    GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Yellow, Msg);
}

void AMyCharacter::SetHitWindowOpen(bool bOpen)
{
    bHitWindowOpen = bOpen;

    if (!bOpen) return;

    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * 150.f;

    TArray<FHitResult> Hits;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(60.f);

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
                    HitActor, 25.f, GetController(), this, nullptr);
                break;
            }
        }
    }
}

void AMyCharacter::ToggleLockOn()
{
    if (bIsLockedOn)
    {
        bIsLockedOn = false;
        LockOnTarget = nullptr;
        bUseControllerRotationYaw = false;
        GetCharacterMovement()->bOrientRotationToMovement = true;
        return;
    }

    TArray<FOverlapResult> Overlaps;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(LockOnRange);

    GetWorld()->OverlapMultiByChannel(
        Overlaps, GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere);

    AActor* ClosestEnemy = nullptr;
    float ClosestDist = LockOnRange;

    for (FOverlapResult& Overlap : Overlaps)
    {
        AActor* OtherActor = Overlap.GetActor();
        if (OtherActor && OtherActor != this && OtherActor->ActorHasTag(FName("Enemy")))
        {
            float Dist = FVector::Dist(GetActorLocation(), OtherActor->GetActorLocation());
            if (Dist < ClosestDist)
            {
                ClosestDist = Dist;
                ClosestEnemy = OtherActor;
            }
        }
    }

    if (ClosestEnemy)
    {
        bIsLockedOn = true;
        LockOnTarget = ClosestEnemy;
        bUseControllerRotationYaw = true;
        GetCharacterMovement()->bOrientRotationToMovement = false;
    }
}