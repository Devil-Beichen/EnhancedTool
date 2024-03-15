// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnumAndStruct/SmartEnumAndStruct.h"
#include "FormationBase.generated.h"

// 弧度
#define	RADIAN	(UE_DOUBLE_PI / (180.0))

// 角度
#define ANGLE	((180.0)/ UE_DOUBLE_PI)

// 声明 动态 多播 委托 一个参数 无返回值 动态多播生成完成
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGenerateComplete, class AFormationBase*, Formation);

class UArrowComponent;
class UHierarchicalInstancedStaticMeshComponent;
class ASaCharacterBase;

/**
 *	阵型Actor基类
 */
UCLASS()
class SMARTAITOOL_API AFormationBase : public AActor
{
	GENERATED_BODY()

public:
	AFormationBase();

protected:
	virtual void BeginPlay() override;

	// 初始化属性时调用
	virtual void PostInitProperties() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	/*#if WITH_EDITOR
		// 在编辑器中修改属性的时候调用
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif*/

	// 箭头
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="组件")
	TObjectPtr<UArrowComponent> Arrow;

	// 实例化静态网
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="组件")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> InstancedStaticMesh;

	// 生成阵型
	UFUNCTION()
	void FormationGeneration();

	// 生成完成
	UPROPERTY(BlueprintAssignable, Category="逸辰自定义代理")
	FGenerateComplete GenerateComplete;

	/** 广播生成完成
	 * @brief 
	 * @param CallFormation 通知的Formation
	 */
	UFUNCTION(Category="逸辰自定义函数")
	void CallGenerateComplete(AFormationBase* CallFormation) const;

public:
	virtual void Tick(float DeltaTime) override;

#pragma region 共用属性

	// 表面对齐
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName="表面对齐", Category="基础")
	uint8 bAlignSurface : 1;

	// 相差距离(生成位置高 当前位置+相差距离/2，生成位置低 当前位置+相差距离/2 )
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=( EditCondition = "bAlignSurface == true", EditConditionHides), Category="基础")
	float ApartFrom = 500.f;

	// 碰撞类型
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=( EditCondition = "bAlignSurface == true", EditConditionHides), Category="基础")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	/** 获得最终位置
	 * @brief 
	 * @param ApartFromLocation		局部坐标
	 * @return						返回一个修改的局部坐标
	 */
	UFUNCTION(BlueprintPure, Category="逸辰自定义函数")
	FVector GetFinalPosition(FVector ApartFromLocation) const;

	// 阵型
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="基础|阵型相关")
	EFormation Formation = EFormation::EF_Rectangle;

	// 生成朝向
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="基础|阵型相关")
	EDirection Direction = EDirection::ED_Default;

	// 朝向Actor
	UPROPERTY(EditAnywhere, meta=(EditCondition = "Direction == EDirection::ED_WatchDirection", EditConditionHides), Category="基础|阵型相关")
	TObjectPtr<AActor> LookAtActor;

	// 总数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0), Category="基础|阵型相关")
	int32 Sun = 16;

	// 可以随机大小
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName= "CanRandomSize", Category="基础|阵型相关")
	uint8 bCanRandomSize : 1;

	// Actor大小
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0.f, EditCondition = "bCanRandomSize == false", EditConditionHides), Category="基础|阵型相关")
	float ActorSize = 1.f;

	// Actor最小值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0.f, EditCondition = "bCanRandomSize == true", EditConditionHides), Category="基础|阵型相关")
	float MinActorSize = 1.f;

	// Actor最大值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = 0.f, EditCondition = "bCanRandomSize == true", EditConditionHides), Category="基础|阵型相关")
	float MaxActorSize = 1.f;

	// 获得自己的3D大小
	UFUNCTION(BlueprintPure, Category="逸辰自定义函数")
	FORCEINLINE FVector GetThisSize3D() const
	{
		return bCanRandomSize ? FVector(MinActorSize + (MaxActorSize - MinActorSize) * Seed.FRand()) : FVector(ActorSize);
	};

	// 间距
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition = "Formation != EFormation::EF_Round && Formation != EFormation::EF_Donut", EditConditionHides), Category="基础|阵型相关")
	FVector2D Spacing{150.f, 150.f};

	// 间距
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition = "Formation == EFormation::EF_Round || Formation == EFormation::EF_Donut", EditConditionHides), Category="基础|阵型相关")
	float Interval = 200.f;

	// 随机加的距离半径
	UPROPERTY(EditAnywhere, Category="基础|阵型相关")
	float RandomDistance;

	// 随机种子
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="基础|阵型相关")
	FRandomStream Seed;

	// 添加一个随机位置
	UFUNCTION(BlueprintPure, Category="逸辰自定义函数")
	FORCEINLINE FVector AddRandomLocation() const
	{
		// 随机角度
		const float RAngle = 360.f * Seed.FRand();
		// 随机距离
		const float RDistance = Seed.FRandRange(-RandomDistance, RandomDistance) * GetThisSize3D().X;
		// 角度换算成弧度
		const float AngleToRadian = RADIAN * RAngle;
		const float x = FMath::Cos(AngleToRadian) * RDistance;
		const float y = FMath::Sin(AngleToRadian) * RDistance;
		return FVector(x, y, 0.0f);
	}

	/**	获取朝向
	 * @brief 
	 * @param LastTempRelativeLocation		最后临时的位置
	 * @return								返回一个Rotation
	 */
	UFUNCTION(BlueprintPure, Category="逸辰自定义函数")
	FRotator GetOrientation(const FVector& LastTempRelativeLocation) const;

	// 需要生成的角色
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="基础|生成相关")
	TArray<TSubclassOf<ASaCharacterBase>> Characters;

	// 灯光通道
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="基础|渲染相关")
	FLightingChannels LightingChannels;

	// 渲染自定义深度通道
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(InlineEditConditionToggle = true), Category="基础|渲染相关")
	uint8 bRenderCustomDepthPass : 1;

	// 自定义深度模板值
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition = "bRenderCustomDepthPass"), Category="基础|渲染相关")
	uint8 CustomDepthStencilValue = 0;

	/** 设置灯光和渲染通道
	 * @brief 
	 * @param Character				设置的Character
	 * @param Channels				灯光通道
	 * @param CustomDepthPass		是否开启自定义模具
	 * @param DepthStencilValue		自定义通道ID(0 - 255)
	 */
	UFUNCTION(BlueprintCallable, Category="基础|渲染相关|函数")
	static void SetLightingRenderPasses(const ASaCharacterBase* Character, const FLightingChannels Channels, const bool CustomDepthPass, const uint8 DepthStencilValue);


#pragma endregion

#pragma region 矩形

	// 矩形参数
	UPROPERTY
	(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "Formation == EFormation::EF_Rectangle", EditConditionHides), Category="基础|阵型相关")
	FRectangle Rectangle;

	// 矩形
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "阵型函数")
	void FunRectangle();


#pragma endregion
};