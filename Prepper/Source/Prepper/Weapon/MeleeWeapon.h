#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

class UNiagaraSystem;

UCLASS()
class PREPPER_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

	void FindActorsWithinRadius();
protected:
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere)
	float AttackRange = 50.f;

	UPROPERTY(EditAnywhere)
	float AttackReach = 100.f;

private:
	UFUNCTION()
	void DamageTarget(const FHitResult& HitTarget);
	
};
