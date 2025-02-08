#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AnimalSystem/Pool/AnimalPool.h"
#include "AnimalSystem/Manager/AnimalManager.h"
#include "AnimalSystem/UI/Widgets/PoolCountWidget.h"
#include "AnimalSystemCharacter.generated.h"

class UPoolCountWidget;

UCLASS(config = Game)
class AAnimalSystemCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AAnimalSystemCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
		TSubclassOf<UUserWidget> PoolCountWidgetClass;
	UPROPERTY()
		UPoolCountWidget* PoolCountWidget_BP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		TSubclassOf<ABaseAnimal> RabbitBlueprintClass; //F
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		TSubclassOf<ABaseAnimal> RatBlueprintClass; //G
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		TSubclassOf<ABaseAnimal> SquirrelBlueprintClass; //H

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
		float VisionAngle = 140.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
		float VisionHeightOffset = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vision")
		float VisionDistance = 2000.0f;

	void UpdateRabbitStatsOnWidget();
	void UpdateRatStatsOnWidget();
	void UpdateSquirrelStatsOnWidget();

	void MoveForward(float value_);
	void MoveRight(float value_);
	void DetectObjectsInVision();
	void DisplayVisibleActors(const TArray<AActor*>& VisibleActors);
	void DrawVision();
	void TurnAtRate(float rate_);
	void LookUpAtRate(float rate_);
	void TouchStarted(ETouchIndex::Type fingerIndex, FVector location);
	void TouchStopped(ETouchIndex::Type fingerIndex, FVector location);
	void OnResetVR();
	FVector GetEyeLocation() const;
	FVector GetEyeDirection() const;

	UFUNCTION(BlueprintCallable, Category = "Raycast")
		void PerformRaycast();
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_PerformRaycast();
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_DrawRay(FVector start_, FVector end_, FVector targetPoint_);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* playerInputComponent_) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& outLifetimeProps_) const override;
	virtual void BeginPlay() override;
	void Tick(float DeltaTime);
	void UpdateAnimalStatsOnWidget();
	void Server_PerformRaycast_Implementation();
	void Multicast_DrawRay_Implementation(FVector start_, FVector end_, FVector targetPoint_);
	void SpawnRabbitOnKeyPress();
	void SpawnRatOnKeyPress();
	void SpawnSquirrelOnKeyPress();
	void SpawnAnimal(TSubclassOf<ABaseAnimal> AnimalClass);
	bool Server_PerformRaycast_Validate();

	FTimerHandle spawnTimerHandle;
};