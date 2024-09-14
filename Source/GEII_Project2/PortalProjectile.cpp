// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalProjectile.h"

APortalProjectile::APortalProjectile()
{
	DamageType = nullptr;
	Damage = 0.f;
	DamageRadius = 0.f;
}

void APortalProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void APortalProjectile::SetWeaponThatShot(UTP_PortalGun* Weapon)
{
	WeaponThatShotMe = Weapon;
}
