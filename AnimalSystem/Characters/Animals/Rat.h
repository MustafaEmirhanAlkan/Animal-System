#pragma once

#include "CoreMinimal.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Rat.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UCapsuleComponent;

UCLASS()
class ANIMALSYSTEM_API ARat : public ABaseAnimal
{
	GENERATED_BODY()

public:
	ARat();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float RatHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Rat")
		TSubclassOf<AActor> RatInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float RatMinSpawnDistance = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float RatMaxSpawnDistance = 1000.0f;

	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	bool bCanSeePlayer = false;
	int32 HitCount = 0;

};
