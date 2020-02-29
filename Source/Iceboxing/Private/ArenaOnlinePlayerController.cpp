// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaOnlinePlayerController.h"
#include "ArenaPlayerPawn.h"
#include "ArenaOnlineGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "Net/UnrealNetwork.h"


void AArenaOnlinePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController())
	{		
		RPC_IsReadyForServer();
	}
}

void AArenaOnlinePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (IsLocalController())
	{
		InputComponent->BindAxis("MoveX", this, &AArenaOnlinePlayerController::Move_XAxis);
		InputComponent->BindAxis("MoveY", this, &AArenaOnlinePlayerController::Move_YAxis);
		InputComponent->BindAction("Attack", IE_Pressed, this, &AArenaOnlinePlayerController::ChargeAttack);
		InputComponent->BindAction("Attack", IE_Released, this, &AArenaOnlinePlayerController::ReleaseAttack);
		InputComponent->BindAction("Dodge", IE_Pressed, this, &AArenaOnlinePlayerController::SideDodge);
	}
}

void AArenaOnlinePlayerController::Move_XAxis(float AxisValue)
{
	if (!IsLocalController())
		return;
	
	if (ArenaPlayerPawn) {

		ArenaPlayerPawn->Move_XAxis(AxisValue);
	}
}

void AArenaOnlinePlayerController::Move_YAxis(float AxisValue)
{
	if (!IsLocalController())
		return;

	if (ArenaPlayerPawn)
		ArenaPlayerPawn->Move_YAxis(AxisValue);
}

void AArenaOnlinePlayerController::ChargeAttack()
{
	if (!IsLocalController())
		return;

	if (ArenaPlayerPawn)
		ArenaPlayerPawn->ChargeAttack();  
}

void AArenaOnlinePlayerController::ReleaseAttack()
{
	if (!IsLocalController())
		return;

	if (ArenaPlayerPawn)
		ArenaPlayerPawn->ReleaseAttack();
}

void AArenaOnlinePlayerController::SideDodge()
{
	if (!IsLocalController())
		return;

	if (ArenaPlayerPawn)
		ArenaPlayerPawn->SideDodge();
}

void AArenaOnlinePlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (GetNetMode() == NM_ListenServer)
	{
		ArenaPlayerPawn = dynamic_cast<AArenaPlayerPawn*>(aPawn);

		ACameraActor* camera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass()));

		if (camera)
			SetViewTarget(camera);
	}
}

void AArenaOnlinePlayerController::RPC_IsReadyForServer_Implementation()
{	
	AArenaOnlineGameModeBase* arenaOnlineGM = Cast<AArenaOnlineGameModeBase>(GetWorld()->GetAuthGameMode());
	APlayerStart* playerStart = arenaOnlineGM->RequestPlayerStart();

	arenaOnlineGM->RestartPlayerAtPlayerStart(this, playerStart);
}

bool AArenaOnlinePlayerController::RPC_IsReadyForServer_Validate()
{
	return true;
}

void AArenaOnlinePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		
	DOREPLIFETIME(AArenaOnlinePlayerController, ArenaPlayerPawn);
}


