// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnim.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include <Particles/ParticleSystem.h>
#include <Particles/ParticleSystemComponent.h>
#include "../BattleGroundCharacter.h"
#include <Sound/SoundCue.h>

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

//	DrawDebugLine(GetWorld(), me->fsm->startPos, me->fsm->NewEndPos, FColor::Blue, false, 0.7, 0, 3);

		if (bDamage) {
			ABattleGroundCharacter* player = Cast<ABattleGroundCharacter>(hitInfoShot.GetActor());
			if (player) {
				auto NewRotation = hitInfoShot.ImpactNormal.Rotation();
				NewRotation.Roll += 90;
				NewRotation.Yaw += 270;
				
				auto bloodEffect = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), me->damageEffect, hitInfoShot.ImpactPoint);
				bloodEffect->SetWorldRotation(NewRotation);
				player->currHp--;

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