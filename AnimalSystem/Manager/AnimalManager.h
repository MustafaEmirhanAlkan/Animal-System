#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnimalSystem/Pool/AnimalPool.h"
#include "AnimalSystem/Characters/Animals/Rabbit.h"
#include "AnimalSystem/Characters/Animals/Rat.h"
#include "AnimalSystem/Characters/Animals/Squirrel.h"
#include "AnimalSystem/Pool/AnimalSpawnRegion.h"
#include "AnimalManager.generated.h"

class AAnimalSystemCharacter;

UCLASS(BlueprintType, Blueprintable)
class ANIMALSYSTEM_API AAnimalManager : public AActor
{
	GENERATED_BODY()

public:
	AAnimalManager();

	UPROPERTY()
		TArray<AAnimalSystemCharacter*> PlayerCharacters;
	UPROPERTY()
		int32 CurrentCharacterIndex = 0;

	UPROPERTY()
		int32 CurrentRaycastCharacterIndex = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		int32 RayCheckCountPerAnimal = 3;

	UPROPERTY(BlueprintReadWrite, Category = "Animal Pool")
		AAnimalPool* AnimalPool;
	//UPROPERTY(BlueprintReadWrite, Category = "Animal Pool")
	//	AAnimalPool* RabbitPool;
	//UPROPERTY(BlueprintReadWrite, Category = "Animal Pool")
	//	AAnimalPool* RatPool;
	//UPROPERTY(BlueprintReadWrite, Category = "Animal Pool")
	//	AAnimalPool* SquirrelPool;

	//TArray<ARabbit*> ActiveRabbits;
	//TArray<ARat*> ActiveRats;
	//TArray<ASquirrel*> ActiveSquirrels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rabbit")
		TSubclassOf<ABaseAnimal> RabbitClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rat")
		TSubclassOf<ABaseAnimal> RatClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squirrel")
		TSubclassOf<ABaseAnimal> SquirrelClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Pool")
		int32 PoolLimit = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Pool")
		int32 RatPoolSize = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Pool")
		int32 RabbitPoolSize = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Pool")
		int32 SquirrelPoolSize = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Pool")
		int32 ActiveLimit = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Pool")
		int32 RatActiveSize = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Pool")
		int32 RabbitActiveSize = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Pool")
		int32 SquirrelActiveSize = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Spawn")
		float RespawnInterval = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rabbit Spawn")
		float RabbitSpawnInterval = 6.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rat Spawn")
		float RatSpawnInterval = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squirrel Spawn")
		float SquirrelSpawnInterval = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Spawn")
		float SuccessRespawnInterval = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Spawn")
		float FailRespawnInterval = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Spawn")
		float PoolEmptyRespawnInterval = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rabbit")
		float RabbitHealth = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rat")
		float RatHealth = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Squirrel")
		float SquirrelHealth = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rabbit")
		bool bIsDead = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed_Rabbit = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed_Rat = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float Speed_Squirrel = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float OffsetAboveGround = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float CenterForwardPosition = -500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Checks")
		float ReturnPoolDistanceDead = 3000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Checks")
		float ReturnPoolDistanceAlive = 5000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Area")
		float CharacterCircleRadius = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Area")
		float RandomSpawnRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float CanSeePlayerRayInterval = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float SpawnAttemptInterval = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float ReturningHoleInterval = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float RabbitMinSpawnDistance = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float RabbitMaxSpawnDistance = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float RatMinSpawnDistance = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float RatMaxSpawnDistance = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float SquirrelMinSpawnDistance = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float SquirrelMaxSpawnDistance = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Regions")
		TArray<AAnimalSpawnRegion*> CachedSpawnRegions;

	virtual void Tick(float DeltaTime) override;
	void InitializePlayerCharacters();
	void PerformRaycastForAnimal(ABaseAnimal* Animal, AAnimalSystemCharacter* Character);
	void ActivateAnimalFromPool(ABaseAnimal* animal_, FVector newLocation_, FRotator newRotation_);
	void CheckDistanceAndVisibility(ABaseAnimal* animal_);
	void HandleVisibilityAndDistanceForAnimal(float& Health, int32& HitCount, bool& bCanSeePlayer, FTimerHandle& VisibilityCheckTimerHandle, ABaseAnimal* Animal, float Distance);
	void SpawnNewAnimal(TSubclassOf<ABaseAnimal> animalClass_);
	void SpawnAnimalInSelectedRegion();
	void TrySpawnRabbit();
	void TrySpawnRat();
	void TrySpawnSquirrel();
	void TrySpawnAnimal(TSubclassOf<ABaseAnimal> AnimalClass);
	bool IsDistanceValidForAnimal(TSubclassOf<ABaseAnimal> AnimalClass, float Distance);
	bool IsLocationVisibleToPlayer(FVector location, ABaseAnimal* animalToIgnore, bool incrementHitCount);
	AAnimalSpawnRegion* GetCurrentRegionForCharacter();
	AAnimalSpawnRegion* GetRegionForLocation(const FVector& Location);
	AAnimalSpawnRegion* GetCurrentRegionForAnimal(const FVector& Location);
	FVector GenerateRandomPointNearPlayer(const FVector& CharacterLocation);


protected:

	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
		FTimerHandle spawnTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
		FTimerHandle rabbitSpawnTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
		FTimerHandle ratSpawnTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
		FTimerHandle squirrelSpawnTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
		FTimerHandle destroyTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
		FTimerHandle respawnTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
		FTimerHandle visibilityCheckTimerHandle;
};