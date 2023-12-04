// Fill out your copyright notice in the Description page of Project Settings.


#include "SmartFunctionLibrary.h"
#include "DelayAction.h"
#include "Engine/Engine.h"
#include "Engine/LatentActionManager.h"

void USmartFunctionLibrary::YcDelay(const TWeakObjectPtr<UObject> WorldContextObject, const FName FunctionName, const float DelayTime)
{
	// 输入的世界上下文对象是否有效
	if (WorldContextObject.IsValid())
	{
		// 使用WorldContextObject获取当前World
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject.Get(), EGetWorldErrorMode::LogAndReturnNull))
		{
			// 创建一个空的LatentInfo
			FLatentActionInfo LatentInfo;
			// 将WorldContextObject作为回调对象，意味着我们回调的函数是这个对象的成员函数
			LatentInfo.CallbackTarget = WorldContextObject.Get();
			// ExecutionFunction是一个FName类型
			LatentInfo.ExecutionFunction = FunctionName;
			LatentInfo.Linkage = 0;
			// 取一个随机数
			LatentInfo.UUID = FMath::Rand();
			// 使用World获取异步操作管理
			FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
			// 将我们的异步请求加入到执行列表
			LatentActionManager.AddNewAction(WorldContextObject.Get(), LatentInfo.UUID, new FDelayAction(DelayTime, LatentInfo));
		}
	}
}
