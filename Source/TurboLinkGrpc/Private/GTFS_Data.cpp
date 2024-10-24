// Fill out your copyright notice in the Description page of Project Settings.


#include "GTFS_Data.h"

#include "FileDownloadManager.h"
#include "gtfs-realtime.pb.h"
#include "Kismet/GameplayStatics.h"

transit_realtime::FeedMessage UGTFS_Data::GetFeedMessageFromVehiclePositionsBinary()
{
	TArray<uint8> DataFromBinary;
	FString BaseDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "/GTFS_Temp/";
	FString Filename = "VehiclePosition.pb";
	FString FullPath = (BaseDir + Filename);
	// const FString FilePath = "C:/Users/AsusC/Downloads/VehiclePosition.pb";
	FFileHelper::LoadFileToArray(DataFromBinary, *FullPath);
	
	transit_realtime::FeedMessage Message;
	if(!Message.ParseFromArray(DataFromBinary.GetData(), DataFromBinary.Num()))
	{
		UE_LOG(LogTemp, Error, TEXT("UGTFS_Test::GetFeedMessageFromVehiclePositionsBinary(): Failed to parse FeedMessage from Array!"));
	}
	return Message;
}

FString UGTFS_Data::ReturnTestPositionWithDebug(UObject* Outer)
{
	const transit_realtime::FeedMessage Message = GetFeedMessageFromVehiclePositionsBinary();
	
	std::string Test = Message.DebugString();
	FString Debug = Test.c_str();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Debug);
	//
	UE_LOG(LogTemp, Error, TEXT("PARSED!!!! %s"), *FString::SanitizeFloat(Message.entity()[0].vehicle().position().latitude()));
	
	transit_realtime::VehiclePosition Position;
	transit_realtime::Position* Internal = new transit_realtime::Position;
	Internal->set_latitude(45.0f);
	Internal->set_longitude(543.0f);
	Position.set_allocated_position(Internal);

	FString Out = FString::SanitizeFloat(Position.position().latitude());
	return Out;
}

FString UGTFS_Data::RetrieveVehiclePositionsMessageAsString()
{
	const transit_realtime::FeedMessage Message = GetFeedMessageFromVehiclePositionsBinary();
	const std::string DebugString = Message.DebugString();
	return DebugString.c_str();
}

UFileDownloadManager* UGTFS_Data::DownloadVehiclePositions(UObject* Outer)
{
	UFileDownloadManager* DownloadManager = Cast<UFileDownloadManager>(UGameplayStatics::SpawnObject(UFileDownloadManager::StaticClass(),  Outer));

	const FString URL = "https://www.rtd-denver.com/files/gtfs-rt/VehiclePosition.pb";

	const FString BaseDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir()) + "/GTFS_Temp/";
	const FString Filename = "VehiclePosition.pb";
	const FString FullPath = (BaseDir + Filename);
	if(FPaths::FileExists(FullPath))
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		PlatformFile.DeleteFile(*FullPath);
	}
	const int32 TaskID = DownloadManager->AddTaskByUrl(URL, BaseDir, Filename);
	DownloadManager->StartTask(TaskID);
	return DownloadManager;
}

FString UGTFS_Data::RetrieveCountedVehiclePositions(int32 NumVehicles, TArray<FVector2D> Positions)
{
	transit_realtime::FeedMessage Message = GetFeedMessageFromVehiclePositionsBinary();
	FString Out;
	NumVehicles = FMath::Clamp(NumVehicles, 1, Message.entity().size() - 1);
	for (int i = 0; i < NumVehicles; i++)
	{
		transit_realtime::FeedEntity Entity = Message.entity()[i];
		transit_realtime::Position Position = Entity.vehicle().position();
		FVector2D Vec = FVector2D(Position.longitude(), Position.latitude());
		Positions.Add(Vec);

		std::string RawID = Entity.vehicle().vehicle().id();
		FString ID = RawID.c_str();
		Out += (" ID: " + ID + " Loc " + Vec.ToString());
	}
	return Out;
}

TMap<FString, FLimitedVehicleData> UGTFS_Data::RetrieveStructuredVehiclePositions()
{
	const transit_realtime::FeedMessage Message = GetFeedMessageFromVehiclePositionsBinary();
	TMap<FString, FLimitedVehicleData> Data;
	for (transit_realtime::FeedEntity Entity : Message.entity())
	{
		FLimitedVehicleData Vehicle;
		Vehicle.VehicleID = Entity.vehicle().vehicle().id().c_str();
		transit_realtime::Position Position = Entity.vehicle().position();
		Vehicle.Position = FVehiclePosition(Position.latitude(),Position.longitude(),Position.bearing());
		
		Data.Add(Vehicle.VehicleID, Vehicle);
	}
	
	return Data;
}
