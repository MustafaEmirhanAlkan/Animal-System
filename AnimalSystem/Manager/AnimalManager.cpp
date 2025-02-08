#include "AnimalSystem/Manager/AnimalManager.h"
#include "AnimalSystem/Characters/PlayerCharacter/AnimalSystemCharacter.h"
#include "AnimalSystem/AI/BlackboardKeys/AnimalBlackboardKeys.h"
#include "AnimalSystem/AI/Controller/AnimalAIController.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSystem/Animation/AnimalAnimInstance.h"
#include "AnimalSystem/Characters/Animals/Rabbit.h"
#include "AnimalSystem/Characters/Animals/Rat.h"
#include "AnimalSystem/Characters/Animals/Squirrel.h"
#include "AnimalSystem/Pool/AnimalSpawnRegion.h"
#include "AnimalSystem/Pool/AnimalPool.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

AAnimalManager::AAnimalManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAnimalManager::BeginPlay()
{
	Super::BeginPlay();
	InitializePlayerCharacters();

	RespawnInterval = 10.0f;
	//MP1
	bReplicates = true;

	UWorld* world_ = GetWorld();
	AnimalPool = world_->SpawnActor<AAnimalPool>(AAnimalPool::StaticClass());

	if (RabbitClass && RabbitClass->IsChildOf(ABaseAnimal::StaticClass()))
	{
		AnimalPool->InitializePool(RabbitPoolSize, RabbitClass);
		if (AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			Character->UpdateRabbitStatsOnWidget();
		}
	}

	if (RatClass && RatClass->IsChildOf(ABaseAnimal::StaticClass()))
	{
		AnimalPool->InitializePool(RatPoolSize, RatClass);
		if (AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			Character->UpdateRatStatsOnWidget();
		}
	}

	if (SquirrelClass && SquirrelClass->IsChildOf(ABaseAnimal::StaticClass()))
	{
		AnimalPool->InitializePool(SquirrelPoolSize, SquirrelClass);
		if (AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			Character->UpdateSquirrelStatsOnWidget();
		}
	}

	if (!RabbitClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RabbitClass is null in AAnimalManager!"));
	}
	if (!RatClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RatClass is null in AAnimalManager!"));
	}
	if (!SquirrelClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SquirrelClass is null in AAnimalManager!"));
	}

	if (RabbitPoolSize <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("RabbitPoolSize is invalid: %d"), RabbitPoolSize);
	}
	if (RatPoolSize <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("RatPoolSize is invalid: %d"), RatPoolSize);
	}
	if (SquirrelPoolSize <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("SquirrelPoolSize is invalid: %d"), SquirrelPoolSize);
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAnimalSpawnRegion::StaticClass(), FoundActors);

	CachedSpawnRegions.Empty();
	for (AActor* Actor : FoundActors)
	{
		if (AAnimalSpawnRegion* Region = Cast<AAnimalSpawnRegion>(Actor))
		{
			CachedSpawnRegions.Add(Region);
		}
	}

	if (CachedSpawnRegions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spawn regions found!"));
		return;
	}

	GetWorldTimerManager().SetTimer(rabbitSpawnTimerHandle, [this]()
		{
			TrySpawnAnimal(RabbitClass);
		}, RabbitSpawnInterval, true);

	GetWorldTimerManager().SetTimer(ratSpawnTimerHandle, [this]()
		{
			TrySpawnAnimal(RatClass);
		}, RatSpawnInterval, true);

	GetWorldTimerManager().SetTimer(squirrelSpawnTimerHandle, [this]()
		{
			TrySpawnAnimal(SquirrelClass);
		}, SquirrelSpawnInterval, true);
}

void AAnimalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacters.Num() == 0)
	{
		return;
	}

	AAnimalSystemCharacter* CurrentCharacter = PlayerCharacters[CurrentRaycastCharacterIndex];
	if (CurrentCharacter)
	{
		TArray<ARabbit*> RabbitCopy = AnimalPool->ActiveRabbits;
		for (ARabbit* Rabbit : RabbitCopy)
		{
			if (Rabbit)
			{
				HandleVisibilityAndDistanceForAnimal(
					Rabbit->RabbitHealth, Rabbit->HitCount, Rabbit->bCanSeePlayer,
					Rabbit->VisibilityCheckTimerHandle, Rabbit,
					FVector::Dist(Rabbit->GetActorLocation(), CurrentCharacter->GetActorLocation())
				);
			}
		}

		TArray<ARat*> RatCopy = AnimalPool->ActiveRats;
		for (ARat* Rat : RatCopy)
		{
			if (Rat)
			{
				HandleVisibilityAndDistanceForAnimal(
					Rat->RatHealth, Rat->HitCount, Rat->bCanSeePlayer,
					Rat->VisibilityCheckTimerHandle, Rat,
					FVector::Dist(Rat->GetActorLocation(), CurrentCharacter->GetActorLocation())
				);
			}
		}

		TArray<ASquirrel*> SquirrelCopy = AnimalPool->ActiveSquirrels;
		for (ASquirrel* Squirrel : SquirrelCopy)
		{
			if (Squirrel)
			{
				HandleVisibilityAndDistanceForAnimal(
					Squirrel->SquirrelHealth, Squirrel->HitCount, Squirrel->bCanSeePlayer,
					Squirrel->VisibilityCheckTimerHandle, Squirrel,
					FVector::Dist(Squirrel->GetActorLocation(), CurrentCharacter->GetActorLocation())
				);
			}
		}
	}

	CurrentRaycastCharacterIndex = (CurrentRaycastCharacterIndex + 1) % PlayerCharacters.Num();

	AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Character)
	{
		FVector CharacterLocation = Character->GetActorLocation();

		DrawDebugSphere(GetWorld(), CharacterLocation, RabbitMinSpawnDistance, 32, FColor::Green, false, -1.0f, 0, 1.0f);
		DrawDebugSphere(GetWorld(), CharacterLocation, RabbitMaxSpawnDistance, 32, FColor::Red, false, -1.0f, 0, 1.0f);
		
		DrawDebugSphere(GetWorld(), CharacterLocation, RatMinSpawnDistance, 32, FColor::Blue, false, -1.0f, 0, 1.0f);
		DrawDebugSphere(GetWorld(), CharacterLocation, RatMaxSpawnDistance, 32, FColor::Yellow, false, -1.0f, 0, 1.0f);
		
		DrawDebugSphere(GetWorld(), CharacterLocation, SquirrelMinSpawnDistance, 32, FColor::Purple, false, -1.0f, 0, 1.0f);
		DrawDebugSphere(GetWorld(), CharacterLocation, SquirrelMaxSpawnDistance, 32, FColor::Cyan, false, -1.0f, 0, 1.0f);
	
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green,
			FString::Printf(TEXT("Spawning for character at index %d"), CurrentCharacterIndex));
	}
}

void AAnimalManager::InitializePlayerCharacters()
{
	PlayerCharacters.Empty();

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (AAnimalSystemCharacter* PlayerCharacter = Cast<AAnimalSystemCharacter>(Iterator->Get()->GetPawn()))
		{
			PlayerCharacters.Add(PlayerCharacter);
		}
	}

	if (PlayerCharacters.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No player characters found in the world!"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Found %d player characters."), PlayerCharacters.Num());
	}
}

void AAnimalManager::PerformRaycastForAnimal(ABaseAnimal* Animal, AAnimalSystemCharacter* Character)
{
	if (!Animal || !Character)
	{
		return;
	}

	FVector StartLocation = Animal->GetActorLocation();
	FVector EndLocation = Character->GetActorLocation() + FVector(0, 0, 50.0f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Animal);

	bool bObstacleDetected = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, bObstacleDetected ? FColor::Red : FColor::Green, false, 0.1f, 0, 1.0f);

	if (bObstacleDetected)
	{
		Animal->FrameHitCount++;

		UE_LOG(LogTemp, Log, TEXT("%s hit obstacle for character %s. Hit Count: %d"), *Animal->GetName(), *Character->GetName(), Animal->FrameHitCount);

		if (Animal->FrameHitCount >= RayCheckCountPerAnimal)
		{
			if (ARabbit* Rabbit = Cast<ARabbit>(Animal))
			{
				AnimalPool->ReturnAnimalToPool(Rabbit);
				UE_LOG(LogTemp, Log, TEXT("%s is returning to pool after hitting obstacles %d times."), *Animal->GetName(), Animal->FrameHitCount);
			}
			else if (ARat* Rat = Cast<ARat>(Animal))
			{
				AnimalPool->ReturnAnimalToPool(Rat);
				UE_LOG(LogTemp, Log, TEXT("%s is returning to pool after hitting obstacles %d times."), *Animal->GetName(), Animal->FrameHitCount);
			}
			else if (ASquirrel* Squirrel = Cast<ASquirrel>(Animal))
			{
				AnimalPool->ReturnAnimalToPool(Squirrel);
				UE_LOG(LogTemp, Log, TEXT("%s is returning to pool after hitting obstacles %d times."), *Animal->GetName(), Animal->FrameHitCount);
			}
		}
	}
	else
	{
		Animal->FrameHitCount = 0;
	}
}

