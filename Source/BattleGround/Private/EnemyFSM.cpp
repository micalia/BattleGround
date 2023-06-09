// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include <Kismet/KismetMathLibrary.h>
#include "Enemy.h"
#include <AIModule/Classes/AIController.h>
#include "../BattleGroundCharacter.h"
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<AEnemy>(GetOwner());

	ai = Cast<AAIController>(me->GetController());
	target = Cast<ABattleGroundCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), ABattleGroundCharacter::StaticClass()));
	playerPointer = target;
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (target != nullptr) {
	dir = target->GetActorLocation() - me->GetActorLocation();
		
	}
	else {
		target = playerPointer;
	}

	UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState"), true);
	if (enumPtr != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Purple, FString::Printf(TEXT("%s : %s / target is (%s)"), *me->GetActorLabel(),
			*enumPtr->GetNameStringByIndex((int32)currState), *target->GetActorLabel()),
			 true, FVector2D(1, 1));
	}
	switch (currState) {
	case EEnemyState::Idle:
		UpdateIdle();
		break;
	case EEnemyState::Move:
		UpdateMove();
		break;
	case EEnemyState::Rotate:
		UpdateRotate();
		break;
	case EEnemyState::Attack:
		UpdateAttack();
		break;
	case EEnemyState::Die:
		UpdateDie();
		break;
	}
}


void UEnemyFSM::ChangeState(EEnemyState state)
{
	//UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState"), true);
	//if (enumPtr != nullptr)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("%s -----> %s"),
	//		*enumPtr->GetNameStringByIndex((int32)currState),
	//		*enumPtr->GetNameStringByIndex((int32)state));
	//}

	currState = state;

	ai->StopMovement();

	switch (currState)
	{
	case EEnemyState::Idle:
		break;
	case EEnemyState::Move:
		break;
	case EEnemyState::Rotate: {
		float ranYaw = UKismetMathLibrary::RandomFloatInRange(-180,180);
		destRot = FRotator(0, ranYaw,0);
		currRot = me->GetActorRotation();
		rotatingTime = UKismetMathLibrary::RandomFloatInRange(rotMinTime, rotMaxTime);
		}
		break;
	case EEnemyState::Attack:
		break;
	case EEnemyState::Die:
		break;
	}
}

void UEnemyFSM::UpdateIdle()
{
	if (IsWaitComplete(idleDelayTime)) {
		ChangeState(EEnemyState::Move);
	}

	if (bTrace) {
		if (dir.Length() < attackRange)
		{
			ChangeState(EEnemyState::Attack);
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Idle!"))
}

void UEnemyFSM::UpdateMove()
{

	if (IsWaitComplete(moveDelayTime)) {
		ChangeState(EEnemyState::Rotate);
	}

	bTrace = IsTargetTrace();

	if (bTrace) {
		if (dir.Length() < attackRange)
		{
			ChangeState(EEnemyState::Attack);
		}
	}
	else
	{
		EPathFollowingRequestResult::Type re = ai->MoveToActor(target);
	}

	//UE_LOG(LogTemp, Warning, TEXT("Move!"))
}

void UEnemyFSM::UpdateRotate()
{
	currTime += GetWorld()->GetDeltaSeconds();

	float alpha = currTime / rotatingTime;
	me->SetActorRotation(FMath::Lerp(currRot, destRot, alpha));
	if (alpha > 1) {
		ChangeState(EEnemyState::Idle);
		currTime = 0;
	}
	ai->StopMovement();
	bTrace = IsTargetTrace();

	if (bTrace) {
		if (dir.Length() < attackRange)
		{
			ChangeState(EEnemyState::Attack);
		}
	}
	else
	{
		EPathFollowingRequestResult::Type re = ai->MoveToActor(target);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Rotate!"))
}

void UEnemyFSM::UpdateAttack()
{
	currAtkTime += GetWorld()->GetDeltaSeconds();

	if (currAtkTime > attackCool) {
		bAttack = true;
		startPos = me->shootPos->GetComponentLocation();
		//End (카메라위치 + 카메라 앞방향 * 거리)
		int32 ranVal = UKismetMathLibrary::RandomIntegerInRange(1,3);
		switch (ranVal)
		{
		case 1:
		{
			randPos = target->GetMesh()->GetSocketLocation(TEXT("head")) + target->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(-attackErrorRange, attackErrorRange);
			
			endPos = randPos;

		}
			break;
		case 2: {
			randPos = target->GetMesh()->GetSocketLocation(TEXT("spine_05")) + target->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(-attackErrorRange, attackErrorRange);
			endPos = randPos;

		}
			break;
		case 3: {
			randPos = target->GetMesh()->GetSocketLocation(TEXT("pelvis")) + target->GetActorRightVector() * UKismetMathLibrary::RandomFloatInRange(-attackErrorRange, attackErrorRange);
			endPos = randPos;
		}
			break;
		}
		lineDir = (endPos - startPos).GetSafeNormal();
		NewEndPos = endPos + lineDir * extensionLength;

		DrawDebugLine(GetWorld(), startPos, NewEndPos, FColor::Blue, false, 0.7, 0, 3);
		currAtkTime = 0;
	}

	float dist = dir.Length();

	bTrace = IsTargetTrace();

	me->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), target->GetActorLocation()));

	if (!bTrace) {
		ChangeState(EEnemyState::Idle);
	}
		
	//UE_LOG(LogTemp, Warning, TEXT("Attack!"))
}

void UEnemyFSM::UpdateDie()
{
	//UE_LOG(LogTemp, Warning, TEXT("Die!"))
}

bool UEnemyFSM::IsWaitComplete(float delayTime)
{
	currTime += GetWorld()->DeltaTimeSeconds;

	if (currTime > delayTime)
	{
		currTime = 0;
		return true;
	}

	return false;
}

bool UEnemyFSM::IsTargetTrace()
{
	float dotValue = FVector::DotProduct(me->GetActorForwardVector(), dir.GetSafeNormal());

	float angle = UKismetMathLibrary::DegAcos(dotValue);

	if (angle < 30 && dir.Length() < traceRange)
	{
		FHitResult hitInfo;
		FCollisionQueryParams param;
		param.AddIgnoredActor(me);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			hitInfo,
			me->GetActorLocation(),
			target->GetActorLocation(),
			ECC_Visibility,
			param);

		DrawDebugLine(GetWorld(), me->GetActorLocation(), target->GetActorLocation(), FColor::Green, false, 0.7, 0, 3);

		if (bHit)
		{
			if (hitInfo.GetActor()->GetName().Contains(TEXT("Person")))
			{
				if (bAttack == true) {
					UE_LOG(LogTemp, Warning, TEXT("Player Damaged!!!"))
					bAttack = false;
				}
				return true;
			}

			if (hitInfo.GetActor()->GetName().Contains(TEXT("Enemy"))) {
				if (bAttack == true) {
					AEnemy* enemy = Cast<AEnemy>(hitInfo.GetActor());
					int32 enemyHP = enemy->Damaged(me->power);
					if (enemyHP <= 0) {
						UE_LOG(LogTemp, Warning, TEXT("hp: %d"), enemyHP)
							target = playerPointer;
					}
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), me->damageEffect, hitInfo.ImpactPoint, me->GetActorRotation());
					//
					bAttack = false;
				}
				return true;
			}
		}

	}

	return false;
}
