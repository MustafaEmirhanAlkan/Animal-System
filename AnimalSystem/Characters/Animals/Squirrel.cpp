#include "AnimalSystem/Characters/Animals/Squirrel.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSystem/Animation/AnimalAnimInstance.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

ASquirrel::ASquirrel()
{
	SquirrelHealth = 15.0f;
	RunSpeed = 500.0f;
	AlertDistance = 800.0f;
	EscapeDistance = 600.0f;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Rodents/Squirrel/Meshes/Squirrel_SM"));
	if (MeshAsset.Succeeded())
	{
		AnimalMesh->SetSkeletalMesh(MeshAsset.Object);
	}
	AnimalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
	AnimalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FClassFinder<UAnimInstance> SquirrelAnimBP(TEXT("/Game/Animations/Squirrel/Squirrel_Anim_BP"));
	if (SquirrelAnimBP.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(SquirrelAnimBP.Class);
	}

	UAIPerceptionStimuliSourceComponent* PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	if (PerceptionStimuliSource)
	{
		PerceptionStimuliSource->bAutoRegister = true;
		PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->SetMovementMode(EMovementMode::MOVE_Walking);
		Movement->MaxWalkSpeed = RunSpeed;
	}
}

void ASquirrel::BeginPlay()
{
	Super::BeginPlay();

	UAnimalAnimInstance* SquirrelAnimInstance = Cast<UAnimalAnimInstance>(AnimalMesh->GetAnimInstance());
	if (SquirrelAnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("SquirrelAnimInstance successfully assigned."));
		SquirrelAnimInstance->UpdateAnimationProperties(0.0f);
	}
}

void ASquirrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SquirrelHealth > 0.0f)
	{
		FVector Velocity = GetVelocity();
		if (Velocity.Size() > 0.0f)
		{
			SetActorRotation(Velocity.Rotation());

			if (UAnimalAnimInstance* AnimInstance = Cast<UAnimalAnimInstance>(AnimalMesh->GetAnimInstance()))
			{
				AnimInstance->Speed_Squirrel = Velocity.Size();
				AnimInstance->UpdateAnimationProperties(DeltaTime);
			}
		}
	}
}

float ASquirrel::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}