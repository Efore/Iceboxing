// Fill out your copyright notice in the Description page of Project Settings.
#include "BorderBlock.h"

#include "DestructibleComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "ArenaPlayerPawn.h"


// Sets default values
ABorderBlock::ABorderBlock()
{	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	destructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Destructible Component"));	
	destructibleComponent->SetupAttachment(RootComponent);	

	boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box component"));	
	boxComponent->SetNotifyRigidBodyCollision(true);
	boxComponent->SetupAttachment(RootComponent);

	maxCollisionResistance = 10000.0f;

	SetReplicates(true);
}
// Called when the game starts or when spawned
void ABorderBlock::BeginPlay()
{
	Super::BeginPlay();

	if (GetNetMode() == NM_Client)
	{
		FVector clientBoxExtent = boxComponent->GetUnscaledBoxExtent();
		clientBoxExtent.Y *= 0.5f;
		boxComponent->SetBoxExtent(clientBoxExtent);
		return;
	}

	boxComponent->OnComponentHit.AddDynamic(this, &ABorderBlock::OnComponentHitCallback);		

	isDestroyed = false;
	currentCollisionResistance = maxCollisionResistance;
}

void ABorderBlock::OnComponentHitCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	if (isDestroyed)
		return;
	
	const float collisionForce = OtherActor->GetVelocity().Size();
	const float dot = FVector::DotProduct(NormalImpulse.GetSafeNormal(), OtherActor->GetVelocity().GetSafeNormal());
	const float damage = dot * collisionForce;
	UE_LOG(LogTemp, Warning, TEXT("force %f"), damage);

	if (damage < 300.0f)
		return;

	currentCollisionResistance -= dot * collisionForce;

	if (currentCollisionResistance <= 0.0f)
	{
		hitLocation = Hit.ImpactPoint;
		DestroyBlock(OtherActor, collisionForce);
	}	
}


void ABorderBlock::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
	if (!isDestroyed || GetNetMode() == NM_Client)
		return;

	if ((maxSpeedForBalancing * maxSpeedForBalancing) > OtherActor->GetVelocity().SizeSquared())
	{
		AArenaPlayerPawn* overlapingPawn = dynamic_cast<AArenaPlayerPawn*>(OtherActor);
		
		if (overlapingPawn)
			overlapingPawn->StartBalancing();
	}
}

void ABorderBlock::DestroyBlock(AActor* destroyer, float force)
{
	const FVector hitDirection = hitLocation - destroyer->GetActorLocation();

	boxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);	
	boxComponent->SetCollisionResponseToChannels(ECollisionResponse::ECR_Overlap);
	boxComponent->SetGenerateOverlapEvents(true);

	FVector newBoxExtent = boxComponent->GetUnscaledBoxExtent();
	newBoxExtent.Y *= 0.5f;
	boxComponent->SetBoxExtent(newBoxExtent);

	destructibleComponent->ApplyDamage(100, hitLocation, GetActorLocation(), force);

	isDestroyed = true;		

	RPC_SendDestroyBlockToClients(destroyer, force, hitLocation);
}

void ABorderBlock::RPC_SendDestroyBlockToClients_Implementation(AActor* destroyer, float force, FVector hitPoint)
{
	if (GetNetMode() < NM_Client)
		return;

	const FVector hitDirection = hitPoint - destroyer->GetActorLocation();

	boxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	boxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	boxComponent->SetGenerateOverlapEvents(true);

	FVector newBoxExtent = boxComponent->GetUnscaledBoxExtent();
	newBoxExtent.Y *= 0.5f;
	boxComponent->SetBoxExtent(newBoxExtent);

	destructibleComponent->ApplyDamage(100, hitPoint, GetActorLocation(), force);

	isDestroyed = true;	
}

bool ABorderBlock::RPC_SendDestroyBlockToClients_Validate(AActor* destroyer, float force, FVector hitPoint)
{
	return true;
}




