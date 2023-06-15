// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMode.h"
#include <UMG/Public/Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include "Enemy.h"

AInGameMode::AInGameMode() {
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
		winWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), winWidgetClass);
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

}
