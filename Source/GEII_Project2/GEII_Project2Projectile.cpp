// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEII_Project2Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Portal.h"

AGEII_Project2Projectile::AGEII_Project2Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	if(GetLocalRole() == ROLE_Authority)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &AGEII_Project2Projectile::OnHit);
	}

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Definition for the Mesh that will serve as your visual representation.
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	DefaultMesh(TEXT("/Game/FPWeapon/Mesh/FirstPersonProjectileMesh.FirstPersonProjectileMesh"));

	StaticMesh =CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	//Set the Static Mesh and its position/scale if you successfully found a mesh asset to use.
	if (DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeScale3D(FVector(0.03f, 0.03f, 0.03f));
	}

	// Definition for the particle effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem>
	DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));

	if(DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	
	// Replication
	bReplicates = true;

	// Damage
	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;
}

void AGEII_Project2Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigator()->Controller, this, DamageType);
		Destroy();
	}
	
}

void AGEII_Project2Projectile::Destroyed()
{
	FVector spawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(
		this, 
		ExplosionEffect, 
		spawnLocation, 
		FRotator::ZeroRotator, 
		true, 
		EPSCPoolMethod::AutoRelease
	);
}
