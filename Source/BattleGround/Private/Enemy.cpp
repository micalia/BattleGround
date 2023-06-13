#include "Enemy.h"
#include <Engine/SkeletalMesh.h>
#include <Components/SceneComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SphereComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include "Components/WidgetComponent.h"
#include "EnemyFSM.h"
#include <Camera/CameraComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/PlayerController.h>
#include "UI_EnemyHP.h"
#include <Particles/ParticleSystem.h>
#include "EnemyAnim.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Modelings/Soldier/SM_Soldier.SM_Soldier'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0,0,-85));
		GetMesh()->SetRelativeRotation(FRotator(0,-90,0));
	}

	shootPos = CreateDefaultSubobject<USceneComponent>(TEXT("shootPos"));
	shootPos->SetupAttachment(GetCapsuleComponent());
	shootPos->SetRelativeLocation(FVector(18, 0, 53));

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));

	fsm = CreateDefaultSubobject<UEnemyFSM>(TEXT("FSM"));

	enemyHPwidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHpWidget"));
	enemyHPwidget->SetupAttachment(GetCapsuleComponent());

	static ConstructorHelpers::FObjectFinder<UParticleSystem> tempDamageEffect(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tempDamageEffect.Succeeded()) {
		damageEffect = tempDamageEffect.Object;
	}

	ConstructorHelpers::FClassFinder<UEnemyAnim> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprints/ABP_Enemy.ABP_Enemy_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}

	gunMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("gunMesh"));
	gunMeshComp->SetupAttachment(GetMesh(), TEXT("GunPos"));
	gunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempGunMesh(TEXT("/Script/Engine.StaticMesh'/Game/Modelings/M416/M4A1.M4A1'"));
	if (tempGunMesh.Succeeded()) {
		gunMeshComp->SetStaticMesh(tempGunMesh.Object);
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	currHP = FullHp;

	enemyHpUI = Cast<UUI_EnemyHP>(enemyHPwidget->GetWidget());
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateWidgetRotation, this, &AEnemy::UpdateWidgetRotation, 0.1f, true);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (enemyHpUI !=nullptr) {
		enemyHpUI->UpdateCurrHP(currHP, FullHp);
	}

	CheckCreatureCollision();
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::CheckCreatureCollision()
{
	EmptyActorsToIgnore.Add(this);
	bool bResult = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation(),
		checkEnemyCollRadius,
		UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel4),
		true,
		EmptyActorsToIgnore,
		EDrawDebugTrace::None,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.1
	);

	for (int32 i=0; i<OutHits.Num();i++)
	{
		if (fsm->target == nullptr)return;
			if (OutHits[i].GetActor()->GetName().Contains(TEXT("Person"))) {
				fsm->target = Cast<ACharacter>(OutHits[i].GetActor());
				break;
			}
			else {
				fsm->target = Cast<ACharacter>(OutHits[i].GetActor());
			}
	}
}

int32 AEnemy::Damaged(int32 damage)
{
	currHP -= damage;
	if (currHP <= 0) {
		fsm->ChangeState(EEnemyState::Die);
	}
	return currHP;
}

void AEnemy::UpdateWidgetRotation()
{
	CameraLocation = GetPlayerCameraLocation();
	CameraRotation = GetPlayerCameraRotation();
	if (enemyHPwidget)
	{
		WidgetRotation = UKismetMathLibrary::FindLookAtRotation(enemyHPwidget->GetComponentLocation(), CameraLocation);
		enemyHPwidget->SetWorldRotation(WidgetRotation);
	}
}

FVector AEnemy::GetPlayerCameraLocation()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController != nullptr)
	{
		UCameraComponent* MainCamera = PlayerController->GetPawn()->FindComponentByClass<UCameraComponent>();

		if (MainCamera != nullptr)
		{
			return MainCamera->GetComponentLocation();
		}
	}

	return FVector::ZeroVector;
}

FRotator AEnemy::GetPlayerCameraRotation()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (PlayerController != nullptr)
	{
		UCameraComponent* MainCamera = PlayerController->GetPawn()->FindComponentByClass<UCameraComponent>();

		if (MainCamera != nullptr)
		{
			return MainCamera->GetComponentRotation();
		}
	}

	return FRotator::ZeroRotator;
}