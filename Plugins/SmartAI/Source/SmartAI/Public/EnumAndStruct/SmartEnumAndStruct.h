// 聪明AI的结构和枚举
#pragma once
#include "CoreMinimal.h"
#include "SmartEnumAndStruct.generated.h"

// 阵型
UENUM(BlueprintType)
enum class EFormation:uint8
{
	// 矩形
	EF_Rectangle UMETA(DisplayName = "矩形"),
	// 三角形
	EF_Triangle UMETA(DisplayName = "三角形"),
	// 圆环
	EF_Donut UMETA(DisplayName = "圆环"),
	// 圆形
	EF_Round UMETA(DisplayName = "圆形")
};

// 朝向
UENUM(BlueprintType)
enum class EDirection : uint8
{
	// 默认朝向
	ED_Default UMETA(DisplayName = "默认朝向"),

	// 包围朝向
	ED_Surround UMETA(DisplayName = "包围朝向"),

	// 防御朝向
	ED_Defense UMETA(DisplayName = "防御朝向"),

	// 随机朝向
	ED_RandomDirection UMETA(DisplayName = "随机朝向"),

	// 指定朝向
	ED_WatchDirection UMETA(DisplayName = "指定朝向")
};

// 矩形
USTRUCT(BlueprintType)
struct FRectangle
{
	GENERATED_USTRUCT_BODY()

	// 横向
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="矩形")
	int32 Crosswise;

	// 纵向
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category="矩形")
	int32 Parallel;

	FRectangle(): Crosswise(4), Parallel(0)
	{
	}
};


// 三角形
USTRUCT(BlueprintType)
struct FTriangle
{
	GENERATED_USTRUCT_BODY()

	// 另一种
	UPROPERTY(BlueprintReadWrite, EditAnywhere, DisplayName = "Another", Category="三角形")
	uint8 bAnother : 1;

	// 第一层数量
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="三角形")
	int32 GfNumber; //Ground Floor Number

	// 总层数
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="三角形")
	int32 AmountTier;

	// 初始化
	FTriangle(): bAnother(false), GfNumber(1), AmountTier(1)
	{
	}
};

// 圆环
USTRUCT(BlueprintType)
struct FDonut
{
	GENERATED_USTRUCT_BODY()

	// 每层数量
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0), Category = "圆环")
	int32 NumberOfLayer; // A number of layer(每层数量)

	// 总层数
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "圆环")
	int32 Layer;

	// 角度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0.0f, ClampMax = 360.f), Category = "圆环")
	float Angle;

	// 错开角度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0.0f, ClampMax = 360.f), Category = "圆环")
	float StaggerAngle;

	// 圆半径
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "圆环")
	float CircleRadius;

	// 初始化
	FDonut()
	{
		NumberOfLayer = 5;
		Layer = 1;
		Angle = 360.f;
		StaggerAngle = 0.f;
		CircleRadius = 200.f;
	}
};

// 圆形
USTRUCT(BlueprintType)
struct FRound
{
	GENERATED_USTRUCT_BODY()

	// 圆半径
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "圆形")
	float CircleRadius;

	// 层数
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "圆形")
	int32 Layer;

	FRound(): CircleRadius(100.f), Layer(1)
	{
	}
};
