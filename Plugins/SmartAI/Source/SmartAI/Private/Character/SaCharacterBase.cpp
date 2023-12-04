// 作者逸辰，有问题请联系2394439184@qq.com

#include "Character/SaCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASaCharacterBase::ASaCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

#pragma region Collision Correlation // 碰撞相关

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 只用于查询
	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn); // 碰撞枚举为Pawn
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore); // 忽略所有碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 与世界静态碰撞发生碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // 与世界动态碰撞发生重叠

	GetMesh()->BodyInstance.bNotifyRigidBodyCollision = true; // 可以触发碰撞
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); // 只用于查询
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody); // 物理形体
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore); // // 忽略所有碰撞
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // 与世界静态碰撞发生碰撞
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block); // 与世界动态碰撞发生碰撞

#pragma endregion

#pragma region	MoveComp Correlation // 移动组件相关
	GetCharacterMovement()->GravityScale = 1.5f; // 设置重力
	GetCharacterMovement()->bOrientRotationToMovement = true; // 设置旋转率
#pragma endregion
}

void ASaCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ASaCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
