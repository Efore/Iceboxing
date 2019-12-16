// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawnBase.h"

#include "assert.h"

#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardData.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "LevelGameState.h"


// Sets default values
APlayerPawnBase::APlayerPawnBase()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetSimulatePhysics(true);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->AttachToComponent(CapsuleComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RootComponent = CapsuleComponent;
}

// Called when the game starts or when spawned
void APlayerPawnBase::BeginPlay()
{
	Super::BeginPlay();
	isAttacking = false;
	movementGoal = currentMovement = FVector2D::ZeroVector;
	currentAttackCharge = 0.0f;
	currentAttackCooldown = attackMaxCooldown;
	currentSideDodgeCooldown = sideDodgeCooldown;

	GetWorld()->GetGameState<ALevelGameState>()->RegisterPlayingPawn(this);
}

// Called every frame
void APlayerPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ProcessPawnMovement();
	ProcessCooldowns(DeltaTime);
	RotatePawnTowardsMovement();
}

void APlayerPawnBase::ReceiveAttack(APlayerPawnBase *attacker, float pushForce)
{
	assert(attacker);
	//UE_LOG(LogTemp, Warning, TEXT("%s received attack from %s with force %f"), *this->GetName(),*attacker->GetName(), pushForce);
	CapsuleComponent->AddImpulse((attacker->GetActorForwardVector()) * pushForce);
}

void APlayerPawnBase::Move_XAxis(float AxisValue)
{
	movementGoal.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * maxGoalMovement;
}

void APlayerPawnBase::Move_YAxis(float AxisValue)
{
	// Move at 100 units per second right or left
	movementGoal.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * maxGoalMovement;
}

void APlayerPawnBase::ChargeAttack()
{
	if (currentAttackCooldown >= attackMinCooldown)
	{		
		startAttackTime = currentAttackCooldown;
		currentAttackCharge = 0.0f;		
		isAttacking = true;
		OnAttackEvent.Broadcast(isAttacking);
	}
}

void APlayerPawnBase::ReleaseAttack()
{
	if (isAttacking)
	{
		isAttacking = false;
		currentAttackCooldown = 0.0f;
		OnAttackEvent.Broadcast(isAttacking);
	}
}

void APlayerPawnBase::SideDodge()
{
	if (currentSideDodgeCooldown == sideDodgeCooldown)
	{
		CapsuleComponent->AddImpulse(CapsuleComponent->GetRightVector().GetSafeNormal() * sideDodgeForce);
		currentSideDodgeCooldown = 0.0f;
	}
}

void APlayerPawnBase::DisablePlayerPawn()
{
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	IsHiddenInGame = true;
}

void APlayerPawnBase::ProcessPawnMovement()
{
	if (currentMovement.X != movementGoal.X)
		currentMovement.X = FMath::Lerp(currentMovement.X, movementGoal.X, acceleration);
	if (currentMovement.Y != movementGoal.Y)
		currentMovement.Y = FMath::Lerp(currentMovement.Y, movementGoal.Y, acceleration);

	//UE_LOG(LogTemp, Warning, TEXT("%f %f"), m_currentMovement.X, m_currentMovement.Y);

	if (currentMovement != FVector2D::ZeroVector)
		CapsuleComponent->AddForce(FVector(currentMovement.X, currentMovement.Y, 0.0f));
}

void APlayerPawnBase::ProcessCooldowns(float DeltaTime)
{
	if (isAttacking)
	{
		currentAttackCharge += DeltaTime;

		if (currentAttackCharge >= startAttackTime)
			currentAttackCharge = startAttackTime;
	}
	else if (currentAttackCooldown < attackMaxCooldown)
	{
		currentAttackCooldown += DeltaTime;
		if (currentAttackCooldown > attackMaxCooldown)
			currentAttackCooldown = attackMaxCooldown;
	}

	attackChargePercentage = currentAttackCharge / attackMaxCooldown;
	attackCooldownPercentage = currentAttackCooldown / attackMaxCooldown;

	if (currentSideDodgeCooldown < sideDodgeCooldown)
	{
		currentSideDodgeCooldown += DeltaTime;
		if (currentSideDodgeCooldown > sideDodgeCooldown)
			currentSideDodgeCooldown = sideDodgeCooldown;
	}

	dodgeCooldownPercentage = currentSideDodgeCooldown / sideDodgeCooldown;
}

void APlayerPawnBase::RotatePawnTowardsMovement()
{
	if (movementGoal != FVector2D::ZeroVector)
	{
		CapsuleComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorForwardVector(), FVector(movementGoal, 0.0f));
		this->SetActorRotation(FMath::Lerp(this->GetActorRotation().Quaternion(), PlayerRot.Quaternion(), 0.05f));
	}
}

void APlayerPawnBase::CheckImpact()
{
	TArray<AActor*> receivers = GetAttackReceivers();

	float force = attackPushForce * (currentAttackCharge / attackMaxCooldown);
	UE_LOG(LogTemp, Warning, TEXT("force %f"), force);
	currentAttackCharge = 0.0f;

	for (int i = 0; i < receivers.Num(); ++i)
	{
		((APlayerPawnBase*)receivers[i])->ReceiveAttack(this, force);
	}	
}

TArray<AActor*> APlayerPawnBase::GetAttackReceivers()
{
	FVector position = SkeletalMeshComponent->GetSocketLocation("LeftHand");

	EObjectTypeQuery type = UEngineTypes::ConvertToObjectType(CapsuleComponent->GetCollisionObjectType());
	TEnumAsByte<EObjectTypeQuery> enumAsByte(type);

	TArray < TEnumAsByte < EObjectTypeQuery > > collisionTypes = { type };

	TArray<AActor*> actors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), position, 100.0f, collisionTypes, APlayerPawnBase::StaticClass(), { this }, actors);

	return actors;
}
