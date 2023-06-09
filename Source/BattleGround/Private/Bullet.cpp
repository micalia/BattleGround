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

	// 1. 충돌체 등록하기
	collisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));

	// 2. 충돌 프로파일 설정
	// BlockAll : 모든 물체와 부딪혀 튕귈 수 있도록 한다.
	collisionComp->SetCollisionProfileName(TEXT("BlockAll"));

	// 3. 충돌체 크기 설정
	collisionComp->SetSphereRadius(13);

	// 4. 루트로 등록
	RootComponent = collisionComp;

	//<-----------------------------------외관을 담당하는 스태틱 메시 컴포넌트를 추가해준다.---------------------------------->
	// 5. 외관 컴포넌트 등록하기
	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMeshComp"));

	// 6. 부모 컴포넌트 지정
	// 충돌 효과의 발생을 위해서이다. 
	bodyMeshComp->SetupAttachment(collisionComp);

	// 7. 충돌 비활성화 
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 8. 외관 크기 설정
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	// 프로젝타일 무브먼트 컴포넌트 인스턴스를 생성하고 갱신시킬 컴포넌트로 캡슐 컴포넌트를 지정해줍니다. 
	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));

	// **movement 컴포넌트가 갱신시킬 컴포넌트 지정
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

