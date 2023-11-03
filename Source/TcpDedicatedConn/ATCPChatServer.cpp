// Fill out your copyright notice in the Description page of Project Settings.


#include "ATCPChatServer.h"
#include "Networking.h"


// Sets default values
AATCPChatServer::AATCPChatServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AATCPChatServer::BeginPlay()
{
	Super::BeginPlay();

    const FString ServerIP = TEXT("127.0.0.1"); 
    const int32 ServerPort = 7777; 

    if (!StartTCPReceiver(TEXT("ChatServer"), ServerIP, ServerPort))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to start the TCP server."));
    }
}

// Called every frame
void AATCPChatServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AATCPChatServer::StartTCPReceiver(const FString& SocketName, const FString& IP, const int32 Port)
{
    bool bSuccess = CreateTCPConnectionListener(SocketName, IP, Port);
    if (bSuccess)
    {
        FTimerHandle TimerHandle;
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUFunction(this, FName("TCPSocketListener"));
        GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.01, true);
        return true;
    }
    else
    {
        return false;
    }
}

bool AATCPChatServer::CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize)
{
    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool bIsValid;
    Addr->SetIp(*IP, bIsValid);
    Addr->SetPort(Port);

    if (!bIsValid)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid IP address: %s"), *IP);
        return false;
    }

    FIPv4Endpoint Endpoint(Addr);

    ListenerSocket = FTcpSocketBuilder(*SocketName)
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .Listening(8);

    return true;
}

void AATCPChatServer::TCPSocketListener()
{
    if (!ListenerSocket)
    {
        return;
    }

    // Remote address that will be assigned when a connection is accepted
    RemoteAddress = FIPv4Endpoint();

    if (ListenerSocket->HasPendingData(PendingDataSize
    ))
    {
        ConnectionSocket = ListenerSocket->Accept(TEXT("TCPChatServer"));
        if (ConnectionSocket)
        {
            RemoteAddress.ToString(RemoteAddressString);
            UE_LOG(LogTemp, Warning, TEXT("Client connected: %s"), *RemoteAddress);
            ConnectionSocket->Close();
        }
    }
}



