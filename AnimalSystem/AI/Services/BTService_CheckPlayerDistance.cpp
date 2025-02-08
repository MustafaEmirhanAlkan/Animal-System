#include "AnimalSystem/AI/Services/BTService_CheckPlayerDistance.h"
#include "AnimalSystem/AI/BlackboardKeys/AnimalBlackboardKeys.h"
#include "AnimalSystem/AI/Controller/AnimalAIController.h"
#include "AnimalSystem/Characters/Animals/BaseAnimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTService_CheckPlayerDistance::UBTService_CheckPlayerDistance()
{
	NodeName = TEXT("Check Player Distance");
}

void UBTService_CheckPlayerDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!PlayerCharacter)
	{
		return;
	}

	ABaseAnimal* ControlledAnimal = Cast<ABaseAnimal>(ControlledPawn);
	if (!ControlledAnimal) 
	{
		return;
	}

	float DistanceToPlayer = FVector::Distance(PlayerCharacter->GetActorLocation(), ControlledPawn->GetActorLocation());
	float AnimalSightRange = ControlledAnimal->AlertDistance;
	float AnimalEscapeDistance = ControlledAnimal->EscapeDistance;

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(AnimalBlackboardKeys::target_location, PlayerCharacter->GetActorLocation());
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::can_see_player, DistanceToPlayer <= SightRange);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(AnimalBlackboardKeys::distance_to_enemy, DistanceToPlayer);

	//UE_LOG(LogTemp, Log, TEXT("Rabbit distance to player: %.2f"), DistanceToPlayer);
}