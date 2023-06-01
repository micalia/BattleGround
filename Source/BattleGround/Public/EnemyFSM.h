// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Rotate,
	Attack,
	Die,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEGROUND_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY()
	class AEnemy* me;

	UPROPERTY()
	class AAIController* ai;

	//타겟
	UPROPERTY(EditAnywhere)
		class ABattleGroundCharacter* target;

	UPROPERTY(VisibleAnywhere)
	float currTime = 0;

	UPROPERTY(EditAnywhere)
	float idleDelayTime = 1.3;
	UPROPERTY(EditAnywhere)
	float moveDelayTime = 2;

//회전
UPROPERTY(EditAnywhere)
float rotMinTime = 0.6;
UPROPERTY(EditAnywhere)
float rotMaxTime = 1;
UPROPERTY(EditAnywhere) // 회전하는 시간
float rotatingTime = 0.7;
FRotator currRot;
FRotator destRot;

//동작
	UPROPERTY(EditAnywhere)
	EEnemyState currState = EEnemyState::Move;

	void ChangeState(EEnemyState state);

	void UpdateIdle();
	void UpdateMove();
	void UpdateRotate();
	void UpdateAttack();
	void UpdateDie();

	bool IsWaitComplete(float delayTime);

	//쫓아 갈 수 있는 범위
	UPROPERTY(EditAnywhere)
	float traceRange = 900;
	bool IsTargetTrace();

	//공격범위
	UPROPERTY(EditAnywhere)
	float attackRange = 800;
};
