// 作者逸辰，有问题请联系2394439184@qq.com

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SaCharacterBase.generated.h"

/**
 *	聪明的AI基类
 */

UCLASS()
class SMARTAI_API ASaCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASaCharacterBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
