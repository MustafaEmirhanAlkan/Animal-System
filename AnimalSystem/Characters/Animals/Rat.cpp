#include "AnimalSystem/Characters/Animals/Rat.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSystem/Animation/AnimalAnimInstance.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

ARat::ARat()
{
	RatHealth = 10.0f;
	RunSpeed = 500.0f;
	AlertDistance = 800.0f;
	EscapeDistance = 600.0f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Rodents/Rat/Meshes/Rat_Lod_0"));
	if (MeshAsset.Succeeded())
	{
		AnimalMesh->SetSkeletalMesh(MeshAsset.Object);
	}
	AnimalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
	AnimalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FClassFinder<UAnimInstance> RatAnimBP(TEXT("/Game/Animations/Rat/Rat_Anim_BP"));
	if (RatAnimBP.Succeeded())
	{
		AnimalMesh->SetAnimInstanceClass(RatAnimBP.Class);
	}

	UAIPerceptionStimuliSourceComponent* PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	if (PerceptionStimuliSource)
	{
		PerceptionStimuliSource->bAutoRegister = true;
		PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	}
}

void ARat::BeginPlay()
{
	Super::BeginPlay();

	UAnimalAnimInstance* RatAnimInstance = Cast<UAnimalAnimInstance>(AnimalMesh->GetAnimInstance());
	if (RatAnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("RatAnimInstance successfully assigned."));
		RatAnimInstance->UpdateAnimationProperties(0.0f);
	}

	if (UAnimalAnimInstance* AnimInstance = Cast<UAnimalAnimInstance>(AnimalMesh->GetAnimInstance()))
	{
		UE_LOG(LogTemp, Log, TEXT("UAnimalAnimInstance loaded successfully for Rat."));
		AnimInstance->PlayIdleAnimation();
	}
}

void ARat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RatHealth > 0.0f)
	{
		FVector Velocity = GetVelocity();
		if (Velocity.Size() > 0.0f)
		{
			SetActorRotation(Velocity.Rotation());

			if (UAnimalAnimInstance* AnimInstance = Cast<UAnimalAnimInstance>(AnimalMesh->GetAnimInstance()))
			{
				AnimInstance->Speed_Rat = Velocity.Size();
				AnimInstance->UpdateAnimationProperties(DeltaTime);
			}
		}
	}
}

float ARat::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}