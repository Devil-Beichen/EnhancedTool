// Fill out your copyright notice in the Description page of Project Settings.


#include "FormationBase.h"

#include "Components/ArrowComponent.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "SmartAI/Public/Character/SaCharacterBase.h"


AFormationBase::AFormationBase():
	bGameInHidden(false),
	bAlignSurface(false),
	bCanRandomSize(false),
	bRenderCustomDepthPass(false)

{
	PrimaryActorTick.bCanEverTick = false;
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
	InstancedStaticMesh->SetHiddenInGame(bGameInHidden);
	InstancedStaticMesh->SetLightingChannels(LightingChannels.bChannel0, LightingChannels.bChannel1, LightingChannels.bChannel2);
	InstancedStaticMesh->SetRenderCustomDepth(bRenderCustomDepthPass);
	InstancedStaticMesh->SetCustomDepthStencilValue(CustomDepthStencilValue);
	InstancedStaticMesh->SetScalarParameterValueOnMaterials("ColorOffset", ColorOffset);
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
		TampRotator = TempTransform.InverseTransformRotation(Arrow->GetComponentRotation().Quaternion()).Rotator();
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
	case EFormation::EF_Triangle:
		FunTriangle();
		break;
	case EFormation::EF_Donut:
		FunDonut();
		break;
	case EFormation::EF_Round:
		FunRound();
		break;
	default:
		break;
	}
}

void AFormationBase::FunRectangle_Implementation()
{
	// 检查网格体是否有效且数量不为零
	if (!InstancedStaticMesh->GetStaticMesh() || Sun == 0) return;

	// 用于存储每个实例的临时变换信息
	FTransform TempRelativeTransform;

	// 计算纵向的数量，确保至少有一个单位
	Rectangle.Parallel = (Sun + Rectangle.Crosswise - 1) / Rectangle.Crosswise;

	// 计算矩阵横向和纵向的一半长度，用于后续计算实例的位置
	const float RectCrosswiseHalf = (Rectangle.Crosswise - 1) * (Spacing.X / 2.f);
	const float RectParallelHalf = (Rectangle.Parallel - 1) * (Spacing.Y / 2.f);

	// 遍历每个实例
	for (int32 i = 0; i < Sun; i++)
	{
		// 计算当前实例在矩阵中的X和Y位置
		const float X = (((i % Rectangle.Crosswise) * Spacing.X) - RectCrosswiseHalf) * -1;
		const float Y = ((i / Rectangle.Crosswise) * Spacing.Y) - RectParallelHalf;

		// 设置实例的位置，包括调用GetFinalPosition来考虑表面对齐和随机位置偏移
		TempRelativeTransform.SetLocation(GetFinalPosition(FVector(X, Y, 0.f) + AddRandomLocation()));
		// 设置实例的旋转，根据GetOrientation函数的结果
		TempRelativeTransform.SetRotation(GetOrientation(TempRelativeTransform.GetLocation()).Quaternion());
		// 设置实例的缩放，GetThisSize3D函数可能根据需要提供不同的缩放值
		TempRelativeTransform.SetScale3D(GetThisSize3D());
		// 将配置好的实例添加到InstancedStaticMesh组件中
		InstancedStaticMesh->AddInstance(TempRelativeTransform);
	}
}

