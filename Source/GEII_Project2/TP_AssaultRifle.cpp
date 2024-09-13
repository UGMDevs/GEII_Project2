// Fill out your copyright notice in the Description page of Project Settings.


#include "TP_AssaultRifle.h"
#include "Engine/EngineTypes.h"
#include "CollisionQueryParams.h"
#include "GEII_Project2Character.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"



UTP_AssaultRifle::UTP_AssaultRifle()
{
	// Definition for the particle effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem>
		DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));

	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	// Damage
	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;

	NumberOfFiringBullets = 1;
	SpreadAngle = 0.f;
}

void UTP_AssaultRifle::HandleFire()
{
		// Try and do a line trace
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			if (PlayerController == nullptr)
			{
				return;
			}

			//Get camera location and rotation
			FVector CameraLocation;
			FRotator CameraRotation;
			PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

			if (NumberOfFiringBullets > 0)
			{
				// Try and play the sound if specified
				if (FireSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
				}

				// Try and play a firing animation if specified
				if (FireAnimation != nullptr)
				{
					// Get the animation object for the arms mesh
					UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
					if (AnimInstance != nullptr)
					{
						AnimInstance->Montage_Play(FireAnimation, 1.f);
					}
				}

				for( int32 i = 0; i< NumberOfFiringBullets; i++)
				{
					// Generate random yaw and pitch variation within the spread angle
					float RandomYaw = FMath::RandRange(-SpreadAngle, SpreadAngle);
					float RandomPitch = FMath::RandRange(-SpreadAngle, SpreadAngle);

					// Calculate end location for line trace
					FRotator AdjustedDirection = CameraRotation;
					AdjustedDirection.Yaw += RandomYaw;
					AdjustedDirection.Pitch += RandomPitch;

					FVector EndLocation = CameraLocation + (AdjustedDirection.Vector() * 4000.f);
				
					// Perform line trace using visibility line trace
					FHitResult HitResult;
					FCollisionQueryParams CollisionParams;
					CollisionParams.AddIgnoredActor(Character);
					bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndLocation, ECC_Camera, CollisionParams);

					if (bHit)
					{
						GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, TEXT("Hit"));

						//UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, GetOwner()->GetInstigator()->Controller, this->GetOwner(), DamageType);
						UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, CameraLocation, HitResult, GetOwner()->GetInstigatorController(), GetOwner(), DamageType);

						FVector spawnLocation = HitResult.Location;
						UGameplayStatics::SpawnEmitterAtLocation(
							this,
							ExplosionEffect,
							spawnLocation,
							FRotator::ZeroRotator,
							true,
							EPSCPoolMethod::AutoRelease
						);
					}
				}
			}
		}
}
