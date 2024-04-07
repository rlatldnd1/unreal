// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AInteractable.h"
#include "GameFramework/Actor.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct PREPPER_API FItemData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelUp")
	FString ItemCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelUp")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelUp")
	TObjectPtr<AInteractable> Target;
};
