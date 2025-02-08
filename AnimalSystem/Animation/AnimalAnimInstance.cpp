#include "AnimalAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "Kismet/KismetMathLibrary.h"

UAnimalAnimInstance::UAnimalAnimInstance()
{
	Speed_Rabbit = 0.0f;
	Speed_Rat = 0.0f;
	Speed_Squirrel = 0.0f;
	bIsInAir = false;
	pawnOwner = nullptr;
	PlayIdleLoop = false;
}

void UAnimalAnimInstance::NativeInitializeAnimation()
{
	pawnOwner = TryGetPawnOwner();
}

void UAnimalAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (bIsDead)
	{
		return;
	}

	if (pawnOwner && !bIsDead)
	{
		FVector Velocity = pawnOwner->GetVelocity();
		Velocity.Z = 0;

		Speed_Rabbit = Velocity.Size();
		Speed_Rat = Velocity.Size();
		Speed_Squirrel = Velocity.Size();

		if (Velocity.Size() <= 10.0f)
		{
			PlayIdleLoop = true;
			PlayIdleAnimation();
		}
		else if (Velocity.Size() > 10.0f && Velocity.Size() <= 300.0f)
		{
			PlayIdleLoop = false;
			PlayWalkAnimation();
		}
		else if (Velocity.Size() > 300.0f)
		{
			PlayIdleLoop = false;
			PlayRunAnimation();
		}

		ACharacter* CharacterOwner = Cast<ACharacter>(pawnOwner);
		if (CharacterOwner)
		{
			bIsInAir = CharacterOwner->GetMovementComponent()->IsFalling();
		}
	}
}

void UAnimalAnimInstance::UpdateAnimationStates()
{
	UpdateAnimationProperties(0.0f);
}

void UAnimalAnimInstance::PlayDeathAnimation()
{
	bIsDead = true;
	if (!Montage_IsPlaying(DeathMontage))
	{
		Montage_Play(DeathMontage);
		UE_LOG(LogTemp, Warning, TEXT("Death animation is playing."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Death animation was already playing."));
	}
}
void UAnimalAnimInstance::PlayIdleAnimation()
{
	if (!Montage_IsPlaying(IdleMontage))
	{
		Montage_Play(IdleMontage);
	}
}
void UAnimalAnimInstance::PlayRunAnimation()
{
	if (!Montage_IsPlaying(RunMontage))
	{
		Montage_Play(RunMontage);
	}
}
void UAnimalAnimInstance::PlayWalkAnimation()
{
	if (!Montage_IsPlaying(WalkMontage))
	{
		Montage_Play(WalkMontage);
	}
}
void UAnimalAnimInstance::PlayAlertAnimation()
{
	if (!Montage_IsPlaying(AlertMontage))
	{
		Montage_Play(AlertMontage);
	}
}