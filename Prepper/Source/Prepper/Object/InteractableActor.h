// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prepper/Character/PlayerCharacter.h"
#include "Prepper/Interfaces/Interactable.h"
#include "InteractableActor.generated.h"

UCLASS()
class PREPPER_API AInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	virtual void ShowPickUpWidget(bool bShowWidget) override;
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	class USphereComponent* AreaSphere;
	UPROPERTY(VisibleAnywhere, Category = "Pickup Properties")
	class UWidgetComponent* PickUpWidget;
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	int32 CustomDepthColor;
	
};