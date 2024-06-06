// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "EnemyBaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking")
};

UCLASS()
class PREPPER_API AEnemyBaseCharacter : public ABaseCharacter
{
	GENERATED_BODY()
public:
	AEnemyBaseCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();


protected:

	UPROPERTY(VisibleAnywhere)
	class UGaugeBarComponent* HealthBarWidget;
	
	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	float CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float AttackRadius = 150.f;

	virtual void ReceiveDamage(AActor* DamagedActor, float Damage,
	AController* InstigatorController, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass) override;

	virtual void UpdateHUDHealth() override;

	/** 
	* Navigation
	*/

	UPROPERTY()
	class AAIController* EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	bool InTargetRange(AActor* Target, float Radius);
	void MoveToTarget(AActor* Target);
	void MoveToLocation(FVector &Location);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	UFUNCTION()
	void PawnHearn(APawn *HearnPawn, const FVector &Location, float Volume);

	UPROPERTY()
	AActor* CombatTarget;
	
};
