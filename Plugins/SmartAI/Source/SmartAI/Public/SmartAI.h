// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Modules/ModuleManager.h"

class FSmartAIModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// 网络检查许可证是否有效
	void CheckLicenseValidity();

	/*// 本地许可证是否有效 已经被废弃
	static bool LocalLicenseValid();*/

	// 许可证是否有效
	static bool IsLicenseValid(FDateTime CurrentDate = FDateTime::UtcNow());

	/**
	 * @brief						及时收到回应
	 * @param Request				请求
	 * @param Response				响应
	 * @param bWasSuccessful		是成功的
	 */
	void OnTimeResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// 授权
	static void Authorization(bool bValid);

private:
	// 网络重试次数
	int32 CurrentRetryCount = 0;

	// 最大次数
	const int32 MaxRetryCount = 9;
};
