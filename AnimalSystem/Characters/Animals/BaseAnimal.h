#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseAnimal.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class USkeletalMeshComponent;
class USphereComponent;
class UCapsuleComponent;
class AAnimalPool;

UCLASS()
class ANIMALSYSTEM_API ABaseAnimal : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseAnimal();

	UPROPERTY()
		int32 FrameHitCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
		int32 HitCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visibility")
		bool bCanSeePlayer = false;
	UPROPERTY()
		FTimerHandle VisibilityCheckTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* AnimalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USphereComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
		class UAnimalAnimInstance* AnimalAnimInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Stats")
		float RunSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Stats")
		float AlertDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animal Stats")
		float EscapeDistance;

	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void HandleDeath(FString AnimalType);

	FTimerHandle destroyTimerHandle;
	FTimerHandle respawnTimerHandle;
	FTimerHandle poolReturnTimerHandle;
	FTimerHandle visibilityCheckTimerHandle;

protected:
	virtual void BeginPlay() override;

};