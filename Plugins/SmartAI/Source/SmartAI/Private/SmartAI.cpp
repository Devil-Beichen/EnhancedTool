// Copyright Epic Games, Inc. All Rights Reserved.

#include "SmartAI.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Logging/LogMacros.h"


#define LOCTEXT_NAMESPACE "FSmartAIModule"

// 许可证到期日期
static FString LicenceTime = "2025.01.01-00.00.00";

void FSmartAIModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	CheckLicenseValidity();
}

void FSmartAIModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FSmartAIModule::CheckLicenseValidity()
{
	// 创建一个HTTP请求对象 这里使用的是UE的HTTP模块
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	// 设置请求的URL。这个URL指向一个可以返回当前UTC时间的API
	Request->SetURL(TEXT("https://worldtimeapi.org/api/timezone/Etc/UTC"));

	// 设置请求的方法为GET，表示这是一个获取数据的请求。
	Request->SetVerb(TEXT("GET"));

	// 绑定一个回调函数，这个函数会在HTTP请求完成时被调用。
	// BindRaw需要两个参数，第一个是要绑定的对象，第二个是成员函数指针。
	// 当请求完成时，无论成功还是失败，OnTimeResponseReceived都会被调用。
	Request->OnProcessRequestComplete().BindRaw(this, &FSmartAIModule::OnTimeResponseReceived);

	// 发送请求。这个调用是异步的，意味着它会立即返回，而请求的结果会在稍后通过回调函数返回。
	Request->ProcessRequest();
}

void FSmartAIModule::OnTimeResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		// 解析响应中的时间
		TSharedPtr<FJsonObject> JsonObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			const FString DateTimeString = JsonObject->GetStringField("datetime"); // 根据API的响应格式获取时间字符串
			FDateTime NetworkTime;
			FDateTime::ParseIso8601(*DateTimeString, NetworkTime);

			// 比较网络时间和许可证到期时间
			Authorization(IsLicenseValid(NetworkTime));
		}
	}
	else
	{
		// 重新请求
		if (CurrentRetryCount < MaxRetryCount)
		{
			CurrentRetryCount++;
			UE_LOG(LogTemp, Warning, TEXT("请求失败，正在重试...（%d/%d）"), CurrentRetryCount, MaxRetryCount);
			CheckLicenseValidity();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("无法获取网络时间"));
			Authorization(IsLicenseValid());
		}
	}
}

void FSmartAIModule::Authorization(bool bValid)
{
	// 判断授权
	if (bValid)
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

// 检查许可证是否有效
bool FSmartAIModule::IsLicenseValid(FDateTime CurrentDate)
{
	// 定义一个FDateTime变量来存储解析后的到期日期
	FDateTime ExpiryDate;

	// 将存储在LicenceTime字符串中的到期日期解析到ExpiryDate变量中
	FDateTime::Parse(LicenceTime, ExpiryDate);

	// 如果当前日期小于或等于到期日期，则返回true（表示许可证有效）
	// 否则返回false（表示许可证无效）
	return CurrentDate <= ExpiryDate;
}

// 简单的时间授权  - 被废弃
/*bool FSmartAIModule::LocalLicenseValid()
{
	// const FString LicenseFilePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Plugins/SmartAI/YiChen.txt"));
	/*if (!FFileHelper::LoadFileToString(FileContent, *LicenseFilePath))
	{
		return false; // 许可文件不存在或无法读取
	}#1#

	// 用UE的解析函数
	FDateTime ExpiryDate;
	FDateTime::Parse(LicenceTime, ExpiryDate);

	const FDateTime CurrentDate = FDateTime::UtcNow(); // FDateTime::Now()

	// 检查当前日期是否在到期日期之前
	return CurrentDate <= ExpiryDate;
}*/

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSmartAIModule, SmartAI)
