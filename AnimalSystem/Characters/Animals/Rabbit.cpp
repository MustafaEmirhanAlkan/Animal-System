#include "AnimalSystem/Characters/Animals/Rabbit.h"
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

ARabbit::ARabbit()
{
	RabbitHealth = 15.0f;
	RunSpeed = 700.0f;
	AlertDistance = 1000.0f;
	EscapeDistance = 800.0f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Rodents/Rabbit/Meshes/Rabbit_SM"));
	if (MeshAsset.Succeeded())
	{
		AnimalMesh->SetSkeletalMesh(MeshAsset.Object);
	}
	AnimalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
	AnimalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FClassFinder<UAnimInstance> RabbitAnimBP(TEXT("/Game/Animations/Rabbit/Rabbit_Anim_BP"));
	if (RabbitAnimBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(RabbitAnimBP.Class);
	}

	UAIPerceptionStimuliSourceComponent* PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	if (PerceptionStimuliSource)
	{
		PerceptionStimuliSource->bAutoRegister = true;
		PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	}
}

void ARabbit::BeginPlay()
{
	Super::BeginPlay();

	UAnimalAnimInstance* RabbitAnimInstance = Cast<UAnimalAnimInstance>(AnimalMesh->GetAnimInstance());
	if (RabbitAnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("RabbitAnimInstance successfully assigned."));
		RabbitAnimInstance->UpdateAnimationProperties(0.0f);
	}

	if (UAnimalAnimInstance* AnimInstance = Cast<UAnimalAnimInstance>(AnimalMesh->GetAnimInstance()))
	{
		UE_LOG(LogTemp, Log, TEXT("UAnimalAnimInstance loaded successfully for Rabbit."));
		AnimInstance->PlayIdleAnimation();
	}
}

void ARabbit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RabbitHealth > 0.0f)
	{
		FVector Velocity = GetVelocity();
		if (Velocity.Size() > 0.0f)
		{
			SetActorRotation(Velocity.Rotation());

			if (UAnimalAnimInstance* AnimInstance = Cast<UAnimalAnimInstance>(AnimalMesh->GetAnimInstance()))
			{
				AnimInstance->Speed_Rabbit = Velocity.Size();
				AnimInstance->UpdateAnimationProperties(DeltaTime);
			}
		}
	}
}

float ARabbit::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}