// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaOnlineGameModeBase.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "ArenaPlayerPawn.h"
#include "UMG.h"


APawn * AArenaOnlineGameModeBase::SpawnDefaultPawnFor_Implementation(AController * NewPlayer, AActor * StartSpot)
{
	//We override this method in order to have control over the player pawn's spawning
	return nullptr;
}

void AArenaOnlineGameModeBase::SwapPlayerControllers(APlayerController * OldPC, APlayerController * NewPC)
{
	m_connectedPlayers.Add(NewPC);
}

void AArenaOnlineGameModeBase::StartPlay()
{
	for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		m_playerStarts.Add(Cast<APlayerStart>(*ActorItr));
	}
}

APlayerStart* AArenaOnlineGameModeBase::RequestPlayerStart()
{
	return m_playerStarts.Pop(true);
}
