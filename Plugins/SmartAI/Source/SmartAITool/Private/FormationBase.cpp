// Fill out your copyright notice in the Description page of Project Settings.


#include "FormationBase.h"

#include "Components/ArrowComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "SmartAI/Public/Character/SaCharacterBase.h"


AFormationBase::AFormationBase()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);

	InstancedStaticMesh = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("InstancedStaticMesh"));
	InstancedStaticMesh->SetupAttachment(RootComponent);
	InstancedStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InstancedStaticMesh->SetHiddenInGame(true);
	InstancedStaticMesh->SetReceivesDecals(false);
	InstancedStaticMesh->SetCastShadow(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/Script/Engine.StaticMesh'/SmartAI/StaticMesh/Counus.Counus'"));
	if (StaticMeshAsset.Succeeded())
	{
		InstancedStaticMesh->SetStaticMesh(StaticMeshAsset.Object);
	}

	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
}

void AFormationBase::BeginPlay()
{
	Super::BeginPlay();
}

void AFormationBase::PostInitProperties()
{
	Super::PostInitProperties();
}

void AFormationBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	FormationGeneration();
}

/*#if WITH_EDITOR
void AFormationBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property->GetName() == "Formation"||
		PropertyChangedEvent.Property->GetName() == "Direction"||
		PropertyChangedEvent.Property->GetName() == "Sun")
	{
		FormationGeneration();
	}
}

#endif*/

void AFormationBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AFormationBase::GetFinalPosition(FVector ApartFromLocation) const
{
	if (!bAlignSurface) return ApartFromLocation;

	FVector TempLocation = GetActorTransform().TransformPosition(ApartFromLocation);
	FVector StartLocation = TempLocation + FVector(0.f, 0.f, ApartFrom / 2.f);
	FVector EndLocation = TempLocation - FVector(0.f, 0.f, ApartFrom / 2.f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (FHitResult OutHit; GetWorld() ? GetWorld()->LineTraceSingleByObjectType(OutHit, StartLocation, EndLocation, ObjectTypes, QueryParams) : false)
	{
		return GetActorTransform().InverseTransformPosition(OutHit.ImpactPoint);
	}

	return ApartFromLocation;
}

FRotator AFormationBase::GetOrientation(const FVector& LastTempRelativeLocation) const
{
	FRotator TampRotator;
	const FTransform TempTransform = GetActorTransform();
	const FVector TempLocation = GetActorLocation();

	// 计算朝向
	switch (Direction)
	{
	case EDirection::ED_Default:
		TampRotator = TempTransform.InverseTransformRotation(GetActorRotation().Quaternion()).Rotator();
		break;
	case EDirection::ED_Surround:
		TampRotator = FRotationMatrix::MakeFromX(TempTransform.InverseTransformPosition(TempLocation) - LastTempRelativeLocation).Rotator();
		break;
	case EDirection::ED_Defense:
		TampRotator = FRotationMatrix::MakeFromX(LastTempRelativeLocation - TempTransform.InverseTransformPosition(TempLocation)).Rotator();
		break;
	case EDirection::ED_RandomDirection:
		TampRotator = FRotator(0.f, Seed.FRand() * 360.f, 0.f);
		break;
	case EDirection::ED_WatchDirection:
		if (LookAtActor)
		{
			TampRotator = FRotationMatrix::MakeFromX(TempTransform.InverseTransformPosition(LookAtActor->GetActorLocation()) - LastTempRelativeLocation).Rotator();
		}
		else
		{
			TampRotator = TempTransform.InverseTransformRotation(GetActorRotation().Quaternion()).Rotator();
		}
		break;
	}

	TampRotator = FRotator(0.f, TampRotator.Yaw, 0.f);

	return TampRotator;
}

void AFormationBase::SetLightingRenderPasses(const ASaCharacterBase* Character, const FLightingChannels Channels, const bool CustomDepthPass, const uint8 DepthStencilValue)
{
	if (Character && Character->GetMesh())
	{
		Character->GetMesh()->SetLightingChannels(Channels.bChannel0, Channels.bChannel1, Channels.bChannel2);
		Character->GetMesh()->SetRenderCustomDepth(CustomDepthPass);
		Character->GetMesh()->SetCustomDepthStencilValue(DepthStencilValue);
	}
}


void AFormationBase::CallGenerateComplete(AFormationBase* CallFormation) const
{
	if (GenerateComplete.IsBound())
		GenerateComplete.Broadcast(CallFormation);
}

void AFormationBase::FormationGeneration()
{
	InstancedStaticMesh->ClearInstances();

	switch (Formation)
	{
	case EFormation::EF_Rectangle:
		FunRectangle();
		break;
	default:
		break;
	}
}

void AFormationBase::FunRectangle_Implementation()
{
	// 网格体是否有效 同时数量不为零
	if (!InstancedStaticMesh->GetStaticMesh() || Sun == 0) return;

	// 临时的变换
	FTransform TempRelativeTransform;

	// 纵向的数量
	Rectangle.Parallel = (Sun + Rectangle.Crosswise - 1) / Rectangle.Crosswise;

	// 矩阵横向的一半
	const float RectCrosswiseHalf = (Rectangle.Crosswise - 1) * (Spacing.X / 2.f);

	// 矩阵纵向的一半
	const float RectParallelHalf = (Rectangle.Parallel - 1) * (Spacing.Y / 2.f);

	for (int32 i = 0; i < Sun; i++)
	{
		const float X = (((i % Rectangle.Crosswise) * Spacing.X) - RectCrosswiseHalf) * -1;
		const float Y = ((i / Rectangle.Crosswise) * Spacing.Y) - RectParallelHalf;

		TempRelativeTransform.SetLocation(GetFinalPosition(FVector(X, Y, 0.f) + AddRandomLocation()));
		TempRelativeTransform.SetRotation(GetOrientation(TempRelativeTransform.GetLocation()).Quaternion());
		TempRelativeTransform.SetScale3D(GetThisSize3D());
		InstancedStaticMesh->AddInstance(TempRelativeTransform);
	}
}
