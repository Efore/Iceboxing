// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaGameState.h"
#include "ArenaPlayerPawn.h"

void AArenaGameState::RegisterPlayingPawn(AArenaPlayerPawn* playerPawn)
{
	PlayingPawns.Add(playerPawn);
	OnPawnRegisteredEvent.Broadcast(playerPawn, PlayingPawns.Num());
}

void AArenaGameState::UnregisterPlayingPawn(AArenaPlayerPawn* playerPawn)
{
	OnPawnDestroyedEvent.Broadcast(playerPawn);
	PlayingPawns.Remove(playerPawn);
	SpectatorPawns.Add(playerPawn);
}
