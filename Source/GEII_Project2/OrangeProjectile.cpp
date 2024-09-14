// Fill out your copyright notice in the Description page of Project Settings.


#include "OrangeProjectile.h"

void AOrangeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	WeaponThatShotMe->SpawnOrangePortal();
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
