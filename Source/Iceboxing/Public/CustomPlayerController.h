// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"


/**
 *
 */
UCLASS()
class ICEBOXING_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	//Input functions
	void Move_XAxis(float AxisValue);
	void Move_YAxis(float AxisValue);

	void ChargeAttack();
		void ReleaseAttack();
	void SideDodge();

public:

	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* aPawn) override;

protected:

	UPROPERTY()
		class APlayerPawnBase* PlayerPawnBase;
};

