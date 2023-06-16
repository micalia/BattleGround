// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include "../BattleGroundCharacter.h"
#include <Sound/SoundCue.h>
#include "InGameMode.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/PlayerController.h>

void UEnemyAnim::NativeBeginPlay()
{
	me = Cast<AEnemy>(TryGetPawnOwner());
}

void UEnemyAnim::AnimNotify_Shot(){
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Cast<USoundBase>(me->shotSoundCue), me->shootPos->GetComponentLocation(), 1.f, 1.f,  0.f, nullptr, nullptr, nullptr);
	bShotTiming = true;

	FRotator shotPosRot = FRotator(90, 0, 0);
	me->particleComp->SetActive(true);

	//if (anim->bShotTiming == true && bAttack == true) {
	FHitResult hitInfoShot;
	FCollisionQueryParams paramShot;
	paramShot.AddIgnoredActor(me);

	if (me->fsm == nullptr) return;
	bool bDamage = GetWorld()->LineTraceSingleByChannel(
		hitInfoShot,
		me->fsm->startPos,
		me->fsm->NewEndPos,
		ECC_Visibility,
		paramShot);

		if (bDamage) {
			ABattleGroundCharacter* player = Cast<ABattleGroundCharacter>(hitInfoShot.GetActor());
			if (player) {
				auto NewRotation = hitInfoShot.ImpactNormal.Rotation();
				NewRotation.Roll += 90;
				NewRotation.Yaw += 270;
				
				auto bloodEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), me->damageEffect, hitInfoShot.ImpactPoint);
				bloodEffect->SetWorldRotation(NewRotation);
				player->currHp--;
				if (player->currHp <= 0) {
					AInGameMode* gameMode = Cast<AInGameMode>(GetWorld()->GetAuthGameMode());
					if (gameMode != NULL) { 
						APlayerController* controller = GetWorld()->GetFirstPlayerController();
						player->DisableInput(controller);
						gameMode->originLoseCameraTransform = player->GetFollowCamera()->GetComponentTransform();
						
						//FVector 자료형 예제
						//FVector testVector = FVector(3, 3, 3) - FVector(1, 1, 1);
						/*=
						FTransform testTransform = FTransform(FQuat(1), FVector(1), FVector(1));
						FTransform test2Transform = player->GetFollowCamera()->GetComponentTransform();
						FTransform testTT = test2Transform - testTransform;*/

						//FTransform 자료형 빼기 식 성공
						gameMode->moveLoseCameraTransform = FTransform(
							gameMode->SetRotation,
							gameMode->originLoseCameraTransform.GetLocation() + gameMode->SetLocation,
							FVector(1)
						);
						UE_LOG(LogTemp, Warning, TEXT("originTransform : %s / moveTransform : %s"), *gameMode->originLoseCameraTransform.ToString(),*gameMode->moveLoseCameraTransform.ToString())
						gameMode->bLose = true;
					}
				}

				me->fsm->bAttack = false;

			}
			else {
				AEnemy* enemy = Cast<AEnemy>(hitInfoShot.GetActor());
				if (enemy) {
					int32 enemyHP = enemy->Damaged(me->power);
					auto NewRotation = hitInfoShot.ImpactNormal.Rotation();
					NewRotation.Roll += 90;
					NewRotation.Yaw += 270;

					auto bloodEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), me->damageEffect, hitInfoShot.ImpactPoint);
					bloodEffect->SetWorldRotation(NewRotation);

					if (enemyHP <= 0) {
						//UE_LOG(LogTemp, Warning, TEXT("hp: %d"), enemyHP)
						me->fsm->target = me->fsm->playerPointer;
					}

					me->fsm->bAttack = false;

				}

			}

		}

}

//bool UEnemyFSM::LineTraceAttack()
//{
//	
//}