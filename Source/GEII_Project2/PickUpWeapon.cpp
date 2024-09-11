// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpWeapon.h"
#include "Components/BoxComponent.h"
#include "GEII_Project2Character.h"
#include "AC_Inventory.h"

// Sets default values
APickUpWeapon::APickUpWeapon()
{
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    CollisionComponent->SetCollisionProfileName(TEXT("Trigger"));
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APickUpWeapon::OnOverlapBegin);
    RootComponent = CollisionComponent;

}

// Called when the game starts or when spawned
void APickUpWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickUpWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AGEII_Project2Character* Character = Cast<AGEII_Project2Character>(OtherActor);
    if (Character && WeaponClass)
    {
        UAC_Inventory* InventoryComponent = Character->FindComponentByClass<UAC_Inventory>();
        if (InventoryComponent)
        {
            // Add the weapon class to the inventory
            InventoryComponent->AddWeapon(WeaponClass);

            // Switch to the newly added weapon
            InventoryComponent->SwitchWeapon(InventoryComponent->GetWeapons().Num() - 1);

            // Destroy the pickup actor
            Destroy();
        }
    }
}



