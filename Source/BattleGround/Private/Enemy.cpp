#include "Enemy.h"
#include <Engine/SkeletalMesh.h>
#include <Components/SceneComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SphereComponent.h>
#include <Kismet/KismetSystemLibrary.h>
#include "EnemyFSM.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny'"));
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
	/*checkEnemyColl = CreateDefaultSubobject<USphereComponent>(TEXT("checkEnemyColl"));
	checkEnemyColl->SetupAttachment(GetCapsuleComponent());
	checkEnemyColl->SetSphereRadius(500);*/
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
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
		EDrawDebugTrace::ForDuration,
		OutHits,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.1
	);

	//로그 출력
	for (int32 i=0; i<OutHits.Num();i++)
	{
		/*GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Purple, FString::Printf(TEXT("msg: %s"), *OutHits[i].GetActor()->GetName()), true, FVector2D(1, 1));*/
		if (OutHits[i].GetActor()->GetName().Contains(TEXT("Person"))) {
			fsm->target = Cast<ACharacter>(OutHits[i].GetActor());
			break;
		}
		else {
			fsm->target = Cast<ACharacter>(OutHits[i].GetActor());
		}
	}
}

