// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEGROUND_API AInGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AInGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<class UUserWidget> winWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
		TSubclassOf<class UUserWidget> loseWidgetClass;

	UPROPERTY(EditAnywhere)
		class UWinPanel* winWidgetInstance;
	UPROPERTY(EditAnywhere)
	class UUserWidget* loseWidgetInstance;

	int32 enemyCount = 0;
	TArray<class AEnemy*> ActiveEnemies;

	bool bWin;
	UPROPERTY(EditAnywhere)
	float winCurrTime;
	float winFadeInTime = 0.8;
};