void AAnimalManager::ActivateAnimalFromPool(ABaseAnimal* animal_, FVector newLocation_, FRotator newRotation_)
{
	AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (animal_)
	{
		ARabbit* Rabbit = Cast<ARabbit>(animal_);
		ARat* Rat = Cast<ARat>(animal_);
		ASquirrel* Squirrel = Cast<ASquirrel>(animal_);

		if (Rabbit != nullptr && AnimalPool->ActiveRabbits.Num() <= RabbitPoolSize)
		{
			if (AnimalPool && !AnimalPool->ActiveRabbits.Contains(Rabbit))
			{
				AnimalPool->ActiveRabbits.Add(Rabbit);
				AnimalPool->ActiveAnimals.Add(Rabbit);
				Character->UpdateRabbitStatsOnWidget();
			}
			AnimalPool->RabbitPool.Remove(Rabbit);
		}
		if (Rat != nullptr && AnimalPool->ActiveRats.Num() <= RatPoolSize)
		{
			if (AnimalPool && !AnimalPool->ActiveRats.Contains(Rat))
			{
				AnimalPool->ActiveRats.Add(Rat);
				AnimalPool->ActiveAnimals.Add(Rat);
				Character->UpdateRatStatsOnWidget();
			}
			AnimalPool->RatPool.Remove(Rat);
		}
		if (Squirrel != nullptr && AnimalPool->ActiveSquirrels.Num() <= SquirrelPoolSize)
		{
			if (AnimalPool && !AnimalPool->ActiveSquirrels.Contains(Squirrel))
			{
				AnimalPool->ActiveSquirrels.Add(Squirrel);
				AnimalPool->ActiveAnimals.Add(Squirrel);
				Character->UpdateSquirrelStatsOnWidget();
			}
			AnimalPool->SquirrelPool.Remove(Squirrel);
		}

		animal_->SetActorLocation(newLocation_);
		animal_->SetActorRotation(newRotation_);
		animal_->SetActorHiddenInGame(false);
		animal_->SetActorEnableCollision(true);

		if (UCharacterMovementComponent* Movement = animal_->GetCharacterMovement())
		{
			Movement->SetMovementMode(EMovementMode::MOVE_Walking);
		}

		if (UAnimalAnimInstance* AnimInstance = Cast<UAnimalAnimInstance>(animal_->AnimalMesh->GetAnimInstance()))
		{
			AnimInstance->bIsDead = false;
			AnimInstance->UpdateAnimationProperties(0.0f);
			AnimInstance->PlayWalkAnimation();
		}

		if (AAnimalAIController* AIController = Cast<AAnimalAIController>(animal_->GetController()))
		{
			if (Rabbit && AIController->RabbitBehaviorTree)
			{
				AIController->RunBehaviorTree(AIController->RabbitBehaviorTree);
				if (AIController->GetBlackboardComponent())
				{
					AIController->GetBlackboardComponent()->SetValueAsVector(AnimalBlackboardKeys::target_location, newLocation_);
					AIController->GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::can_see_player, false);
					AIController->GetBlackboardComponent()->SetValueAsObject(AnimalBlackboardKeys::player_target, nullptr);
					AIController->GetBlackboardComponent()->SetValueAsFloat(AnimalBlackboardKeys::distance_to_enemy, 0.0f);
					AIController->GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::is_running, false);
					AIController->GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::is_walking, true);
					AIController->GetBlackboardComponent()->SetValueAsFloat(AnimalBlackboardKeys::health, Rabbit->RabbitHealth);
				}
			}
			else if (Rat && AIController->RatBehaviorTree)
			{
				AIController->RunBehaviorTree(AIController->RatBehaviorTree);
				if (AIController->GetBlackboardComponent())
				{
					AIController->GetBlackboardComponent()->SetValueAsVector(AnimalBlackboardKeys::target_location, newLocation_);
					AIController->GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::can_see_player, false);
					AIController->GetBlackboardComponent()->SetValueAsObject(AnimalBlackboardKeys::player_target, nullptr);
					AIController->GetBlackboardComponent()->SetValueAsFloat(AnimalBlackboardKeys::distance_to_enemy, 0.0f);
					AIController->GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::is_running, false);
					AIController->GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::is_walking, true);
					AIController->GetBlackboardComponent()->SetValueAsFloat(AnimalBlackboardKeys::health, Rat->RatHealth);
				}
			}
			else if (Squirrel && AIController->SquirrelBehaviorTree)
			{
				AIController->RunBehaviorTree(AIController->SquirrelBehaviorTree);
				if (AIController->GetBlackboardComponent())
				{
					AIController->GetBlackboardComponent()->SetValueAsVector(AnimalBlackboardKeys::target_location, newLocation_);
					AIController->GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::can_see_player, false);
					AIController->GetBlackboardComponent()->SetValueAsObject(AnimalBlackboardKeys::player_target, nullptr);
					AIController->GetBlackboardComponent()->SetValueAsFloat(AnimalBlackboardKeys::distance_to_enemy, 0.0f);
					AIController->GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::is_running, false);
					AIController->GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::is_walking, true);
					AIController->GetBlackboardComponent()->SetValueAsFloat(AnimalBlackboardKeys::health, Squirrel->SquirrelHealth);
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Animal activated: %s"), *animal_->GetName());
		GetWorldTimerManager().SetTimerForNextTick([this, animal_]()
			{
				CheckDistanceAndVisibility(animal_);
			});
	}

	UE_LOG(LogTemp, Log, TEXT("Active Animals: %d, Active Rabbits: %d, Active Rats: %d, Active Squirrels: %d"),
		AnimalPool->ActiveRabbits.Num() + AnimalPool->ActiveRats.Num() + AnimalPool->ActiveSquirrels.Num(),
		AnimalPool->ActiveRabbits.Num(),
		AnimalPool->ActiveRats.Num(),
		AnimalPool->ActiveSquirrels.Num());
}

