// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "gtfs-realtime.pb.h"
#include "UObject/Object.h"
#include "GTFS_Data.generated.h"

USTRUCT(BlueprintType)
struct FVehiclePosition
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Latitude;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Longitude;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Bearing;

	FVehiclePosition()
	{
		//
	}

	FVehiclePosition(double InLat, double InLong, double InDir)
	{
		Latitude = InLat;
		Longitude = InLong;
		Bearing = InDir;
	}	
};

USTRUCT(BlueprintType)
struct FLimitedVehicleData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString VehicleID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVehiclePosition Position;

	FLimitedVehicleData()
	{
		//
	}

	bool operator==(const FLimitedVehicleData& Other) const
	{
		return VehicleID == Other.VehicleID;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FLimitedVehicleData& Data)
	{
		return GetTypeHash(Data.VehicleID);
	}

};

class UFileDownloadManager;

/**
 * 
 */
UCLASS()
class TURBOLINKGRPC_API UGTFS_Data : public UObject
{
	GENERATED_BODY()

public:

	static transit_realtime::FeedMessage GetFeedMessageFromVehiclePositionsBinary();
	
	static FString ReturnTestPositionWithDebug(UObject* Outer);

	static FString RetrieveVehiclePositionsMessageAsString();

	static UFileDownloadManager* DownloadVehiclePositions(UObject* Outer);

	static FString RetrieveCountedVehiclePositions(int32 NumVehicles, TArray<FVector2D> Positions);

	static TMap<FString, FLimitedVehicleData> RetrieveStructuredVehiclePositions();
};
