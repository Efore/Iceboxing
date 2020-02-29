// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ArenaPlayerPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackEvent, bool, ChargeAttack);

UCLASS()
class ICEBOXING_API AArenaPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AArenaPlayerPawn();

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

	UFUNCTION(Server, Unreliable)
		void RPC_SendXAxisValueToServer(float axisValue);

	UFUNCTION(Server, Unreliable)
		void RPC_SendYAxisValueToServer(float axisValue);

	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_SendChargeAttackToServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_SendReleaseAttackToServer();

	UFUNCTION(Server, Reliable, WithValidation)
		void RPC_SendSideDodgeToServer();

	UFUNCTION()
		void OnRep_isAttacking();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ReceiveAttack(AArenaPlayerPawn* attacker, float pushForce);

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

	UPROPERTY(BlueprintReadOnly, Replicated)
		FVector2D movementGoal;

	UPROPERTY(BlueprintReadOnly, Replicated)
		float attackCooldownPercentage = 0.0f;

	UPROPERTY(BlueprintReadOnly, Replicated)
		float attackChargePercentage = 0.0f;

	UPROPERTY(BlueprintReadOnly, Replicated)
		float dodgeCooldownPercentage = 0.0f;	

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_isAttacking)
	bool isAttacking;

	float currentSideDodgeCooldown;

	float currentAttackCharge;
	float currentAttackCooldown;
	float startAttackTime;
		
	FVector2D currentMovement;
};
