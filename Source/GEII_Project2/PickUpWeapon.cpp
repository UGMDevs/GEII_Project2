// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpWeapon.h"

// Sets default values
APickUpWeapon::APickUpWeapon()
{
    Weapon = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("Weapon"));

    PickUp = CreateDefaultSubobject<UTP_PickUpComponent>(TEXT("PickUp"));
    PickUp->SetupAttachment(Weapon);
}