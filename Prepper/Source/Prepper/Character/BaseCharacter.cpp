﻿#include "BaseCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/PlayerController/PrepperPlayerController.h"
#include "Prepper/GameMode/DeathMatchGameMode.h"

ABaseCharacter::ABaseCharacter()
{
	WalkSpeed = 600.f;
	SprintSpeed = 900.f;

	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, Health);
}

void ABaseCharacter::AttachActorAtSocket(const FName& SocketName, AActor* TargetActor)
{
	const USkeletalMeshSocket* TargetSocket = GetMesh()->GetSocketByName(SocketName);
	if(TargetSocket)
	{
		TargetSocket->AttachActor(TargetActor, GetMesh());
	}
	UE_LOG(LogTemp, Warning, TEXT("Attach %s"), *SocketName.ToString());
}

void ABaseCharacter::Attach(IObserver<FGaugeFloat>* Observer)
{
	Observers.insert(Observer);
	Observer->Update(Health);
}

void ABaseCharacter::Detach(IObserver<FGaugeFloat>* Observer)
{
	Observers.erase(Observer);
}

void ABaseCharacter::Notify()
{
	std::ranges::for_each(Observers, [&](IObserver<FGaugeFloat>* Observer) {
		Observer->Update(Health);
		});
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &IDamageable::DynamicDamage);
	}
}

void ABaseCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ABaseCharacter::PlayHitReactMontage()
{
	if (!HitReactMontage) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(FName("HitFront"));
	}
}

void ABaseCharacter::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	Health.SubValue(Damage);
	PlayHitReactMontage();

	if(Health.GetCurValue() != 0.f) return;
	
	// 해당 캐릭터가 사망했다면 
	APrepperGameMode* PrepperGameMode =  GetWorld()->GetAuthGameMode<APrepperGameMode>();
	
	if(PrepperGameMode == nullptr) return;
	
	PrepperPlayerController = (PrepperPlayerController == nullptr) ? Cast<APrepperPlayerController>(Controller) : PrepperPlayerController;
	APrepperPlayerController* AttackerController = Cast<APrepperPlayerController>(InstigatorController);
	PrepperGameMode->PlayerEliminated(this, PrepperPlayerController, AttackerController);
}

void ABaseCharacter::Elim()
{
	MulticastElim();
}

void ABaseCharacter::MulticastElim_Implementation()
{
	bElimed = true;
	PlayElimMontage();

	// Start Dissolve Effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);

		TArray<USceneComponent*> AttachedComponents;
		GetMesh()->GetChildrenComponents(true,  AttachedComponents);

		// Loop through all found Static Mesh components
		for (USceneComponent* SceneComponent : AttachedComponents)
		{
			UStaticMeshComponent* SMComp = Cast<UStaticMeshComponent>(SceneComponent);
			if(SMComp)
			{
				SMComp->SetMaterial(0, DynamicDissolveMaterialInstance);
			}
		}
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("DissolveValue"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("GlowValue"), 200.f);
	}
	StartDissolve();

	// Disable Movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	// Disable Collision
	SetActorEnableCollision(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("DissolveValue"), DissolveValue);
	}
}

void ABaseCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ABaseCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

void ABaseCharacter::OnRep_Health()
{
	PlayHitReactMontage();
}