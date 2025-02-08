#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSpawnRegion.generated.h"

class AAnimalManager;

USTRUCT(BlueprintType)
struct FAnimalSpawnPercentage
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		TSubclassOf<class ABaseAnimal> AnimalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings", meta = (ClampMin = "0.0", ClampMax = "100.0"))
		float SpawnPercentage;
};

UCLASS()
class ANIMALSYSTEM_API AAnimalSpawnRegion : public AActor
{
	GENERATED_BODY()

public:
	AAnimalSpawnRegion();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Region Settings")
		class UBoxComponent* SpawnRegion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
		TArray<FAnimalSpawnPercentage> AnimalSpawnPercentages;
	UFUNCTION(BlueprintCallable, Category = "Spawn Settings")
		TSubclassOf<ABaseAnimal> GetRandomAnimalClass();
	UFUNCTION(BlueprintCallable, Category = "Spawn Settings")
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void SpawnAnimalInRegion();

	void BeginPlay();
	void DisplayAnimalPercentages(AActor* Character);
	bool IsOverlappingLocation(const FVector& Location) const;
	UPROPERTY()
		AAnimalManager* AnimalManager;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timers")
		FTimerHandle spawnTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float spawnInterval = 5.0f;
};