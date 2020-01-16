// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawnBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackEvent, bool, ChargeAttack);

UCLASS()
class ICEBOXING_API APlayerPawnBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawnBase();

	void SetPlayerControllerIndex(int index);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ProcessPawnMovement();

	void ProcessCooldowns(float DeltaTime);

	void RotatePawnTowardsMovement();


	UFUNCTION(BlueprintCallable)
	void CheckImpact();

	TArray<AActor*> GetAttackReceivers();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ReceiveAttack(APlayerPawnBase* attacker, float pushForce);

	//Input functions
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);

	void ChargeAttack();
	void ReleaseAttack();
	void SideDodge();

	void DisablePlayerPawn();

	bool IsHiddenInGame = false;

protected:

	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnAttackEvent OnAttackEvent;

	UPROPERTY(EditDefaultsOnly)
		float attackMaxCooldown = 3.0f;

	UPROPERTY(EditDefaultsOnly)
		float attackMinCooldown = 1.0f;

	UPROPERTY(EditDefaultsOnly)
		float attackPushForce = 5000.0f;

	UPROPERTY(EditDefaultsOnly)
		float sideDodgeCooldown = 3.0f;

	UPROPERTY(EditDefaultsOnly)
		float sideDodgeForce = 100.0f;

	UPROPERTY(EditDefaultsOnly)
		float collisionForce = 0.5f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float acceleration = 0.1f;

	UPROPERTY(EditDefaultsOnly)
		float maxGoalMovement = 1000.0f;

	UPROPERTY(BlueprintReadOnly)
		FVector2D movementGoal;

	UPROPERTY(BlueprintReadOnly)
		float attackCooldownPercentage = 0.0f;

	UPROPERTY(BlueprintReadOnly)
		float attackChargePercentage = 0.0f;

	UPROPERTY(BlueprintReadOnly)
		float dodgeCooldownPercentage = 0.0f;	

	UPROPERTY(BlueprintReadOnly)
	bool isAttacking;

	float currentSideDodgeCooldown;

	float currentAttackCharge;
	float currentAttackCooldown;
	float startAttackTime;
		
	FVector2D currentMovement;
};
