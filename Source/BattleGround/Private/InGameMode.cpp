// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMode.h"
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"
#include "WinPanel.h"
#include <UMG/Public/Components/CanvasPanel.h>

AInGameMode::AInGameMode() {
	PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FClassFinder<UUserWidget> tempWinWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UMG/WB_WinWidget.WB_WinWidget_C'"));
    if (tempWinWidgetClass.Succeeded())
    {
        winWidgetClass = tempWinWidgetClass.Class;
    }

	static ConstructorHelpers::FClassFinder<UUserWidget> tempLoseWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/UMG/WB_LoseWidget.WB_LoseWidget_C'"));
	if (tempLoseWidgetClass.Succeeded())
	{
		loseWidgetClass = tempLoseWidgetClass.Class;
	}
}

void AInGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (winWidgetClass) {
		winWidgetInstance = CreateWidget<UWinPanel>(GetWorld(), winWidgetClass);
		if (winWidgetInstance) {
			winWidgetInstance->AddToViewport();
		}
	}

	if (loseWidgetClass) {
		loseWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), loseWidgetClass);
		if (loseWidgetInstance) {
			loseWidgetInstance->AddToViewport();
		}
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AEnemy* enemy = Cast<AEnemy>(Actor);
		if (enemy) {
			ActiveEnemies.Add(enemy);
		}
	}

	enemyCount = ActiveEnemies.Num();
	UE_LOG(LogTemp, Warning, TEXT("enemyCount: %d"), enemyCount)
}

void AInGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWin) {
		winCurrTime += DeltaTime;

		float opacity = winCurrTime / winFadeInTime;
		if (opacity <= 1) {
			winWidgetInstance->winPanel->SetRenderOpacity(opacity);
		}
		/*winWidgetInstance->
		FMath::Lerp(prevCamPos, powerAttackCamMove->GetRelativeLocation(), GetWorld()->GetDeltaSeconds() * SkillCamSpeed);*/
	}
}
