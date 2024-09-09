// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GEII_Project2Character.h"
#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GEII_Project2Projectile.h"
#include "Portal.generated.h"

UCLASS()
class GEII_PROJECT2_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

private:
	// Default scene root
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* DefaultSceneRoot;

	// Static mesh component for the portal
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PortalMesh;

	// Scene capture component 2D of the portal
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* PortalCamera;

	// Back facing scene component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* BackFacingScene;

	// Collision to detect player to pass through the portal
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Variable for the portal material 
	UPROPERTY(EditAnywhere, Category = "Portal")
	UMaterial* PortalMaterial;

	// Variable for linked portal
	UPROPERTY(EditAnywhere, BlueprintReadONly, Category="Portal")
	APortal* LinkedPortal;

	// Variable to set the portal color
	UPROPERTY(EditAnywhere, BlueprintReadONly, Category="Portal")
	FLinearColor PortalColor;

	// Variable for referencing the Portal Mesh Material
	UPROPERTY(BlueprintReadOnly, Category = "Portal")
	UMaterialInstanceDynamic* Portal_MAT;

	// Variable for Capture Render Target
	UPROPERTY(BlueprintReadOnly, Category = "Portal")
	UTextureRenderTarget2D* Portal_RT;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Function to update the Linked Portal's Camera
	void UpdateSceneCapture();

	// Begin Overlap
    UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// End Overlap
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	
private:
	// Begin Player Teleport

	AGEII_Project2Character* PlayerInPortal;

	UFUNCTION(Category = "TeleportPlayer")
	void CheckPlayerCanTeleport(AGEII_Project2Character* Player);

	UFUNCTION(Category = "TeleportPlayer")
	void TeleportPlayer(AGEII_Project2Character* Player);

	UFUNCTION(Server, Reliable, Category= "TeleportPlayer")
	void Server_TeleportPlayer(AGEII_Project2Character* Player);
	
	//End Player Teleport
	

private:
	// Begin Projectile Teleport

	UPROPERTY(VisibleAnywhere, Category = "TeleportProjectile")
	TSet<AGEII_Project2Projectile*> ProjectilesToIgnore;

	void TeleportProjectile(AGEII_Project2Projectile* Projectile);

	void AddProjectileToIgnore(AGEII_Project2Projectile* Projectile);

	//End Projectile Teleport

private:
	// Begin Portal Spawn

	UPROPERTY()
	USceneCaptureComponent2D* LinkedPortalCamera;

protected:
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawn")
	AActor* CurrentWall;

	// Check and adjust portal position in the wall
	UFUNCTION(Category = "PortalSpawn")
	void CheckPortalBounds();

public:

	// Function to give a reference of the Portal Camera to the Linked Portal requesting it
	UFUNCTION(Category = "PortalSpawn")
	USceneCaptureComponent2D* GetSceneCapture() const;

	// Function to receive reference of a portal to link
	UFUNCTION(Category = "PortalSpawn")
	void SetPortalToLink(APortal* PortalToLink);

	// Place a portal adjacent to a wall
	UFUNCTION(Category = "PortalSpawn")
	void PlacePortal(FVector NewLocation, FRotator NewRotation);

	// Get a reference to the linked portal
	UFUNCTION(Category = "PortalSpawn")
	APortal* GetLinkedPortal();

	// Setup the linked portal for the render target
	UFUNCTION(Category = "PortalSpawn")
	void SetupLinkedPortal();

	// Set the current wall to be the one the portal spawned in
	UFUNCTION(Category = "PortalSpawn")
	void SetCurrentWall(AActor* NewWall);

	// End Portal Spawn
};
