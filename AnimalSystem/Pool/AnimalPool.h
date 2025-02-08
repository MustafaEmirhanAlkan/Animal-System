#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnimalPool.generated.h"

class ABaseAnimal;
class ARabbit;
class ARat;
class ASquirrel;

UCLASS()
class ANIMALSYSTEM_API AAnimalPool : public AActor
{
	GENERATED_BODY()

public:
	AAnimalPool();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
		TArray<ARabbit*> RabbitPool;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
		TArray<ARat*> RatPool;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
		TArray<ASquirrel*> SquirrelPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Pool")
		int32 PoolLimit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active")
		TArray<ABaseAnimal*> ActiveAnimals;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active")
		TArray<ARabbit*> ActiveRabbits;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active")
		TArray<ARat*> ActiveRats;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active")
		TArray<ASquirrel*> ActiveSquirrels;

	UPROPERTY(EditDefaultsOnly, Category = "Animal Pool")
		TSubclassOf<ABaseAnimal> RabbitClass;
	UPROPERTY(EditDefaultsOnly, Category = "Animal Pool")
		TSubclassOf<ABaseAnimal> RatClass;
	UPROPERTY(EditDefaultsOnly, Category = "Animal Pool")
		TSubclassOf<ABaseAnimal> SquirrelsClass;

	FTimerHandle RespawnTimerHandle;

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	void InitializePool(int32 poolSize_, TSubclassOf<ABaseAnimal> rabbitClassRef_);
	void ReturnAnimalToPool(ABaseAnimal* animal_);
	int32 GetTotalAnimalsInPool() const;
	int32 GetTotalActiveAnimals() const;
	int32 GetAvailableAnimalCount(TSubclassOf<ABaseAnimal> AnimalClass);
	ABaseAnimal* GetAnimalFromPool(TArray<ABaseAnimal*>& pool_);
};