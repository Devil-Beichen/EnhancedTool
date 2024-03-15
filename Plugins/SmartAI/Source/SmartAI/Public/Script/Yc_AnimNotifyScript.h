// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Yc_AnimNotifyScript.generated.h"

/**
 * 动画通知脚本
 */
UCLASS()
class SMARTAI_API UYc_AnimNotifyScript : public UAnimNotify
{
	GENERATED_BODY()

	// 需要调用的函数名称
	UPROPERTY(EditAnywhere, Category = "名称相关")
	FName FunctionName;

	// 反射函数的名字
	virtual FString GetNotifyName_Implementation() const override;

	// 触发时调用通知
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
