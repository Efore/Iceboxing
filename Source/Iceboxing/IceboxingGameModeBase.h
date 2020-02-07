// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IceboxingGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ICEBOXING_API AIceboxingGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString & Options) override;

	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, class AActor* StartSpot) override;

	virtual void StartPlay() override;
	
protected:

	void ManageControllers();

	void SpawnPlayerPawns();

	TArray<APlayerController*> m_playerControllers;
	TArray<TTuple<FVector, FQuat>> m_playerStartPositions;
	APlayerController* m_firstPlayerController; 

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1", ClampMax = "4"), Category = "Params")
		int NumberOfPlayers;
};
