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
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector dir = target->GetActorLocation() - me->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("dir: %f"), dir.Length())

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
	UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EEnemyState"), true);
	if (enumPtr != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s -----> %s"),
			*enumPtr->GetNameStringByIndex((int32)currState),
			*enumPtr->GetNameStringByIndex((int32)state));
	}

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
	UE_LOG(LogTemp, Warning, TEXT("Idle!"))
}

void UEnemyFSM::UpdateMove()
{
	if (IsWaitComplete(moveDelayTime)) {
		ChangeState(EEnemyState::Rotate);
	}

	bool bTrace = IsTargetTrace();
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	if (bTrace) {
		if (dir.Length() < attackRange)
		{
			//상태를 Attack 으로 변경
			ChangeState(EEnemyState::Attack);
		}
	}
	else
	{
		//ai 를 이용해서 목적지까지 이동하고 싶다.	
		EPathFollowingRequestResult::Type re = ai->MoveToActor(target);
	}

	UE_LOG(LogTemp, Warning, TEXT("Move!"))
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
	UE_LOG(LogTemp, Warning, TEXT("Rotate!"))
}

void UEnemyFSM::UpdateAttack()
{
	FVector dir = target->GetActorLocation() - me->GetActorLocation();
	float dist = dir.Length();

	bool bTrace = IsTargetTrace();

	me->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(me->GetActorLocation(), target->GetActorLocation()));
	if (bTrace) {
		if (dir.Length() < attackRange)
		{
			//상태를 Attack 으로 변경
			ChangeState(EEnemyState::Attack);
		}
	}
	else
	{
		ChangeState(EEnemyState::Idle);
		
	}
	UE_LOG(LogTemp, Warning, TEXT("Attack!"))
}

void UEnemyFSM::UpdateDie()
{
	UE_LOG(LogTemp, Warning, TEXT("Die!"))
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
	FVector dir = target->GetActorLocation() - me->GetActorLocation();

	//2. 나의 앞방향과 1번에 구한 벡터의 내적
	float dotValue = FVector::DotProduct(me->GetActorForwardVector(), dir.GetSafeNormal());

	//3. 2번에 구한 값을 Acos --> 두 벡터의 사이각
	float angle = UKismetMathLibrary::DegAcos(dotValue);

	//4. 만약에 3번에서 구한 값이 30보다 작고(시야각 60)
	//   나 - 타겟 과의 거리가 traceRange 보다 작으면
	if (angle < 30 && dir.Length() < traceRange)
	{
		//Enemy -----> target LineTrace 쏘자!!
		FHitResult hitInfo;
		FCollisionQueryParams param;
		param.AddIgnoredActor(me);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			hitInfo,
			me->GetActorLocation(),
			target->GetActorLocation(),
			ECC_Visibility,
			param);

		//만약에 부딪힌 곳이 있다면
		if (bHit)
		{
			//만약에 부딪힌 놈의 이름이 Player 를 포함하고 있다면
			if (hitInfo.GetActor()->GetName().Contains(TEXT("Person")))
			{
				//5. true 반환
				return true;
			}
		}
	}

	//6. 그렇지 않으면 false 반환	
	return false;
}
