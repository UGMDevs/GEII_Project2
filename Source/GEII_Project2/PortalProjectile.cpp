// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalProjectile.h"
#include "Net/UnrealNetwork.h"

APortalProjectile::APortalProjectile()
{
	bReplicates = true;
	DamageType = nullptr;
	Damage = 0.f;
	DamageRadius = 0.f;
}

void APortalProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APortalProjectile, WeaponThatShotMe);
}

void APortalProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void APortalProjectile::Destroyed()
{

}

void APortalProjectile::SetWeaponThatShot(UTP_PortalGun* Weapon)
{
	WeaponThatShotMe = Weapon;
}
