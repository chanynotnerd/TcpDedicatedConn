// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "ATCPChatServer.generated.h"

UCLASS()
class TCPDEDICATEDCONN_API AATCPChatServer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AATCPChatServer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;

	FIPv4Endpoint RemoteAddress;
	uint32 PendingDataSize;
	FString RemoteAddressString;

	bool StartTCPReceiver(const FString& SocketName, const FString& IP, const int32 Port);
	bool CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize = 2 * 1024 * 1024);

	void TCPSocketListener();
};
