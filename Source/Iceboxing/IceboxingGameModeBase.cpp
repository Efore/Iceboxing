// Fill out your copyright notice in the Description page of Project Settings.


#include "IceboxingGameModeBase.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "PlayerPawnBase.h"
#include "UMG.h"


APlayerController* AIceboxingGameModeBase::SpawnPlayerController(ENetRole InRemoteRole, const FString & Options)
{
	//We override this method in order to have control over the Controllers' spawning
	//UE_LOG(LogTemp, Warning, TEXT("spawning player"));
	m_firstPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);	
	m_playerControllers.Add(m_firstPlayerController);
	return m_firstPlayerController;
}

APawn * AIceboxingGameModeBase::SpawnDefaultPawnFor_Implementation(AController * NewPlayer, AActor * StartSpot)
{
	//We override this method in order to have control over the player pawn's spawning
	return nullptr;
}

void AIceboxingGameModeBase::ManageControllers()
{
	int playersSpawned = 1;
	for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (playersSpawned < NumberOfPlayers)
		{
			APlayerController* NewController = (UGameplayStatics::CreatePlayer(GetWorld(), -1));
			m_playerControllers.Add(NewController);			
		}

		m_playerStartPositions.Add(TTuple<FVector, FQuat>(Cast<APlayerStart>(*ActorItr)->GetActorLocation(),
			Cast<APlayerStart>(*ActorItr)->GetActorQuat()));

		++playersSpawned;
	}
}

void AIceboxingGameModeBase::SpawnPlayerPawns()
{
	for (int i = 0; i < m_playerStartPositions.Num(); ++i)    
	{
		//For each start position, a Pawn is created with the transform information of that start position
		FTransform transform(m_playerStartPositions[i].Value, m_playerStartPositions[i].Key);

		auto playerPawn = (UGameplayStatics::BeginDeferredActorSpawnFromClass(this, DefaultPawnClass, transform));

		if (playerPawn != nullptr)
		{
			//If the current index is lower than the max Number of players, the Pawn is configured to be played by a human player
			if (i < NumberOfPlayers)
			{
				Cast<APlayerPawnBase>(playerPawn)->SetPlayerControllerIndex(i);				
			}

			UGameplayStatics::FinishSpawningActor(playerPawn, transform);

			//If the current index is lower than the max Number of players, the Pawn is possesed by a PlayerController
			if (i < NumberOfPlayers)
			{
				m_playerControllers[i]->Possess(Cast<APlayerPawnBase>(playerPawn)); 
			}
		}
	}
}

void AIceboxingGameModeBase::StartPlay()
{	
	ManageControllers();
	SpawnPlayerPawns();
	Super::StartPlay();
}