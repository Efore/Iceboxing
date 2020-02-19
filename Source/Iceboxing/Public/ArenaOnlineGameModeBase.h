// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaOnlineGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ICEBOXING_API AArenaOnlineGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, class AActor* StartSpot) override;

	virtual void StartPlay() override;

	class APlayerStart* RequestPlayerStart();

private:

	TArray<APlayerController*> m_connectedPlayers;
	TArray<class APlayerStart*> m_playerStarts;
};
