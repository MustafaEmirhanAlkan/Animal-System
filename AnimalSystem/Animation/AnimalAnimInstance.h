 #pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/BlendSpace.h"
#include "AnimalAnimInstance.generated.h"

UCLASS()
class ANIMALSYSTEM_API UAnimalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UAnimalAnimInstance();

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
		float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Speed_Rabbit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Speed_Rat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Speed_Squirrel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Direction_Rabbit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Direction_Rat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Direction_Squirrel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		bool PlayIdleLoop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
		bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
		UAnimMontage* DeathAnimationMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* IdleMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* RunMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* WalkMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		UAnimMontage* AlertMontage;

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void UpdateAnimationProperties(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayDeathAnimation();
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayIdleAnimation();
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayRunAnimation();
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayWalkAnimation();
	UFUNCTION(BlueprintCallable, Category = "Animation")
		void PlayAlertAnimation();

	UFUNCTION(BlueprintCallable, Category = "Animation")
		void UpdateAnimationStates();

	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bIsInAir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		APawn* pawnOwner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		UBlendSpace* rabbit_MovementBlendSpace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		UBlendSpace* rat_MovementBlendSpace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
		UBlendSpace* squirrel_MovementBlendSpace;
};