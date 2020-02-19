// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArenaOnlinePlayerController.generated.h"


/**
 *
 */
UCLASS()
class ICEBOXING_API AArenaOnlinePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	//Input functions
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);

	void ChargeAttack();
	void ReleaseAttack();
	void SideDodge();

	UFUNCTION(Server, Reliable, WithValidation)
	void IsReadyForServer();

public:

	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;

protected:

	UPROPERTY()
		class AArenaPlayerPawn* ArenaPlayerPawn;
};

