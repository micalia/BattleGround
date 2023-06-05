// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class BATTLEGROUND_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//모델링
	/*UPROPERTY()
	ABattleGroundCharacter* */

	UPROPERTY(EditAnywhere)
	class UEnemyFSM* fsm;

	//사격 포인트 (라인트레이스 방식)
	UPROPERTY(EditAnywhere)
	class USceneComponent* shootPos;

	TArray<AActor*> EmptyActorsToIgnore;
	TArray<FHitResult> OutHits;

	UPROPERTY(EditAnywhere)
	float checkEnemyCollRadius = 500;
	void CheckCreatureCollision();
	/*UPROPERTY(EditAnywhere)
	class USphereComponent* checkEnemyColl;*/
};
