// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelGameState.h"
#include "PlayerPawnBase.h"

void ALevelGameState::RegisterPlayingPawn(APlayerPawnBase* playerPawn)
{
	PlayingPawns.Add(playerPawn);
	OnPawnRegisteredEvent.Broadcast(playerPawn, PlayingPawns.Num());
}

void ALevelGameState::UnregisterPlayingPawn(APlayerPawnBase* playerPawn)
{
	OnPawnDestroyedEvent.Broadcast(playerPawn);
	PlayingPawns.Remove(playerPawn);
	SpectatorPawns.Add(playerPawn);
}
