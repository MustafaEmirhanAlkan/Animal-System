#include "AnimalSystem/Pool/AnimalPool.h"
#include "AnimalSystem/Characters/PlayerCharacter/AnimalSystemCharacter.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSystem/Animation/AnimalAnimInstance.h"
#include "AnimalSystem/Characters/Animals/Rabbit.h"
#include "AnimalSystem/Characters/Animals/Rat.h"
#include "AnimalSystem/Characters/Animals/Squirrel.h"
#include "AnimalSystem/AI/Controller/AnimalAIController.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AAnimalPool::AAnimalPool()
{
	PrimaryActorTick.bCanEverTick = false;
	PoolLimit = 3;
}

void AAnimalPool::BeginPlay()
{
	Super::BeginPlay();

}

void AAnimalPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAnimalPool::InitializePool(int32 poolSize_, TSubclassOf<ABaseAnimal> animalClassRef_)
{
	UE_LOG(LogTemp, Log, TEXT("Initializing pool with size: %d"), poolSize_);
	UE_LOG(LogTemp, Log, TEXT("animalClassRef_ is: %s"), *animalClassRef_->GetName());

	for (int32 i = 0; i < poolSize_; i++)
	{
		ABaseAnimal* animal_ = GetWorld()->SpawnActor<ABaseAnimal>(animalClassRef_);
		if (!animal_)
		{
			continue;
		}
	}
}

void AAnimalPool::ReturnAnimalToPool(ABaseAnimal* animal_)
{
	if (!animal_)
	{
		UE_LOG(LogTemp, Warning, TEXT("Animal is null!"));
		return;
	}

	if (ARabbit* Rabbit = Cast<ARabbit>(animal_))
	{
		ActiveRabbits.Remove(Rabbit);
		if (RabbitPool.Num() < PoolLimit)
		{
			Rabbit->SetActorHiddenInGame(true);
			Rabbit->SetActorEnableCollision(false);
			Rabbit->GetCharacterMovement()->DisableMovement();

			RabbitPool.Add(Rabbit);
			UE_LOG(LogTemp, Log, TEXT("Rabbit %s returned to RabbitPool."), *Rabbit->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RabbitPool limit reached. Destroying rabbit %s."), *Rabbit->GetName());
			Rabbit->Destroy();
		}
		return;
	}

	if (ARat* Rat = Cast<ARat>(animal_))
	{
		ActiveRats.Remove(Rat);
		if (RatPool.Num() < PoolLimit)
		{
			Rat->SetActorHiddenInGame(true);
			Rat->SetActorEnableCollision(false);
			Rat->GetCharacterMovement()->DisableMovement();

			RatPool.Add(Rat);
			UE_LOG(LogTemp, Log, TEXT("Rat %s returned to RatPool."), *Rat->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RatPool limit reached. Destroying rat %s."), *Rat->GetName());
			Rat->Destroy();
		}
		return;
	}

	if (ASquirrel* Squirrel = Cast<ASquirrel>(animal_))
	{
		ActiveSquirrels.Remove(Squirrel);
		if (SquirrelPool.Num() < PoolLimit)
		{
			Squirrel->SetActorHiddenInGame(true);
			Squirrel->SetActorEnableCollision(false);
			Squirrel->GetCharacterMovement()->DisableMovement();

			SquirrelPool.Add(Squirrel);
			UE_LOG(LogTemp, Log, TEXT("Squirrel %s returned to SquirrelPool."), *Squirrel->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SquirrelPool limit reached. Destroying squirrel %s."), *Squirrel->GetName());
			Squirrel->Destroy();
		}
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("%s returned to pool successfully."), *animal_->GetName());
}

int32 AAnimalPool::GetTotalAnimalsInPool() const
{
	return RabbitPool.Num() + RatPool.Num() + SquirrelPool.Num();
}

int32 AAnimalPool::GetTotalActiveAnimals() const
{
	return ActiveRabbits.Num() + ActiveRats.Num() + ActiveSquirrels.Num();
}