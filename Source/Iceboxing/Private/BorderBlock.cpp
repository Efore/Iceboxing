// Fill out your copyright notice in the Description page of Project Settings.
#include "BorderBlock.h"

#include "DestructibleComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABorderBlock::ABorderBlock()
{	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	destructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Destructible Component"));
	destructibleComponent->SetNotifyRigidBodyCollision(true);	
	destructibleComponent->SetupAttachment(RootComponent);

	maxCollisionResistance = 10000.0f;
}

// Called when the game starts or when spawned
void ABorderBlock::BeginPlay()
{
	Super::BeginPlay();
	destructibleComponent->OnComponentHit.AddDynamic(this, &ABorderBlock::OnComponentHitCallback);	

	isDestroyed = false;
	currentCollisionResistance = maxCollisionResistance;
}

void ABorderBlock::OnComponentHitCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	if (isDestroyed)
		return;
	
	const float collisionForce = OtherActor->GetVelocity().Size();
	const float dot = FVector::DotProduct(NormalImpulse.GetSafeNormal(), OtherActor->GetVelocity().GetSafeNormal());
	
	//UE_LOG(LogTemp, Warning, TEXT("force %f"), collisionForce);	

	currentCollisionResistance -= dot * collisionForce;

	if (currentCollisionResistance <= 0.0f)
	{
		hitLocation = Hit.ImpactPoint;
		DestroyBlock(OtherActor);
	}
	
	//OtherComp->AddImpulse(NormalImpulse);
}


void ABorderBlock::DestroyBlock(AActor* destroyer)
{
	const FVector hitDirection = hitLocation - destroyer->GetActorLocation();
	destructibleComponent->ApplyDamage(100, hitLocation, GetActorLocation(), defaultPiecesImpulse);

	isDestroyed = true;		
}