void AAnimalManager::CheckDistanceAndVisibility(ABaseAnimal* animal_)
{
	if (!animal_ || !AnimalPool)
	{
		return;
	}

	AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Character)
	{
		return;
	}

	ARabbit* Rabbit = Cast<ARabbit>(animal_);
	ARat* Rat = Cast<ARat>(animal_);
	ASquirrel* Squirrel = Cast<ASquirrel>(animal_);

	float distance = FVector::Dist(animal_->GetActorLocation(), Character->GetActorLocation());

	if (Rabbit)
	{
		HandleVisibilityAndDistanceForAnimal(Rabbit->RabbitHealth, Rabbit->HitCount, Rabbit->bCanSeePlayer,
			Rabbit->visibilityCheckTimerHandle, Rabbit, distance);
	}
	else if (Rat)
	{
		HandleVisibilityAndDistanceForAnimal(Rat->RatHealth, Rat->HitCount, Rat->bCanSeePlayer,
			Rat->visibilityCheckTimerHandle, Rat, distance);
	}
	else if (Squirrel)
	{
		HandleVisibilityAndDistanceForAnimal(Squirrel->SquirrelHealth, Squirrel->HitCount, Squirrel->bCanSeePlayer,
			Squirrel->visibilityCheckTimerHandle, Squirrel, distance);
	}
}

