#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SaAIController.generated.h"

/**
 * 聪明的AI控制器基类
 */

UCLASS()
class SMARTAI_API ASaAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASaAIController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
