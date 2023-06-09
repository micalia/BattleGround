// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include <Components/SphereComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include "../Public/Enemy.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ABullet::ABullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. �浹ü ����ϱ�
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));

	// 2. �浹 �������� ����
	// BlockAll : ��� ��ü�� �ε��� ƨ�� �� �ֵ��� �Ѵ�.
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));

	// 3. �浹ü ũ�� ����
	collisionComp->SetSphereRadius(13);

	// 4. ��Ʈ�� ���
	RootComponent = collisionComp;

	//<-----------------------------------�ܰ��� ����ϴ� ����ƽ �޽� ������Ʈ�� �߰����ش�.---------------------------------->
	// 5. �ܰ� ������Ʈ ����ϱ�
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));

	// 6. �θ� ������Ʈ ����
	// �浹 ȿ���� �߻��� ���ؼ��̴�. 
	bodyMeshComp->SetupAttachment(collisionComp);

	// 7. �浹 ��Ȱ��ȭ 
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 8. �ܰ� ũ�� ����
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	// ������Ÿ�� �����Ʈ ������Ʈ �ν��Ͻ��� �����ϰ� ���Ž�ų ������Ʈ�� ĸ�� ������Ʈ�� �������ݴϴ�. 
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));

	// **movement ������Ʈ�� ���Ž�ų ������Ʈ ����
	movementComp->SetUpdatedComponent(collisionComp);

	movementComp->InitialSpeed = 5000;
	movementComp->MaxSpeed = 5000;
	movementComp->bShouldBounce = true;
	movementComp->Bounciness = 0.3f;

	ConstructorHelpers::FObjectFinder<UParticleSystem> tempDamageEffect(TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));
	if (tempDamageEffect.Succeeded()) {
		damageEffect = tempDamageEffect.Object;
	}
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	collisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlap);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* enemy = Cast<AEnemy>(OtherActor);
	enemy->Damaged(power);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), damageEffect, OverlappedComponent->GetComponentLocation(), GetActorRotation());
	Destroy();
	//SweepResult.ImpactPoint
}

