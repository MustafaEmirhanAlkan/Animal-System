#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSystem/Characters/Animals/Rabbit.h"
#include "AnimalSystem/Characters/Animals/Rat.h"
#include "AnimalSystem/Characters/Animals/Squirrel.h"
#include "AnimalSystem/Manager/AnimalManager.h"
#include "AnimalSystem/Pool/AnimalPool.h"
#include "AnimalSystem/Characters/PlayerCharacter/AnimalSystemCharacter.h"
#include "AnimalSystem/Animation/AnimalAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ABaseAnimal::ABaseAnimal()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	RootComponent = GetCapsuleComponent();

	AnimalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AnimalMesh"));
	AnimalMesh->SetupAttachment(RootComponent);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	AnimalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
	CollisionComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	UAnimalAnimInstance* AnimInstance = Cast<UAnimalAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->PlayIdleLoop = (GetVelocity().Size() <= 5.0f);
		if (GetVelocity().Size() > 5.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Animal is moving, velocity: %f"), GetVelocity().Size());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Animal is idle, velocity: %f"), GetVelocity().Size());
		}
	}
}

void ABaseAnimal::BeginPlay()
{
	Super::BeginPlay();

    if (GetMesh())
    {
        AnimalAnimInstance = Cast<UAnimalAnimInstance>(GetMesh()->GetAnimInstance());
        if (!AnimalAnimInstance)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to cast to UAnimalAnimInstance."));
        }
    }
}

void ABaseAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AnimalAnimInstance)
	{
		AnimalAnimInstance->UpdateAnimationProperties(DeltaTime);
	}

	FVector Velocity = GetVelocity();
	if (Velocity.Size() > 0)
	{
		FRotator NewRotation = Velocity.Rotation();
		SetActorRotation(NewRotation);
	}
}

float ABaseAnimal::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority())
	{
		return DamageAmount;
	}

	ARabbit* HitRabbit = Cast<ARabbit>(this);
	if (HitRabbit)
	{
		HitRabbit->RabbitHealth -= DamageAmount;
		if (HitRabbit->RabbitHealth <= 0.0f)
		{
			HandleDeath(TEXT("Rabbit"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Rabbit Health: %.2f"), HitRabbit->RabbitHealth);
		}
		return DamageAmount;
	}

	ARat* HitRat = Cast<ARat>(this);
	if (HitRat)
	{
		HitRat->RatHealth -= DamageAmount;
		if (HitRat->RatHealth <= 0.0f)
		{
			HandleDeath(TEXT("Rat"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Rat Health: %.2f"), HitRat->RatHealth);
		}
		return DamageAmount;
	}

	ASquirrel* HitSquirrel = Cast<ASquirrel>(this);
	if (HitSquirrel)
	{
		HitSquirrel->SquirrelHealth -= DamageAmount;
		if (HitSquirrel->SquirrelHealth <= 0.0f)
		{
			HandleDeath(TEXT("Squirrel"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Squirrel Health: %.2f"), HitSquirrel->SquirrelHealth);
		}
		return DamageAmount;
	}

	return DamageAmount;
}

void ABaseAnimal::HandleDeath(FString AnimalType)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s Dead!"), *AnimalType));
	}

	GetCharacterMovement()->DisableMovement();

	if (AnimalAnimInstance)
	{
		AnimalAnimInstance->bIsDead = true;
		AnimalAnimInstance->PlayDeathAnimation();
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to play death animation for %s."), *AnimalType);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AnimalAnimInstance is null for %s."), *AnimalType);
	}

	AAnimalManager* animalManager = Cast<AAnimalManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AAnimalManager::StaticClass()));
	if (animalManager)
	{
		GetWorld()->GetTimerManager().SetTimer(destroyTimerHandle, [this, animalManager]()
			{
				animalManager->CheckDistanceAndVisibility(this);
			}, 3.0f, false);
	}
}