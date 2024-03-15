// Fill out your copyright notice in the Description page of Project Settings.


#include "Script/Yc_AnimNotifyScript.h"
#include "Components/SkeletalMeshComponent.h"

FString UYc_AnimNotifyScript::GetNotifyName_Implementation() const
{
	return FunctionName.ToString();
}

void UYc_AnimNotifyScript::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;
	if (AActor* TempActor = MeshComp->GetOwner())
	{
		if (UFunction* TempFunction = TempActor->FindFunction(FunctionName))
		{
			TempActor->ProcessEvent(TempFunction, nullptr);
		}
	}
}
