// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArenaGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPawnRegistered, class AArenaPlayerPawn*, playerPawn, int, currentNumOfPlayers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnDestroyed, class AArenaPlayerPawn*, playerPawn);

/**
 * 
 */
UCLASS()
class ICEBOXING_API AArenaGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Level GameState")
		TArray<class AArenaPlayerPawn*> m_playingPawns;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Level GameState")
		TArray<class AArenaPlayerPawn*> m_spectatorPawns;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnPawnRegistered OnPawnRegisteredEvent;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnPawnDestroyed OnPawnDestroyedEvent;

public:
	
	void RegisterPlayingPawn(class AArenaPlayerPawn* playerPawn);
	
	void UnregisterPlayingPawn(class AArenaPlayerPawn* playerPawn);
	
};
