// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaPlayerController.h"
#include "ArenaPlayerPawn.h"

void AArenaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveX", this, &AArenaPlayerController::Move_XAxis);
	InputComponent->BindAxis("MoveY", this, &AArenaPlayerController::Move_YAxis);
	InputComponent->BindAction("Attack", IE_Pressed, this, &AArenaPlayerController::ChargeAttack);
	InputComponent->BindAction("Attack", IE_Released, this, &AArenaPlayerController::ReleaseAttack);
	InputComponent->BindAction("Dodge", IE_Pressed, this, &AArenaPlayerController::SideDodge);
}

void AArenaPlayerController::Move_XAxis(float AxisValue)
{
	ArenaPlayerPawn->Move_XAxis(AxisValue);
}

void AArenaPlayerController::Move_YAxis(float AxisValue)
{
	ArenaPlayerPawn->Move_YAxis(AxisValue);
}

void AArenaPlayerController::ChargeAttack()
{
	ArenaPlayerPawn->ChargeAttack();  
}

void AArenaPlayerController::ReleaseAttack()
{
	ArenaPlayerPawn->ReleaseAttack();
}


void AArenaPlayerController::SideDodge()
{
	ArenaPlayerPawn->SideDodge();
}

void AArenaPlayerController::OnPossess(APawn * aPawn)
{
	Super::OnPossess(aPawn);

	ArenaPlayerPawn = dynamic_cast<AArenaPlayerPawn*>(aPawn);
}


