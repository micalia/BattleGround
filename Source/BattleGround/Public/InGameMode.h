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
	//AInGameMode();
	virtual void BeginPlay() override;

	class UUserWidget* winWidget;
};
