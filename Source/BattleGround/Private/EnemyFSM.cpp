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
			//���¸� Attack ���� ����
			ChangeState(EEnemyState::Attack);
		}
	}
	else
	{
		//ai �� �̿��ؼ� ���������� �̵��ϰ� �ʹ�.	
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
			//���¸� Attack ���� ����
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

	//2. ���� �չ���� 1���� ���� ������ ����
	float dotValue = FVector::DotProduct(me->GetActorForwardVector(), dir.GetSafeNormal());

	//3. 2���� ���� ���� Acos --> �� ������ ���̰�
	float angle = UKismetMathLibrary::DegAcos(dotValue);

	//4. ���࿡ 3������ ���� ���� 30���� �۰�(�þ߰� 60)
	//   �� - Ÿ�� ���� �Ÿ��� traceRange ���� ������
	if (angle < 30 && dir.Length() < traceRange)
	{
		//Enemy -----> target LineTrace ����!!
		FHitResult hitInfo;
		FCollisionQueryParams param;
		param.AddIgnoredActor(me);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			hitInfo,
			me->GetActorLocation(),
			target->GetActorLocation(),
			ECC_Visibility,
			param);

		//���࿡ �ε��� ���� �ִٸ�
		if (bHit)
		{
			//���࿡ �ε��� ���� �̸��� Player �� �����ϰ� �ִٸ�
			if (hitInfo.GetActor()->GetName().Contains(TEXT("Person")))
			{
				//5. true ��ȯ
				return true;
			}
		}
	}

	//6. �׷��� ������ false ��ȯ	
	return false;
}