void AAnimalManager::HandleVisibilityAndDistanceForAnimal(float& Health, int32& HitCount, bool& bCanSeePlayer, FTimerHandle& VisibilityCheckTimerHandle, ABaseAnimal* Animal, float Distance)
{
	bIsDead = (Health <= 0.0f);
	float ReturnPoolDistance = bIsDead ? ReturnPoolDistanceDead : ReturnPoolDistanceAlive;

	if (Distance > ReturnPoolDistance)
	{
		bool bObstacleDetectedForAllCharacters = false;

		for (int32 i = 0; i < PlayerCharacters.Num(); i++)
		{
			AAnimalSystemCharacter* CurrentCharacter = PlayerCharacters[CurrentRaycastCharacterIndex];
			if (!CurrentCharacter)
			{
				continue;
			}

			FVector StartLocation = Animal->GetActorLocation();
			FVector EndLocation = CurrentCharacter->GetActorLocation() + FVector(0, 0, 50.0f);

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(Animal);

			bool bObstacleDetected = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, bObstacleDetected ? FColor::Red : FColor::Green, false, 0.1f, 0, 1.0f);

			if (bObstacleDetected)
			{
				bObstacleDetectedForAllCharacters = true;
				Animal->FrameHitCount++;

				UE_LOG(LogTemp, Log, TEXT("%s hit obstacle for character %s. Hit Count: %d"), *Animal->GetName(), *CurrentCharacter->GetName(), Animal->FrameHitCount);

				if (Animal->FrameHitCount >= RayCheckCountPerAnimal)
				{
 					if (ARabbit* Rabbit = Cast<ARabbit>(Animal))
					{
						AnimalPool->ReturnAnimalToPool(Rabbit);
						UE_LOG(LogTemp, Log, TEXT("%s is returning to pool after hitting obstacles %d times."), *Animal->GetName(), Animal->FrameHitCount);
					}
					else if (ARat* Rat = Cast<ARat>(Animal))
					{
						AnimalPool->ReturnAnimalToPool(Rat);
						UE_LOG(LogTemp, Log, TEXT("%s is returning to pool after hitting obstacles %d times."), *Animal->GetName(), Animal->FrameHitCount);
					}
					else if (ASquirrel* Squirrel = Cast<ASquirrel>(Animal))
					{
						AnimalPool->ReturnAnimalToPool(Squirrel);
						UE_LOG(LogTemp, Log, TEXT("%s is returning to pool after hitting obstacles %d times."), *Animal->GetName(), Animal->FrameHitCount);
					}
					return;
				}
			}
		}

		if (!bObstacleDetectedForAllCharacters)
		{
			Animal->FrameHitCount = 0;
		}

		bCanSeePlayer = !bObstacleDetectedForAllCharacters;
	}
	else
	{
		HitCount = 0;
	}

	CurrentRaycastCharacterIndex = (CurrentRaycastCharacterIndex + 1) % PlayerCharacters.Num();

	GetWorldTimerManager().SetTimer(VisibilityCheckTimerHandle, [this, Animal]()
		{
			CheckDistanceAndVisibility(Animal);
		}, CanSeePlayerRayInterval, false);
}

