// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaGameState.h"
#include "ArenaPlayerPawn.h"
#include "Net/UnrealNetwork.h"

void AArenaGameState::RegisterPlayingPawn(AArenaPlayerPawn* playerPawn)
{
	m_playingPawns.Add(playerPawn);
	OnPawnRegisteredEvent.Broadcast(playerPawn, m_playingPawns.Num());
}

void AArenaGameState::UnregisterPlayingPawn(AArenaPlayerPawn* playerPawn)
{
	OnPawnDestroyedEvent.Broadcast(playerPawn);
	m_playingPawns.Remove(playerPawn);
	m_spectatorPawns.Add(playerPawn);
}

void AArenaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaGameState, m_playingPawns);
	DOREPLIFETIME(AArenaGameState, m_spectatorPawns);
}