void AFormationBase::FunTriangle_Implementation()
{
	// 如果实例的数量为零，或者 InstancedStaticMesh 组件无效，则返回
	if (Sun == 0 || !InstancedStaticMesh->GetStaticMesh()) return;

	// 初始化三角形阵型的层数
	Triangle.AmountTier = 1;

	// 初始化已添加的实例的计数器
	int32 TempSun = 0;

	// 初始化用于存储每个实例的临时变换
	FTransform TempRelativeTransform;

	// 第一层临时的数量
	const int32 LastGfNumber = Triangle.GfNumber - 1;

	// Y一半的距离				
	const float HalfSpacingY = Spacing.Y / 2.f;

	// 如果是另一种形式的阵型
	if (Triangle.bAnother)
	{
		// 计算需要的层数来达到所需的实例数量
		while (((Triangle.AmountTier * Triangle.AmountTier * 2 + 2) / 2 - 1) + (LastGfNumber * Triangle.AmountTier) < Sun)
		{
			Triangle.AmountTier++;
		}
		// 循环处理每一层
		for (int32 i = 1; i <= Triangle.AmountTier; i++)
		{
			const int LastLayer = i - 1;
			// 循环处理每一层中的单位
			for (int32 j = 1; j <= i * 2 + Triangle.GfNumber - 2; j++)
			{
				// 如果尚未达到所需的实例数量
				if (TempSun < Sun)
				{
					TempSun++;
					// 计算当前单位的位置
					const float x = (i - 1) * Spacing.X * -1;
					const float y = (j - 1) * Spacing.Y - LastLayer * Spacing.Y - LastGfNumber * HalfSpacingY;
					TempRelativeTransform.SetLocation(GetFinalPosition(FVector(x, y, 0.f) + AddRandomLocation()));
					// 设置当前单位的旋转
					TempRelativeTransform.SetRotation(GetOrientation(TempRelativeTransform.GetLocation()).Quaternion());
					// 设置当前单位的缩放
					TempRelativeTransform.SetScale3D(GetThisSize3D());
					// 将当前单位添加到 InstancedStaticMesh 组件
					InstancedStaticMesh->AddInstance(TempRelativeTransform);
				}
				else
				{
					// 如果已达到所需的实例数量，则中断循环
					break;
				}
			}
		}
	}
	else
	{
		while (((Triangle.AmountTier * Triangle.AmountTier + Triangle.AmountTier) / 2) + (LastGfNumber * Triangle.AmountTier) < Sun)
		{
			Triangle.AmountTier++;
		}
		for (int32 i = 1; i <= Triangle.AmountTier; i++)
		{
			const int LastLayer = i - 1;
			for (int32 j = 1; j <= i + LastGfNumber; j++)
			{
				// 如果尚未达到所需的实例数量
				if (TempSun < Sun)
				{
					TempSun++;
					// 计算当前单位的位置
					const float x = (i - 1) * Spacing.X * -1;
					const float y = (j - 1) * Spacing.Y - LastGfNumber * HalfSpacingY - LastLayer * HalfSpacingY;
					TempRelativeTransform.SetLocation(GetFinalPosition(FVector(x, y, 0.f) + AddRandomLocation()));
					// 设置当前单位的旋转
					TempRelativeTransform.SetRotation(GetOrientation(TempRelativeTransform.GetLocation()).Quaternion());
					// 设置当前单位的缩放
					TempRelativeTransform.SetScale3D(GetThisSize3D());
					// 将当前单位添加到 InstancedStaticMesh 组件
					InstancedStaticMesh->AddInstance(TempRelativeTransform);
				}
				else
					break;
			}
		}
	}
}

void AFormationBase::FunDonut_Implementation()
{
	if (Sun == 0 || Donut.NumberOfLayer == 0 || !InstancedStaticMesh->GetStaticMesh()) return;

	int32 TempSun = 0;
	FTransform TempRelativeTransform;

	Donut.Layer = 0;
	while (Donut.NumberOfLayer * Donut.Layer < Sun) { Donut.Layer++; }

	for (int32 i = 0; i < Donut.Layer; i++)
	{
		const float TempInterval = i * Interval + Donut.CircleRadius;

		for (int32 j = 0; j < Donut.NumberOfLayer; j++)
		{
			if (TempSun >= Sun)
			{
				break;
			}
			TempSun++;

			const float StaggerAngle = j * FinalAngle() / Donut.NumberOfLayer + i * Donut.StaggerAngle;
			const float AngleRadians = RADIAN * StaggerAngle;
			const float x = FMath::Cos(AngleRadians) * TempInterval;
			const float y = FMath::Sin(AngleRadians) * TempInterval;

			TempRelativeTransform.SetLocation(GetFinalPosition(AddRandomLocation() + FVector(x, y, 0.f)));
			TempRelativeTransform.SetRotation(GetOrientation(TempRelativeTransform.GetLocation()).Quaternion());
			TempRelativeTransform.SetScale3D(GetThisSize3D());
			InstancedStaticMesh->AddInstance(TempRelativeTransform);
		}
	}
}

void AFormationBase::FunRound_Implementation()
{
	if (Sun == 0 || Interval <= 0 || !InstancedStaticMesh->GetStaticMesh()) return;
	// 临时生成的数量
	int32 TempSun = 0;
	Round.Layer = 0;
	// 所以层数的数量
	int32 SoNumberLayers = 0;

	FTransform TempRelativeTransform;

	while (SoNumberLayers <= Sun - 1)
	{
		SoNumberLayers = GetFrontNum(Round.CircleRadius, Round.Layer) + SoNumberLayers;
		if (SoNumberLayers < Sun)
		{
			Round.Layer++;
		}
		else
			break;
	}

	for (int i = 1; i <= Round.Layer; i++)
	{
		// 单层数量
		const int32 NumberLayers = GetFrontNum(Round.CircleRadius, i);

		// 临时的距离
		const float LastInterval = Round.CircleRadius * i;

		for (int j = 0; j <= NumberLayers - 1; j++)
		{
			// 临时的角度
			const float TempAngle = 360.f / NumberLayers * j;
			// 角弧度
			const float AngleRadians = RADIAN * TempAngle;
			if (TempSun <= Sun - 1)
			{
				const float x = FMath::Cos(AngleRadians) * LastInterval;
				const float y = FMath::Sin(AngleRadians) * LastInterval;

				TempRelativeTransform.SetLocation(GetFinalPosition(AddRandomLocation() + FVector(x, y, 0.f)));
				TempRelativeTransform.SetRotation(GetOrientation(TempRelativeTransform.GetLocation()).Quaternion());
				TempRelativeTransform.SetScale3D(GetThisSize3D());
				InstancedStaticMesh->AddInstance(TempRelativeTransform);
				TempSun++;
			}
		}
	}
}
