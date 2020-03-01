// Fill out your copyright notice in the Description page of Project Settings.

#include "ArenaPlayerPawn.h"

#include "assert.h"

#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardData.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "ArenaGameState.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AArenaPlayerPawn::AArenaPlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetSimulatePhysics(true);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->AttachToComponent(CapsuleComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RootComponent = CapsuleComponent;

	SetReplicates(true);
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

// Called when the game starts or when spawned
void AArenaPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	isAttacking = false;
	movementGoal = currentMovement = FVector2D::ZeroVector;
	currentAttackCharge = 0.0f;
	currentAttackCooldown = attackMaxCooldown;
	currentSideDodgeCooldown = sideDodgeCooldown;

	if (GetNetMode() < NM_Client)
		GetWorld()->GetGameState<AArenaGameState>()->RegisterPlayingPawn(this);
}

// Called every frame
void AArenaPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ProcessPawnMovement();
	RotatePawnTowardsMovement();
	
	if (GetNetMode() < NM_Client)
	{
		ProcessCooldowns(DeltaTime);
	}
}

void AArenaPlayerPawn::ReceiveAttack(AArenaPlayerPawn *attacker, float pushForce)
{
	assert(attacker);
	//UE_LOG(LogTemp, Warning, TEXT("%s received attack from %s with force %f"), *this->GetName(),*attacker->GetName(), pushForce);
	CapsuleComponent->AddImpulse((attacker->GetActorForwardVector()) * pushForce);

	RPC_SendReceiveAttackToClients(attacker, pushForce);
}

void AArenaPlayerPawn::Move_XAxis(float AxisValue)
{
	if (GetNetMode() == NM_Client)
	{
		RPC_SendXAxisValueToServer(AxisValue);
	}
	else
	{
		movementGoal.X = FMath::Clamp(AxisValue, -1.0f, 1.0f) * maxGoalMovement;
	}
}

void AArenaPlayerPawn::Move_YAxis(float AxisValue)
{
	if (GetNetMode() == NM_Client)
	{
		RPC_SendYAxisValueToServer(AxisValue);
	}
	else
	{
		// Move at 100 units per second right or left
		movementGoal.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f) * maxGoalMovement;
	}
}

void AArenaPlayerPawn::ChargeAttack()
{
	if (GetNetMode() == NM_Client)
	{
		RPC_SendChargeAttackToServer();
	}
	else if (currentAttackCooldown >= attackMinCooldown)
	{
		startAttackTime = currentAttackCooldown;
		currentAttackCharge = 0.0f;
		isAttacking = true;
		OnAttackEvent.Broadcast(isAttacking);
	}	
}

void AArenaPlayerPawn::ReleaseAttack()
{
	if (GetNetMode() == NM_Client)
	{
		RPC_SendReleaseAttackToServer();
	}
	else if (isAttacking)
	{
		isAttacking = false;
		currentAttackCooldown = 0.0f;
		OnAttackEvent.Broadcast(isAttacking);
	}
}

void AArenaPlayerPawn::SideDodge()
{
	if (GetNetMode() == NM_Client)
	{
		RPC_SendSideDodgeToServer();
	}
	else if (currentSideDodgeCooldown == sideDodgeCooldown)
	{
		CapsuleComponent->AddImpulse(CapsuleComponent->GetRightVector().GetSafeNormal() * sideDodgeForce);
		currentSideDodgeCooldown = 0.0f;
	}
}

void AArenaPlayerPawn::DisablePlayerPawn()
{
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	IsHiddenInGame = true;
}

void AArenaPlayerPawn::ProcessPawnMovement()
{
	if (currentMovement.X != movementGoal.X)
		currentMovement.X = FMath::Lerp(currentMovement.X, movementGoal.X, acceleration);
	if (currentMovement.Y != movementGoal.Y)
		currentMovement.Y = FMath::Lerp(currentMovement.Y, movementGoal.Y, acceleration);

	if (currentMovement != FVector2D::ZeroVector)
		CapsuleComponent->AddForce(FVector(currentMovement.X, currentMovement.Y, 0.0f));
}

void AArenaPlayerPawn::ProcessCooldowns(float DeltaTime)
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

void AArenaPlayerPawn::RotatePawnTowardsMovement()
{
	if (movementGoal != FVector2D::ZeroVector)
	{
		CapsuleComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorForwardVector(), FVector(movementGoal, 0.0f));
		this->SetActorRotation(FMath::Lerp(this->GetActorRotation().Quaternion(), PlayerRot.Quaternion(), 0.05f));
	}
}

