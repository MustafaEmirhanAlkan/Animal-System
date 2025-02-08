#include "AnimalSystem/AI/Controller/AnimalAIController.h"
#include "AnimalSystem/AI/BlackboardKeys/AnimalBlackboardKeys.h"
#include "AnimalSystem/Characters/Animals/Rabbit.h"
#include "AnimalSystem/Characters/Animals/Rat.h"
#include "AnimalSystem/Characters/Animals/Squirrel.h"
#include "AnimalSystem/Animation/AnimalAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "UObject/ConstructorHelpers.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

AAnimalAIController::AAnimalAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	controlledPawn = nullptr;
	bIsRabbit = false;
	bIsRat = false;
	bIsSquirrel = false;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> RabbitBTAsset(TEXT("/Game/Blueprints/Animals/AI/Rabbit_Blackboard/Rabbit_BT"));
	static ConstructorHelpers::FObjectFinder<UBlackboardData> RabbitBBAsset(TEXT("/Game/Blueprints/Animals/AI/Rabbit_Blackboard/Rabbit_BB"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> RatBTAsset(TEXT("/Game/Blueprints/Animals/AI/Rat_Blackboard/Rat_BT"));
	static ConstructorHelpers::FObjectFinder<UBlackboardData> RatBBAsset(TEXT("/Game/Blueprints/Animals/AI/Rat_Blackboard/Rat_BB"));

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> SquirrelBTAsset(TEXT("/Game/Blueprints/Animals/AI/Squirrel_Blackboard/Squirrel_BT"));
	static ConstructorHelpers::FObjectFinder<UBlackboardData> SquirrelBBAsset(TEXT("/Game/Blueprints/Animals/AI/Squirrel_Blackboard/Squirrel_BB"));

	if (RabbitBTAsset.Succeeded())
	{
		RabbitBehaviorTree = RabbitBTAsset.Object;
	}
	if (RabbitBBAsset.Succeeded())
	{
		RabbitBlackboard = RabbitBBAsset.Object;
	}
	if (RatBTAsset.Succeeded())
	{
		RatBehaviorTree = RatBTAsset.Object;
	}
	if (RatBBAsset.Succeeded())
	{
		RatBlackboard = RatBBAsset.Object;
	}
	if (SquirrelBTAsset.Succeeded())
	{
		SquirrelBehaviorTree = SquirrelBTAsset.Object;
	}
	if (SquirrelBBAsset.Succeeded())
	{
		SquirrelBlackboard = SquirrelBBAsset.Object;
	}

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	if (SightConfig)
	{
		SightConfig->SightRadius = 1000.0f;
		SightConfig->LoseSightRadius = 1200.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		SightConfig->SetMaxAge(5.0f);
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		PerceptionComponent->ConfigureSense(*SightConfig);
		PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	}
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAnimalAIController::OnPerception);
}

void AAnimalAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ABaseAnimal* ControlledAnimal = Cast<ABaseAnimal>(GetPawn());
	if (ControlledAnimal)
	{
		FVector Velocity = ControlledAnimal->GetVelocity();
		if (Velocity.Size() > 0)
		{
			FRotator LookAtRotation = Velocity.Rotation();
			ControlledAnimal->SetActorRotation(LookAtRotation);
		}

		USkeletalMeshComponent* MeshComp = ControlledAnimal->AnimalMesh;
		if (MeshComp)
		{
			UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
			if (AnimInstance)
			{
				UAnimalAnimInstance* AnimalAnimInstance = Cast<UAnimalAnimInstance>(AnimInstance);
				if (AnimalAnimInstance)
				{
					AnimalAnimInstance->UpdateAnimationProperties(DeltaTime);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AnimInstance is null."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MeshComp is null."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ControlledAnimal is null"));
	}
}

void AAnimalAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ARabbit* Rabbit = Cast<ARabbit>(InPawn))
	{
		if (RabbitBlackboard && RabbitBehaviorTree)
		{
			if (UseBlackboard(RabbitBlackboard, BlackboardComponent))
			{
				RunBehaviorTree(RabbitBehaviorTree);
				UE_LOG(LogTemp, Warning, TEXT("Behavior Tree activated: %s"), *RabbitBehaviorTree->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Blackboard not started!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("RabbitBlackboard or RabbitBehaviorTree not assigned!"));
		}
		if (BlackboardComponent)
		{
			BlackboardComponent->SetValueAsBool(AnimalBlackboardKeys::is_walking, false);
			BlackboardComponent->SetValueAsBool(AnimalBlackboardKeys::is_running, false);
			BlackboardComponent->SetValueAsFloat(AnimalBlackboardKeys::health, 100.0f);
			BlackboardComponent->SetValueAsVector(AnimalBlackboardKeys::target_location, Rabbit->GetActorLocation());
		}
	}
	controlledPawn = InPawn;

	if (ARat* Rat = Cast<ARat>(InPawn))
	{
		if (RatBlackboard && RatBehaviorTree)
		{
			if (UseBlackboard(RatBlackboard, BlackboardComponent))
			{
				RunBehaviorTree(RatBehaviorTree);
				UE_LOG(LogTemp, Warning, TEXT("Behavior Tree activated: %s"), *RatBehaviorTree->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Blackboard not started!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("RatBlackboard or RatBehaviorTree not assigned!"));
		}
		if (BlackboardComponent)
		{
			BlackboardComponent->SetValueAsBool(AnimalBlackboardKeys::is_walking, false);
			BlackboardComponent->SetValueAsBool(AnimalBlackboardKeys::is_running, false);
			BlackboardComponent->SetValueAsFloat(AnimalBlackboardKeys::health, 100.0f);
			BlackboardComponent->SetValueAsVector(AnimalBlackboardKeys::target_location, Rat->GetActorLocation());
		}
	}
	controlledPawn = InPawn;

	if (ASquirrel* Squirrel = Cast<ASquirrel>(InPawn))
	{
		if (SquirrelBlackboard && SquirrelBehaviorTree)
		{
			if (UseBlackboard(SquirrelBlackboard, BlackboardComponent))
			{
				RunBehaviorTree(SquirrelBehaviorTree);
				UE_LOG(LogTemp, Warning, TEXT("Behavior Tree activated: %s"), *SquirrelBehaviorTree->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Blackboard not started!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SquirrelBlackboard or SquirrelBehaviorTree not assigned!"));
		}
		if (BlackboardComponent)
		{
			BlackboardComponent->SetValueAsBool(AnimalBlackboardKeys::is_walking, false);
			BlackboardComponent->SetValueAsBool(AnimalBlackboardKeys::is_running, false);
			BlackboardComponent->SetValueAsFloat(AnimalBlackboardKeys::health, 100.0f);
			BlackboardComponent->SetValueAsVector(AnimalBlackboardKeys::target_location, Squirrel->GetActorLocation());
		}
	}
	controlledPawn = InPawn;
}

void AAnimalAIController::InitializeAI(UBehaviorTree* BehaviorTree, UBlackboardData* BlackboardData)
{
	AnimalBehaviorTree = BehaviorTree;
	AnimalBlackboard = BlackboardData;

	if (AnimalBehaviorTree)
	{
		RunBehaviorTree(AnimalBehaviorTree);
		if (Blackboard)
		{
			UseBlackboard(AnimalBlackboard, Blackboard);
		}
	}
}

void AAnimalAIController::SetCanSeePlayer(bool SeePlayer, UObject* Player)
{
	if (GetBlackboardComponent())
	{
		GetBlackboardComponent()->SetValueAsBool(AnimalBlackboardKeys::can_see_player, SeePlayer);

		if (SeePlayer)
		{
			GetBlackboardComponent()->SetValueAsObject(AnimalBlackboardKeys::player_target, Player);

			AActor* PlayerActor = Cast<AActor>(Player);
			if (PlayerActor)
			{
				GetBlackboardComponent()->SetValueAsVector(AnimalBlackboardKeys::target_location, PlayerActor->GetActorLocation());
			}
		}
		else
		{
			ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
			if (ControlledCharacter && ControlledCharacter->GetMesh())
			{
				if (UAnimInstance* AnimInstance = ControlledCharacter->GetMesh()->GetAnimInstance())
				{
					AnimInstance->StopAllMontages(0);
				}
			}
		}
	}
}

void AAnimalAIController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor == nullptr)
	{
		return;
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			SetCanSeePlayer(true, Actor);
			//UE_LOG(LogTemp, Log, TEXT("Rabbit saw player, can_see_player true."));

			FVector EnemyLocation = GetPawn()->GetActorLocation();
			FVector PlayerLocation = Actor->GetActorLocation();
			float DistanceToPlayer = FVector::Distance(EnemyLocation, PlayerLocation);

			if (DistanceToPlayer > 800.0f && DistanceToPlayer < 1000.0f)
			{
				//UE_LOG(LogTemp, Log, TEXT("Player is within 800-1000 units, playing alarm animation."));
			}
			//UE_LOG(LogTemp, Log, TEXT("Player is within 800 units, starting flee behavior."));
		}
		else
		{
			SetCanSeePlayer(false, Actor);
			//UE_LOG(LogTemp, Log, TEXT("Rabbit not seeing player, can_see_player false."));
		}
	}
}