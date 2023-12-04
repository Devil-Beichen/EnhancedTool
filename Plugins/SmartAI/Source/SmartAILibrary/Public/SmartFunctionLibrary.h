// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SmartFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SMARTAILIBRARY_API USmartFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/** 延迟
	 * @brief
	 * @param WorldContextObject	指定式赋值
	 * @param FunctionName			函数的名称
	 * @param DelayTime				延迟的时间
	 */
	static void YcDelay(const TWeakObjectPtr<UObject> WorldContextObject, const FName FunctionName, const float DelayTime = 0.2f);
};
