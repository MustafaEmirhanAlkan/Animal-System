#include "AnimalSystem/AI/Tasks/BTTask_MoveRandomLocation.h"
#include "AnimalSystem/AI/BlackboardKeys/AnimalBlackboardKeys.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AnimalSystem/Animation/AnimalAnimInstance.h"
#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

UBTTask_MoveRandomLocation::UBTTask_MoveRandomLocation()
{
	NodeName = "Move to Random Location";
	SearchRadius = 10000.0f;
}

EBTNodeResult::Type UBTTask_MoveRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (ControlledPawn)
	{
		ABaseAnimal* Animal = Cast<ABaseAnimal>(ControlledPawn);
		if (Animal && Animal->IsA(ABaseAnimal::StaticClass()))
		{
			//UE_LOG(LogTemp, Log, TEXT("Controlled Pawn is of type: %s"), *Animal->GetName());

		}
		else
		{
			//UE_LOG(LogTemp, Error, TEXT("Controlled Pawn is not of the expected Rabbit_BP type!"));
			return EBTNodeResult::Failed;
		}

		FVector Origin = ControlledPawn->GetActorLocation();
		FNavLocation RandomLocation;

		UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
		if (NavSys && NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, RandomLocation))
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(AnimalBlackboardKeys::target_location, RandomLocation.Location);
			AIController->MoveToLocation(RandomLocation.Location);
			return EBTNodeResult::Succeeded;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_MoveRandomLocation::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);

}