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
#include "Components/CapsuleComponent.h"

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

    HitDamage = 25.f;
    HitRange = 150.f;
    HitRadius = 60.f;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

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
            FVector Start = GetActorLocation() + FVector(0, 0, 40.f);   // от головы игрока, а не от пояса
            FVector Direction = LockOnTarget->GetActorLocation() - GetActorLocation();
            FRotator LookAt = Direction.Rotation();
            LookAt.Pitch = -15.f;
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
        EIC->BindAction(DodgeAction, ETriggerEvent::Started, this, &AMyCharacter::Dodge);
        EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyCharacter::StartSprint);
        EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyCharacter::StopSprint);
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

void AMyCharacter::Dodge()
{
    // Катиться можно из покоя ИЛИ из атаки (отмена атаки в перекат), при наличии стамины
    if ((CombatState != ECombatState::Idle && CombatState != ECombatState::Attacking)
        || Stamina < DodgeStaminaCost)
    {
        return;
    }

    // Если катимся из атаки — оборвать её: остановить монтаж, сбросить таймер, закрыть окно удара
    if (CombatState == ECombatState::Attacking)
    {
        StopAnimMontage(AttackMontage);
        GetWorldTimerManager().ClearTimer(AttackTimerHandle);
        bHitWindowOpen = false;
    }

    CombatState = ECombatState::Dodging;
    Stamina -= DodgeStaminaCost;
    StartIFrames();

    // Направление переката: куда жмёшь WASD, а если стоишь — назад
    FVector DodgeDir = GetLastMovementInputVector();
    if (DodgeDir.IsNearlyZero())
    {
        DodgeDir = -GetActorForwardVector();
    }
    DodgeDir.Normalize();

    SetActorRotation(DodgeDir.Rotation());
    LaunchCharacter(DodgeDir * DodgeImpulse, true, false);

    float Duration = PlayAnimMontage(DodgeMontage);
    if (Duration <= 0.f)
    {
        Duration = DodgeDuration;
    }

    GetWorldTimerManager().SetTimer(
        DodgeTimerHandle, this, &AMyCharacter::StopDodge, Duration, false);
}

void AMyCharacter::StartIFrames()
{
    bIsInvincible = true;
    GetWorldTimerManager().SetTimer(
        IFrameTimerHandle, this, &AMyCharacter::StopIFrames, IFrameDuration, false);
}

void AMyCharacter::StopIFrames()
{
    bIsInvincible = false;
}

void AMyCharacter::StopDodge()
{
    if (CombatState == ECombatState::Dodging)
    {
        CombatState = ECombatState::Idle;
    }
    bIsInvincible = false;
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

void AMyCharacter::OnDeath()
{
    Super::OnDeath();

    // Отключаем ввод
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }

    // Рэгдолл как у врага
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    GetMesh()->SetSimulatePhysics(true);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->DisableMovement();

    // Через 3 секунды — возрождение
    GetWorldTimerManager().SetTimer(
        AttackTimerHandle, this, &AMyCharacter::Respawn, 3.f, false);
}

void AMyCharacter::Respawn()
{
    // 1. Вернуть здоровье и стамину
    Health = MaxHealth;
    Stamina = MaxStamina;
    CombatState = ECombatState::Idle;

    // 2. Выйти из рэгдолла: выключить физику меша
    GetMesh()->SetSimulatePhysics(false);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    // 3. Вернуть меш в стойку относительно капсулы.
    //    Заводское смещение меша берём из CDO — эталонного экземпляра класса.
    const ACharacter* Default = GetClass()->GetDefaultObject<ACharacter>();
    GetMesh()->AttachToComponent(
        GetCapsuleComponent(),
        FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    GetMesh()->SetRelativeLocationAndRotation(
        Default->GetMesh()->GetRelativeLocation(),
        Default->GetMesh()->GetRelativeRotation());

    // 4. Вернуть капсуле коллизию и движение
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    

    // 5. Найти очаг по тегу и телепортировать к нему
    TArray<AActor*> FoundOchags;
    UGameplayStatics::GetAllActorsWithTag(this, FName("Ochag"), FoundOchags);
    if (FoundOchags.Num() > 0)
    {
        const FRotator OchagYaw(0.f, FoundOchags[0]->GetActorRotation().Yaw, 0.f);
        SetActorLocationAndRotation(FoundOchags[0]->GetActorLocation(), OchagYaw);
        if (AController* C = GetController())
        {
            C->SetControlRotation(OchagYaw);
           
        }

        // 5b. Сброс врагов
        TArray<AActor*> OldEnemies;
        UGameplayStatics::GetAllActorsWithTag(this, FName("Enemy"), OldEnemies);
        for (AActor* Enemy : OldEnemies)
        {
            Enemy->Destroy();
        }

        if (UFunction* SpawnFunc = FoundOchags[0]->FindFunction(FName("SpawnEnemies")))
        {
            FoundOchags[0]->ProcessEvent(SpawnFunc, nullptr);
        }
    }                                          // ← ВОТ ЭТОЙ скобки не хватает (закрывает блок 5)

    // 6. Вернуть ввод
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        EnableInput(PC);
    }
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    // Неуязвим в i-frames — урон игнорируем
    if (bIsInvincible)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("DODGE! (i-frames)"));
        }
        return 0.f;
    }

    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AMyCharacter::StartSprint()
{
    // Бежать можно только в покое (не в атаке/блоке/перекате) и при наличии стамины
    if (CombatState != ECombatState::Idle || Stamina <= 0.f)
    {
        return;
    }

    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AMyCharacter::StopSprint()
{
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AMyCharacter::StartWalk()
{
    if (CombatState != ECombatState::Idle) return;
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("WALK START"));

    bIsWalking = true;
    bIsSprinting = false;   // шаг отменяет спринт
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AMyCharacter::StopWalk()
{
    if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("WALK STOP"));
    bIsWalking = false;
    GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}