// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaOnlineGameModeBase.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "ArenaPlayerPawn.h"
#include "UMG.h"

void AArenaOnlineGameModeBase::StartPlay()
{
	for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		m_playerStarts.Add(Cast<APlayerStart>(*ActorItr));
	}

	Super::StartPlay();
}

void AArenaOnlineGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	m_connectedPlayers.Add(NewPlayer);
}

APlayerStart* AArenaOnlineGameModeBase::RequestPlayerStart()
{
	return m_playerStarts.Pop(true);
}
