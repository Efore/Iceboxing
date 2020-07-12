// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BorderBlock.generated.h"

UCLASS()
class ICEBOXING_API ABorderBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABorderBlock();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnComponentHitCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void RPC_SendDestroyBlockToClients(AActor* destroyer, float force, FVector hitPoint);
	
	UFUNCTION(BlueprintCallable)
	void DestroyBlock(AActor* destroyer, float force);

protected:
	
	UPROPERTY(EditDefaultsOnly)
		float maxCollisionResistance;

	UPROPERTY(EditDefaultsOnly)
		float defaultPiecesImpulse;

	UPROPERTY(EditDefaultsOnly)
		float maxSpeedForBalancing;

	UPROPERTY(VisibleAnywhere)
		class UDestructibleComponent* destructibleComponent;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* boxComponent;

	UPROPERTY(BlueprintReadOnly)
		bool isDestroyed;

	float currentCollisionResistance;

	FVector hitLocation;
};
