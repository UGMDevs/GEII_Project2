// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_PickUpComponent.h"
#include "Net/UnrealNetwork.h"

UTP_PickUpComponent::UTP_PickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UTP_PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UTP_PickUpComponent::OnSphereBeginOverlap);
}

void UTP_PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AGEII_Project2Character* Character = Cast<AGEII_Project2Character>(OtherActor);
	if(Character && Character->GetLocalRole() == ROLE_Authority)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);
	}
}
