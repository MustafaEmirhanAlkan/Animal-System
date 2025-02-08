#pragma once

#include "CoreMinimal.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Squirrel.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UCapsuleComponent;

UCLASS()
class ANIMALSYSTEM_API ASquirrel : public ABaseAnimal
{
	GENERATED_BODY()

public:
	ASquirrel();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float SquirrelHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Squirrel")
		TSubclassOf<AActor> SquirrelInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float SquirrelMinSpawnDistance = 800.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float SquirrelMaxSpawnDistance = 1500.0f;
	
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	bool bCanSeePlayer = false;
	int32 HitCount = 0;

};
