#include "Enemy.h"
#include <Engine/SkeletalMesh.h>
#include <Components/SceneComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SphereComponent.h>
#include <Kismet/KismetSystemLibrary.h>

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

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Enemy"));

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

	/*AActor* test = GetOwner();
	EmptyActorsToIgnore.Add(GetOwner());
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
		UE_LOG(LogTemp, Warning, TEXT("bResult: %s"), bResult ? TEXT("True"):TEXT("False"))
		EmptyActorsToIgnore.Empty();
		OutHits.Empty();*/
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

