// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpWeapon.h"

// Sets default values
APickUpWeapon::APickUpWeapon()
{
    Weapon = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("Weapon"));

    PickUp = CreateDefaultSubobject<UTP_PickUpComponent>(TEXT("PickUp"));
    PickUp->SetupAttachment(Weapon);
   

    bReplicates = true;
}




void APickUpWeapon::OnPickupCollected()
{
    if (HasAuthority())
    {
        // Hide the pickup on the server
        Server_HidePickup();

        // Set a timer to reactivate the pickup
        GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &APickUpWeapon::Server_ReactivatePickup, RespawnTime);
    }
    else
    {
        // Request the server to handle the pickup collection
        Server_HidePickup();
    }
}

void APickUpWeapon::ReactivatePickup()
{
    if (HasAuthority())
    {
        // Reactivate the pickup on the server
        Server_ReactivatePickup();
    }
}

void APickUpWeapon::Server_HidePickup_Implementation()
{
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    // Disable collision for all primitive components
    TArray<UPrimitiveComponent*> PrimitiveComponents;
    GetComponents<UPrimitiveComponent>(PrimitiveComponents);

    for (UPrimitiveComponent* Comp : PrimitiveComponents)
    {
        Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Comp->SetVisibility(false);  // Ensure visibility is also disabled if necessary
    }
        
}

bool APickUpWeapon::Server_HidePickup_Validate()
{
    return true; // Add validation logic if needed
}

void APickUpWeapon::Server_ReactivatePickup_Implementation()
{
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);

    // Re-enable collision for all primitive components
    TArray<UPrimitiveComponent*> PrimitiveComponents;
    GetComponents<UPrimitiveComponent>(PrimitiveComponents);

    for (UPrimitiveComponent* Comp : PrimitiveComponents)
    {
        Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Comp->SetVisibility(true);  // Ensure visibility is enabled when reactivating
    }
}

bool APickUpWeapon::Server_ReactivatePickup_Validate()
{
    return true; // Add validation logic if needed
}



