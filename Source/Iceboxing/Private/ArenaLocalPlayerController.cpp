// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaLocalPlayerController.h"
#include "ArenaPlayerPawn.h"

void AArenaLocalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveX", this, &AArenaLocalPlayerController::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &AArenaLocalPlayerController::Move_YAxis);
	InputComponent->BindAction("Attack", IE_Pressed, this, &AArenaLocalPlayerController::ChargeAttack);
	InputComponent->BindAction("Attack", IE_Released, this, &AArenaLocalPlayerController::ReleaseAttack);
	InputComponent->BindAction("Dodge", IE_Pressed, this, &AArenaLocalPlayerController::SideDodge);
}

void AArenaLocalPlayerController::Move_XAxis(float AxisValue)
{
	if (ArenaPlayerPawn)
		ArenaPlayerPawn->Move_XAxis(AxisValue);
}

void AArenaLocalPlayerController::Move_YAxis(float AxisValue)
{
	if (ArenaPlayerPawn)
		ArenaPlayerPawn->Move_YAxis(AxisValue);
}

void AArenaLocalPlayerController::ChargeAttack()
{
	if (ArenaPlayerPawn)
		ArenaPlayerPawn->ChargeAttack();
}

void AArenaLocalPlayerController::ReleaseAttack()
{
	if (ArenaPlayerPawn)
		ArenaPlayerPawn->ReleaseAttack();
}

void AArenaLocalPlayerController::SideDodge()
{
	if (ArenaPlayerPawn)
		ArenaPlayerPawn->SideDodge();
}

void AArenaLocalPlayerController::OnPossess(APawn * aPawn)
{
	Super::OnPossess(aPawn);
	ArenaPlayerPawn = dynamic_cast<AArenaPlayerPawn*>(aPawn);
}