bool AAnimalManager::IsLocationVisibleToPlayer(FVector location, ABaseAnimal* animalToIgnore, bool incrementHitCount)
{
	if (PlayerCharacters.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No player characters available for visibility check."));
		return false;
	}

	for (AAnimalSystemCharacter* Character : PlayerCharacters)
	{
		if (!Character)
		{
			continue;
		}

		FVector eyeLocation = Character->GetActorLocation() + FVector(0, 0, 50.0f);
		FHitResult hitResult;
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(animalToIgnore);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, location, eyeLocation, ECC_Visibility, queryParams);
		AActor* HitActor = hitResult.GetActor();

		if (bHit && HitActor)
		{
			if (HitActor->IsA(AAnimalSystemCharacter::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Ray hit the player. No obstacle detected."));
				return true;
			}

			if (HitActor->IsA(AAnimalSpawnRegion::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Ray hit non-obstacle object: %s"), *HitActor->GetName());
				return true;
			}

			if (incrementHitCount)
			{
				if (ARabbit* Rabbit = Cast<ARabbit>(animalToIgnore))
				{
					Rabbit->HitCount++;
					UE_LOG(LogTemp, Log, TEXT("Rabbit ray hit object %s, Hit Count: %d"), *HitActor->GetName(), Rabbit->HitCount);
				}
				else if (ARat* Rat = Cast<ARat>(animalToIgnore))
				{
					Rat->HitCount++;
					UE_LOG(LogTemp, Log, TEXT("Rat ray hit object %s, Hit Count: %d"), *HitActor->GetName(), Rat->HitCount);
				}
				else if (ASquirrel* Squirrel = Cast<ASquirrel>(animalToIgnore))
				{
					Squirrel->HitCount++;
					UE_LOG(LogTemp, Log, TEXT("Squirrel ray hit object %s, Hit Count: %d"), *HitActor->GetName(), Squirrel->HitCount);
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Ray hit blocking object: %s"), *HitActor->GetName());
			return false;
		}
	}
	return true;
}

FVector AAnimalManager::GenerateRandomPointNearPlayer(const FVector& CharacterLocation)
{
	FVector ForwardVector = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorForwardVector();
	FVector AreaCenter = CharacterLocation + ForwardVector * RandomSpawnRadius;

	float RandomAngle = FMath::RandRange(0.0f, 360.0f);
	float RandomRadius = FMath::RandRange(0.0f, RandomSpawnRadius);

	FVector RandomOffset = FVector(FMath::Cos(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
		FMath::Sin(FMath::DegreesToRadians(RandomAngle)) * RandomRadius,
		0.0f);

	return AreaCenter + RandomOffset;

	if (AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		FVector characterLocation = Character->GetActorLocation();
		FVector forwardVector = Character->GetActorForwardVector();
		FRotator characterRotation = Character->GetActorRotation();
		FVector areaCenterOffset = characterRotation.RotateVector(FVector(CharacterCircleRadius + CenterForwardPosition, 0, -OffsetAboveGround));
		FVector areaCenter = characterLocation + areaCenterOffset;

		float randomAngle = FMath::RandRange(0.0f, 360.0f);
		float randomRadius = FMath::FRandRange(0.0f, RandomSpawnRadius);
		FVector randomOffset = FVector(FMath::Cos(FMath::DegreesToRadians(randomAngle)) * randomRadius,
			FMath::Sin(FMath::DegreesToRadians(randomAngle)) * randomRadius,
			0.0f);

		FVector randomPoint = areaCenter + randomOffset;

		DrawDebugCircle(GetWorld(), areaCenter, CharacterCircleRadius, 64, FColor::Blue, false, 1.0f, 0, RespawnInterval, FVector(1, 0, 0), FVector(0, 1, 0), false);

		return randomPoint;
	}
	return FVector::ZeroVector;
}

void AAnimalManager::TrySpawnRabbit()
{
	TrySpawnAnimal(RabbitClass);
}

void AAnimalManager::TrySpawnRat()
{
	TrySpawnAnimal(RatClass);
}

void AAnimalManager::TrySpawnSquirrel()
{
	TrySpawnAnimal(SquirrelClass);
}

void AAnimalManager::TrySpawnAnimal(TSubclassOf<ABaseAnimal> animalClass_)
{
	if (PlayerCharacters.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No player characters available for spawning!"));
		return;
	}
	AAnimalSystemCharacter* CurrentCharacter = PlayerCharacters[CurrentCharacterIndex];
	if (!CurrentCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid player character at index %d!"), CurrentCharacterIndex);
		return;
	}

	if (AnimalPool->ActiveRabbits.Num() >= PoolLimit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max active rabbits reached. Cannot spawn more rabbits."));
		return;
	}
	if (AnimalPool->ActiveRats.Num() >= PoolLimit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max active rats reached. Cannot spawn more rats."));
		return;
	}
	if (AnimalPool->ActiveSquirrels.Num() >= PoolLimit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max active Squirrels reached. Cannot spawn more Squirrels."));
		return;
	}

	AAnimalSpawnRegion* CurrentRegion = GetCurrentRegionForCharacter();
	if (!CurrentRegion)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player is not in any spawn region."));
		return;
	}

	TSubclassOf<ABaseAnimal> SelectedAnimalClass = CurrentRegion->GetRandomAnimalClass();
	FVector randomLocation = GenerateRandomPointNearPlayer(CurrentCharacter->GetActorLocation());
	float distanceToPlayer_ = FVector::Dist(randomLocation, GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());

	if (!IsDistanceValidForAnimal(animalClass_, distanceToPlayer_))
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn location is not within valid distance range for %s. Retrying."),
			*animalClass_->GetName());
		GetWorldTimerManager().SetTimer(spawnTimerHandle, [this, animalClass_]()
			{
				TrySpawnAnimal(animalClass_);
			},
			FailRespawnInterval, false);
		return;
	}

	if (IsLocationVisibleToPlayer(randomLocation, nullptr, true))
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn location is visible to player. Retrying in %0.1f seconds."), FailRespawnInterval);
		GetWorldTimerManager().SetTimer(spawnTimerHandle, [this, animalClass_]()
			{
				TrySpawnAnimal(animalClass_);
			},
			FailRespawnInterval, false);
		return;
	}

	ABaseAnimal* SpawnedAnimal = nullptr;

	if (RabbitClass->IsChildOf(ARabbit::StaticClass()) && AnimalPool  && AnimalPool->RabbitPool.Num() > 0)
	{
		AnimalPool->ActiveRabbits.Add(Cast<ARabbit>(SpawnedAnimal));
		SpawnedAnimal = AnimalPool->RabbitPool.Pop();
	}
	else if (RatClass->IsChildOf(ARat::StaticClass()) && AnimalPool && AnimalPool->RatPool.Num() > 0)
	{
 		AnimalPool->ActiveRats.Add(Cast<ARat>(SpawnedAnimal));
		SpawnedAnimal = AnimalPool->RatPool.Pop();
	}
	else if (SquirrelClass->IsChildOf(ASquirrel::StaticClass()) && AnimalPool && AnimalPool->SquirrelPool.Num() > 0)
	{
		AnimalPool->ActiveSquirrels.Add(Cast<ASquirrel>(SpawnedAnimal));
		SpawnedAnimal = AnimalPool->SquirrelPool.Pop();
	}
	else
	{
		SpawnNewAnimal(animalClass_);
		UE_LOG(LogTemp, Log, TEXT("%s spawned in region: %s"), *SelectedAnimalClass->GetName(), *CurrentRegion->GetName());
		return;
	}

	if (SpawnedAnimal)
	{
		FVector NewLocation = GenerateRandomPointNearPlayer(PlayerCharacters[CurrentCharacterIndex]->GetActorLocation());
		ActivateAnimalFromPool(SpawnedAnimal, randomLocation, FRotator::ZeroRotator);
		UE_LOG(LogTemp, Log, TEXT("%s spawned in region: %s"), *SpawnedAnimal->GetName(), *CurrentRegion->GetName());
	}

	if (animalClass_ == RabbitClass)
	{
		GetWorldTimerManager().SetTimer(spawnTimerHandle, this, &AAnimalManager::TrySpawnRabbit, RespawnInterval, false);
	}
	else if (animalClass_ == RatClass)
	{
		GetWorldTimerManager().SetTimer(spawnTimerHandle, this, &AAnimalManager::TrySpawnRat, RespawnInterval, false);
	}
	else if (animalClass_ == SquirrelClass)
	{
		GetWorldTimerManager().SetTimer(spawnTimerHandle, this, &AAnimalManager::TrySpawnSquirrel, RespawnInterval, false);
	}

	CurrentCharacterIndex = (CurrentCharacterIndex + 1) % PlayerCharacters.Num();
}

