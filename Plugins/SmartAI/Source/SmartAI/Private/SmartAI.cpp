// Copyright Epic Games, Inc. All Rights Reserved.

#include "SmartAI.h"


#define LOCTEXT_NAMESPACE "FSmartAIModule"

void FSmartAIModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	if (IsLicenseValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("插件授权有效！！！"));
	}
	else
	{
		// FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Please contact the plug author for permission!!! 2394439184@qq.com")));
		UE_LOG(LogTemp, Error, TEXT("插件授权无效"));

		FGenericPlatformMisc::RequestExit(false);
	}
}

void FSmartAIModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

// 简单的时间授权
bool FSmartAIModule::IsLicenseValid()
{
	// const FString LicenseFilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/SmartAI/YiChen.txt"));
	const FString FileContent = "2025.01.01-00.00.00";
	/*if (!FFileHelper::LoadFileToString(FileContent, *LicenseFilePath))
	{
		return false; // 许可文件不存在或无法读取
	}*/

	// 用UE的解析函数
	FDateTime ExpiryDate;
	FDateTime::Parse(FileContent, ExpiryDate);

	// UE_LOG(LogTemp, Error, TEXT("%s"), *ExpiryDate.ToString());

	const FDateTime CurrentDate = FDateTime::Now();
	// UE_LOG(LogTemp, Error, TEXT("%s"), *CurrentDate.ToString());

	// 检查当前日期是否在到期日期之前
	return CurrentDate <= ExpiryDate;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSmartAIModule, SmartAI)