void AArenaPlayerPawn::CheckImpact()
{
	if (GetNetMode() < NM_Client)
	{	
		TArray<AActor*> receivers = GetAttackReceivers();

		float force = attackPushForce * (currentAttackCharge / attackMaxCooldown);
		UE_LOG(LogTemp, Warning, TEXT("force %f"), force);
		currentAttackCharge = 0.0f;

		for (int i = 0; i < receivers.Num(); ++i)
		{
			((AArenaPlayerPawn*)receivers[i])->ReceiveAttack(this, force);
		}
	}
	else
	{
		RPC_SendCheckImpactToServer();
	}
}

TArray<AActor*> AArenaPlayerPawn::GetAttackReceivers()
{
	FVector position = SkeletalMeshComponent->GetSocketLocation("LeftHand");

	EObjectTypeQuery type = UEngineTypes::ConvertToObjectType(CapsuleComponent->GetCollisionObjectType());
	TEnumAsByte<EObjectTypeQuery> enumAsByte(type);

	TArray < TEnumAsByte < EObjectTypeQuery > > collisionTypes = { type };

	TArray<AActor*> actors;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), position, 100.0f, collisionTypes, AArenaPlayerPawn::StaticClass(), { this }, actors);

	return actors;
}

void AArenaPlayerPawn::SetPlayerControllerIndex(int index)
{
	switch (index)
	{
	case 0:
		AutoPossessPlayer = EAutoReceiveInput::Player0;
		break;
	case 1:
		AutoPossessPlayer = EAutoReceiveInput::Player1;
		break;
	case 2:
		AutoPossessPlayer = EAutoReceiveInput::Player2;
		break;
	case 3:
		AutoPossessPlayer = EAutoReceiveInput::Player3;
		break;
	default:
		AutoPossessPlayer = EAutoReceiveInput::Player0;
		break;
	}
}

void AArenaPlayerPawn::RPC_SendXAxisValueToServer_Implementation(float axisValue)
{
	Move_XAxis(axisValue);
}

void AArenaPlayerPawn::RPC_SendYAxisValueToServer_Implementation(float axisValue)
{
	Move_YAxis(axisValue);
}

void AArenaPlayerPawn::RPC_SendChargeAttackToServer_Implementation()
{
	startAttackTime = currentAttackCooldown;
	currentAttackCharge = 0.0f;
	isAttacking = true;
	OnAttackEvent.Broadcast(isAttacking);
}

bool AArenaPlayerPawn::RPC_SendChargeAttackToServer_Validate()
{
	return currentAttackCooldown >= attackMinCooldown;
}

void AArenaPlayerPawn::RPC_SendReleaseAttackToServer_Implementation()
{
	isAttacking = false;
	currentAttackCooldown = 0.0f;
	OnAttackEvent.Broadcast(isAttacking);
}

bool AArenaPlayerPawn::RPC_SendReleaseAttackToServer_Validate()
{
	return isAttacking;
}

void AArenaPlayerPawn::RPC_SendSideDodgeToServer_Implementation()
{
	CapsuleComponent->AddImpulse(CapsuleComponent->GetRightVector().GetSafeNormal() * sideDodgeForce);
	currentSideDodgeCooldown = 0.0f;
}

bool AArenaPlayerPawn::RPC_SendSideDodgeToServer_Validate()
{
	return currentSideDodgeCooldown == sideDodgeCooldown;
}

void AArenaPlayerPawn::RPC_SendReceiveAttackToClients_Implementation(AArenaPlayerPawn* attacker, float pushForce)
{
	if (GetNetMode() == NM_Client)
	{
		CapsuleComponent->AddImpulse((attacker->GetActorForwardVector()) * pushForce);
	}
}

bool AArenaPlayerPawn::RPC_SendReceiveAttackToClients_Validate(AArenaPlayerPawn* attacker, float pushForce)
{
	return attacker != nullptr;
}

void AArenaPlayerPawn::RPC_SendCheckImpactToServer_Implementation()
{
	CheckImpact();
}

bool AArenaPlayerPawn::RPC_SendCheckImpactToServer_Validate()
{
	return true;
}

void AArenaPlayerPawn::OnRep_isAttacking()
{
	if (GetNetMode() == NM_Client)
	{
		OnAttackEvent.Broadcast(isAttacking);
	}
}

void AArenaPlayerPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AArenaPlayerPawn, attackCooldownPercentage);
	DOREPLIFETIME(AArenaPlayerPawn, attackChargePercentage);
	DOREPLIFETIME(AArenaPlayerPawn, dodgeCooldownPercentage);	
	DOREPLIFETIME(AArenaPlayerPawn, movementGoal);
	DOREPLIFETIME(AArenaPlayerPawn, isAttacking);
}

