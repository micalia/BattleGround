// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>

void UEnemyAnim::NativeBeginPlay()
{
	me = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemyAnim::AnimNotify_Shot(){
	bShotTiming = true;

	FRotator shotPosRot = FRotator(90, 0, 0);
	me->particleComp->SetActive(true);
	UE_LOG(LogTemp, Warning, TEXT("shot"))
}