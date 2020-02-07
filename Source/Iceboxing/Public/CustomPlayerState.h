// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CustomPlayerState.generated.h"

UCLASS()
class ICEBOXING_API ACustomPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "CustomPlayerState")
	 void SetIsReadyInLobby(bool value) { isReadyInLobby = value; }
	
	
protected:
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool isReadyInLobby = false;	
};
