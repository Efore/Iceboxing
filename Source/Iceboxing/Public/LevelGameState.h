// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LevelGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPawnRegistered, class APlayerPawnBase*, playerPawn, int, currentNumOfPlayers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnDestroyed, class APlayerPawnBase*, playerPawn);

/**
 * 
 */
UCLASS()
class ICEBOXING_API ALevelGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Level GameState")
		TSet<class APlayerPawnBase*> PlayingPawns;

	UPROPERTY(BlueprintReadOnly, Category = "Level GameState")
		TSet<class APlayerPawnBase*> SpectatorPawns;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnPawnRegistered OnPawnRegisteredEvent;

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnPawnDestroyed OnPawnDestroyedEvent;

public:
	
	void RegisterPlayingPawn(class APlayerPawnBase* playerPawn);
	
	void UnregisterPlayingPawn(class APlayerPawnBase* playerPawn);
	
};
