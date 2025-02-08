#pragma once

#include "CoreMinimal.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "Rabbit.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UCapsuleComponent;

UCLASS()
class ANIMALSYSTEM_API ARabbit : public ABaseAnimal
{
	GENERATED_BODY()

public:
	ARabbit();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float RabbitHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Rabbit")
		TSubclassOf<AActor> RabbitInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float RabbitMinSpawnDistance = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
		float RabbitMaxSpawnDistance = 1000.0f;

	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	bool bCanSeePlayer = false;
	int32 HitCount = 0;

};