void AAnimalManager::SpawnNewAnimal(TSubclassOf<ABaseAnimal> animalClass_)
{
	if (AnimalPool->ActiveRabbits.Num() >= PoolLimit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max active rabbits reached. Cannot spawn more rabbits."));
		return;
	}
	if (AnimalPool->ActiveRats.Num() >= PoolLimit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max active rats reached. Cannot spawn more rats."));
		return;
	}
	if (AnimalPool->ActiveSquirrels.Num() >= PoolLimit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max active Squirrels reached. Cannot spawn more Squirrels."));
		return;
	}

	if (animalClass_ && animalClass_->IsChildOf(ABaseAnimal::StaticClass()))
	{
		AAnimalSystemCharacter* CurrentCharacter = PlayerCharacters[CurrentCharacterIndex];
		if (!CurrentCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid player character at index %d!"), CurrentCharacterIndex);
			return;
		}

		FVector randomLocation = GenerateRandomPointNearPlayer(CurrentCharacter->GetActorLocation());
		float DistanceToPlayer = FVector::Dist(randomLocation, GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation());

		if (!IsDistanceValidForAnimal(animalClass_, DistanceToPlayer))
		{
			UE_LOG(LogTemp, Warning, TEXT("Spawn location is not within valid distance range for %s. Retrying."),
				*animalClass_->GetName());
			GetWorldTimerManager().SetTimer(spawnTimerHandle, [this, animalClass_]()
				{
					SpawnNewAnimal(animalClass_);
				},
				FailRespawnInterval, false);
			return;
		}

		FHitResult hitResult;
		FVector startLocation = randomLocation + FVector(0, 0, 500);
		FVector endLocation = randomLocation - FVector(0, 0, 1000);

		AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		FVector eyeLocation = Character->GetEyeLocation();

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, randomLocation, eyeLocation, ECC_Visibility);
		if (bHit)
		{
			DrawDebugLine(GetWorld(), randomLocation, eyeLocation, FColor::Green, false, 10.0f, 0, 1.0f);
			DrawDebugSphere(GetWorld(), randomLocation, 50.0f, 12, FColor::Green, false, 10.0f);
		}
		else
		{
			DrawDebugLine(GetWorld(), randomLocation, eyeLocation, FColor::Red, false, 3.0f, 0, 1.0f);
			DrawDebugSphere(GetWorld(), randomLocation, 50.0f, 12, FColor::Red, false, 3.0f);
		}

		if (bHit)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ABaseAnimal* newAnimal = GetWorld()->SpawnActor<ABaseAnimal>(animalClass_, randomLocation, FRotator::ZeroRotator, spawnParams);
			if (newAnimal)
			{
				ActivateAnimalFromPool(newAnimal, randomLocation, FRotator::ZeroRotator);
				UE_LOG(LogTemp, Log, TEXT("%s spawned successfully at random location."), *animalClass_->GetName());

				if (animalClass_->IsChildOf(ARabbit::StaticClass()))
				{
					UE_LOG(LogTemp, Log, TEXT("Rabbit spawned at random location out of player's sight."));
				}
				else if (animalClass_->IsChildOf(ARat::StaticClass()))
				{
					UE_LOG(LogTemp, Log, TEXT("Rat spawned at random location out of player's sight."));
				}
				else if (animalClass_->IsChildOf(ASquirrel::StaticClass()))
				{
					UE_LOG(LogTemp, Log, TEXT("Squirrel spawned at random location out of player's sight."));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Point is visible to player, retrying in 1 second (SpawnAttemptInterval)."));
			GetWorldTimerManager().SetTimer(spawnTimerHandle, [this, animalClass_]()
				{
					SpawnNewAnimal(animalClass_);
				}, SpawnAttemptInterval, false);
		}
	}
	else
	{ 
		if (!animalClass_ || !animalClass_->IsChildOf(ABaseAnimal::StaticClass()))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid animal class or class is not a subclass of ABaseAnimal."));
		}
		else if (AnimalPool->ActiveRabbits.Num() >= PoolLimit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Max active rabbits reached. Cannot spawn more rabbits."));
			return;
		}
		else if (AnimalPool->ActiveRats.Num() >= PoolLimit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Max active rats reached. Cannot spawn more rats."));
			return;
		}
		else if (AnimalPool->ActiveSquirrels.Num() >= PoolLimit)
		{
			UE_LOG(LogTemp, Warning, TEXT("Max active Squirrels reached. Cannot spawn more Squirrels."));
			return;
		}
	}
}

