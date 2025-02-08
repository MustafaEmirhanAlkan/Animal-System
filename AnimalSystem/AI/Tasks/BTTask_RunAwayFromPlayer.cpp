#include "AnimalSystem/AI/Tasks/BTTask_RunAwayFromPlayer.h"
#include "AnimalSystem/AI/BlackboardKeys/AnimalBlackboardKeys.h"
#include "AnimalSystem/AI/Controller/AnimalAIController.h"
#include "AnimalSystem/Animation/AnimalAnimInstance.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSystem/Characters/Animals/Rabbit.h"
#include "AnimalSystem/Characters/Animals/Rat.h"
#include "AnimalSystem/Characters/Animals/Squirrel.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "NavigationSystem.h"

UBTTask_RunAwayFromPlayer::UBTTask_RunAwayFromPlayer()
{
	NodeName = "Run Away From Player";
}

EBTNodeResult::Type UBTTask_RunAwayFromPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	ABaseAnimal* Animal = Cast<ABaseAnimal>(ControlledPawn);
	if (!Animal)
	{
		return EBTNodeResult::Failed;
	}

	FVector PlayerLocation = BlackboardComp->GetValueAsVector(AnimalBlackboardKeys::target_location);
	FVector CurrentLocation = ControlledPawn->GetActorLocation();
	float DistanceToPlayer = FVector::Distance(CurrentLocation, PlayerLocation);

	bool bCanSeePlayer = BlackboardComp->GetValueAsBool(AnimalBlackboardKeys::can_see_player);

	if (DistanceToPlayer <= Animal->EscapeDistance)
	{
		FVector RunDirection = (CurrentLocation - PlayerLocation).GetSafeNormal();
		FVector RunLocation = CurrentLocation + RunDirection * 1000.0f;

		//ABaseAnimal* Animal = Cast<ABaseAnimal>(ControlledPawn);
		if (Animal && Animal->GetCharacterMovement())
		{
			Animal->GetCharacterMovement()->MaxWalkSpeed = Animal->RunSpeed;
			USkeletalMeshComponent* MeshComp = Animal->GetMesh();
			if (MeshComp)
			{
				UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
				if (AnimInstance)
				{
					UAnimalAnimInstance* AnimalAnimInstance = Cast<UAnimalAnimInstance>(AnimInstance);
					if (AnimalAnimInstance)
					{
						AnimalAnimInstance->Speed = Animal->GetVelocity().Size();
						UE_LOG(LogTemp, Log, TEXT("Updated Speed: %.2f"), AnimalAnimInstance->Speed);
					}
				}
			}
		}

		AIController->MoveToLocation(RunLocation);
		return EBTNodeResult::Succeeded;
	}

	if (bCanSeePlayer && DistanceToPlayer <= Animal->EscapeDistance)
	{
		UE_LOG(LogTemp, Log, TEXT("Animal on alert! Distance: %.2f"), DistanceToPlayer);

		//ABaseAnimal* Animal = Cast<ABaseAnimal>(ControlledPawn);
		if (Animal)
		{
			USkeletalMeshComponent* MeshComp = Animal->GetMesh();
			if (MeshComp)
			{
				UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
				if (AnimInstance)
				{
					UAnimalAnimInstance* AnimalAnimInstance = Cast<UAnimalAnimInstance>(AnimInstance);
					if (AnimalAnimInstance)
					{
						AnimalAnimInstance->PlayAlertAnimation();
					}
				}
			}
		}
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}