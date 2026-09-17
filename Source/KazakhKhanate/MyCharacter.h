// MyCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "ABaseCharacter.h"
#include "MyCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
class UGameplayStatics;
class AActor;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Attacking   UMETA(DisplayName = "Attacking"),
    Blocking    UMETA(DisplayName = "Blocking"),
    Staggered   UMETA(DisplayName = "Staggered")
};

UCLASS()
class KAZAKHKHANATE_API AMyCharacter : public ABaseCharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void OnDeath() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Attack();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Block();

    void StopAttack();
    void StopBlock();

    UFUNCTION(BlueprintPure, Category = "Combat")
    ECombatState GetCombatState() const { return CombatState; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetStamina() const { return Stamina; }

    // ¬ызываетс€ из AnimNotifyState_HitWindow
    void SetHitWindowOpen(bool bOpen);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsHitWindowOpen() const { return bHitWindowOpen; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void ToggleLockOn();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* AttackAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* BlockAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* LockOnAction;



    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

    // ѕараметры бо€ Ч настраиваютс€ в Blueprint без перекомпил€ции
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* AttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float MaxStamina = 100.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float StaminaRegenPerSecond = 20.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float AttackStaminaCost = 25.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float AttackDuration = 0.6f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")   // <
        float BlockWalkSpeed = 200.f;                                          // <

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    ECombatState CombatState = ECombatState::Idle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    float Stamina = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))   // <
        bool bHitWindowOpen = false;                                                                                // <

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    bool bIsLockedOn = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    AActor* LockOnTarget = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
    float LockOnRange = 1000.f;

    float DefaultWalkSpeed = 600.f;   // <<<  запоминаем в BeginPlay, чтобы вернуть после блока

    FTimerHandle AttackTimerHandle;

    void DrawDebugState() const;
    void Respawn();
};