void AAnimalManager::SpawnAnimalInSelectedRegion()
{
	if (CachedSpawnRegions.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spawn regions found in cache!"));
		return;
	}

	AAnimalSpawnRegion* SelectedRegion = CachedSpawnRegions[FMath::RandRange(0, CachedSpawnRegions.Num() - 1)];
	if (SelectedRegion)
	{
		SelectedRegion->SpawnAnimalInRegion();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid region selected."));
	}
}

AAnimalSpawnRegion* AAnimalManager::GetCurrentRegionForCharacter()
{
	AAnimalSystemCharacter* Character = Cast<AAnimalSystemCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player character not found!"));
		return nullptr;
	}

	for (AAnimalSpawnRegion* Region : CachedSpawnRegions)
	{
		if (Region && Region->SpawnRegion->IsOverlappingActor(Character))
		{
			return Region;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("No overlapping spawn region found for player."));
	return nullptr;
}

int32 AAnimalPool::GetAvailableAnimalCount(TSubclassOf<ABaseAnimal> AnimalClass)
{
	int32 Count = 0;

	if (AnimalClass->IsChildOf(ARabbit::StaticClass()))
	{
		Count = RabbitPool.Num();
	}
	else if (AnimalClass->IsChildOf(ARat::StaticClass()))
	{
		Count = RatPool.Num();
	}
	else if (AnimalClass->IsChildOf(ASquirrel::StaticClass()))
	{
		Count = SquirrelPool.Num();
	}

	return Count;
}

bool AAnimalManager::IsDistanceValidForAnimal(TSubclassOf<ABaseAnimal> AnimalClass, float Distance)
{
	float MinDistance = 0.0f;
	float MaxDistance = 0.0f;

	if (AnimalClass->IsChildOf(ARabbit::StaticClass()))
	{
		MinDistance = RabbitMinSpawnDistance;
		MaxDistance = RabbitMaxSpawnDistance;
	}
	else if (AnimalClass->IsChildOf(ARat::StaticClass()))
	{
		MinDistance = RatMinSpawnDistance;
		MaxDistance = RatMaxSpawnDistance;
	}
	else if (AnimalClass->IsChildOf(ASquirrel::StaticClass()))
	{
		MinDistance = SquirrelMinSpawnDistance;
		MaxDistance = SquirrelMaxSpawnDistance;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Unknown animal class: %s"), *AnimalClass->GetName());
		return false;
	}

	return Distance >= MinDistance && Distance <= MaxDistance;
}