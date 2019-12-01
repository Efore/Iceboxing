// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomPlayerController.h"
#include "PlayerPawnBase.h"

void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveX", this, &ACustomPlayerController::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &ACustomPlayerController::Move_YAxis);
	InputComponent->BindAction("Attack", IE_Pressed, this, &ACustomPlayerController::ChargeAttack);
	InputComponent->BindAction("Attack", IE_Released, this, &ACustomPlayerController::ReleaseAttack);
	InputComponent->BindAction("Dodge", IE_Pressed, this, &ACustomPlayerController::SideDodge);
}

void ACustomPlayerController::Move_XAxis(float AxisValue)
{
	PlayerPawnBase->Move_XAxis(AxisValue);
}

void ACustomPlayerController::Move_YAxis(float AxisValue)
{
	PlayerPawnBase->Move_YAxis(AxisValue);
}

void ACustomPlayerController::ChargeAttack()
{
	PlayerPawnBase->ChargeAttack();
}

void ACustomPlayerController::ReleaseAttack()
{
	PlayerPawnBase->ReleaseAttack();
}


void ACustomPlayerController::SideDodge()
{
	PlayerPawnBase->SideDodge();
}

void ACustomPlayerController::OnPossess(APawn * aPawn)
{
	Super::OnPossess(aPawn);

	PlayerPawnBase = dynamic_cast<APlayerPawnBase*>(aPawn);
}


