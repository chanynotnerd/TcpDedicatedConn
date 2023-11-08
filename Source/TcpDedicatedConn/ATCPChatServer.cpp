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
    // ���� �÷��̰� �Ǹ� ������ IP�ּҿ� Port�� �����ϰ� StartTCPReceiver() �Լ��� ȣ���Ͽ� TCP ���� ����.


    // ������ IP�ּҿ� ��Ʈ �ּ� ����.
    const FString ServerIP = TEXT("127.0.0.1"); 
    const int32 ServerPort = 7777; 

    // ������ ������ IP�� Port�� StartTCPReceiver() ȣ���ؼ� TCP ���� ����.
    if (!StartTCPReceiver(TEXT("ChatServer"), ServerIP, ServerPort))
    {
        // ���� ���⳻��.
        UE_LOG(LogTemp, Error, TEXT("Failed to start the TCP server."));
    }
}

// Called every frame
void AATCPChatServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// StartTCPReceiver(), TCP ���� ���� �� Ŭ���̾�Ʈ ���� ��� �Լ�. ������ ���� ������
bool AATCPChatServer::StartTCPReceiver(const FString& SocketName, const FString& IP, const int32 Port)
{
    // CreateTCPConnectionListener() �Լ� ȣ���ϸ� ListenerSocket ���� �� ����.
    bool bSuccess = CreateTCPConnectionListener(SocketName, IP, Port);
    if (bSuccess)
    {
        // FTimerHandle�� FTimerDelegate�� ����Ͽ� TCPSocketListener()�� �ֱ������� ȣ��(SetTimer() ���)
        // �̷� ���� ������ ����Ͽ� Ŭ���̾�Ʈ�� ������ Ȯ���ϰ� ó���� �� ����.
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

// Ŭ���̾�Ʈ�κ��� �޼����� ������ �Լ� SendChatMessageToClient()
void AATCPChatServer::SendChatMessageToClient(const FString& Message)
{
    // ConnectionSocket�� null���� �ƴ��� Ȯ��. null�̸� Ŭ���̾�Ʈ�� ������� ���� ������.
    if (ConnectionSocket != nullptr)
    {
        FTCHARToUTF8 Convert(*Message); // FString�� UTF8�� ��ȯ. �̷� ���� ����Ʈ �迭�� ��ȯ ����.
        // ����Ʈ �迭 ���� �� ��ȯ�� UTF8 ���ڿ� �߰�.
        TArray<uint8> SendBuffer;
        SendBuffer.Append((uint8*)Convert.Get(), Convert.Length());
        // Send() ����Ͽ� �ٲ� ����Ʈ�迭�� Ŭ���̾�Ʈ�� �ѷ���.
        int32 BytesSent;    // BytesSent ������ ������ Ŭ���̾�Ʈ���� ���� ����Ʈ ���� ����.
        ConnectionSocket->Send(SendBuffer.GetData(), SendBuffer.Num(), BytesSent);
    }
}


// Ŭ���̾�Ʈ�κ��� �޼����� �޴� �Լ�
FString AATCPChatServer::ReceiveChatMessageFromClient()
{
    // ���� �޼����� ������ FString ���� ����.
    FString ReceivedMessage;

    // ����Ǿ�����
    if (ConnectionSocket != nullptr)
    {
        // ConnectionSocket�� ������� �����Ͱ� �ִ��� Ȯ��. ������ �޾Ƽ� ReceivedMessage�� ����.
        uint32 Size;
        while (ConnectionSocket->HasPendingData(Size))
        {
            // ����Ʈ�迭�� �����Ͽ� ConnectionSocket���� �����͸� �о��.
            TArray<uint8> ReceiveBuffer;
            ReceiveBuffer.SetNumUninitialized(FMath::Min(Size, 65507u));

            // Recv() ȣ���Ͽ� �����͸� ����. Read ������ ������ �о���� ����Ʈ ���� ����.
            int32 Read = 0;
            ConnectionSocket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read);

            // ����Ʈ �迭�� UTF8�� ��ȯ �� �ٽ� FString���� ��ȯ.
            // Ŭ���̾�Ʈ�κ��� ���� �޼����� FString ���·� ����ϱ� ����.
            ReceivedMessage = FString(UTF8_TO_TCHAR((char*)ReceiveBuffer.GetData()));
        }
    }

    // ���� �޼����� ��ȯ.
    return ReceivedMessage;
}



// CreateTCPConnectionListener(), ListenerSocket ���� �� ����.
bool AATCPChatServer::CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize)
{
    // ���� �ּ� ��ü�� ����. Addr�̶�� ����Ʈ �����ͷ� �����Ͽ� ���� ���� ���� �� ������ ���� ���� �ּ� ��ü�� ����� ��.
    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

    // TSharedRef�� �𸮾��� ����Ʈ ������ ����. TSharedRef<FInternetAddr>�� ����� �־��� IP, Port�� Ȱ���Ͽ� TCP ListenerSocket�� �ּ� ����.
    // FInternetAddr�� ��Ʈ��ũ �ּ� ���� Ŭ�����̹Ƿ� �ּ� ������ �����ϰ� �����ϴ� ���� addr�� ����. FInternetAddr Ŭ�������� ��Ʈ��ũ ����Ʈ�� ��������.
    // ISocketSubsystem�� �پ��� �÷����� �ü���� ���� ���� �������̽��� �����Ͽ� �̽ļ��� ������ �� �ֱ� ����. �̷� ���� �پ��� ȯ�濡�� �������� ��Ʈ��ũ ��� ����
    // CreateInternetAddr()�� ���ο� ���� �ּ� ��ü�� �����ϸ� �� ��ü�� ��Ʈ��ũ ��ſ��� ���Ǵ� IP�� Port ���� �����ϴµ� ���.

    bool bIsValid;
    Addr->SetIp(*IP, bIsValid);
    Addr->SetPort(Port);

    // ������ ��ȿ�� �˻�.
    if (!bIsValid)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid IP address: %s"), *IP);
        return false;
    }

    FIPv4Endpoint Endpoint(Addr);   // Ŭ���̾�Ʈ �ּ� ��ü
    // Endpoint�� Ŭ���̾�Ʈ�� ���� ���� �� ����ϸ� Addr�� Endpoint�� �ʱ�ȭ����.


    // FTcpSocketBuilder�� ����Ͽ� ListenerSocket�� ���� �� ����.
    // ListenerSocket�� Ŭ���̾�Ʈ���� ������ ����ϰ� ����.
    ListenerSocket = FTcpSocketBuilder(*SocketName)
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .Listening(8);
    // FTcpSocketBuilder�� TCP ������ �����ϰ� �����ϱ� ���� builderŬ����
    // AsReusable()�� ���� ���� ���� ����. ���� Ŭ���̾�Ʈ���� ������ �ٷ�� ����.
    // BoundToEndpoint(Endpoint)�� Socket�� Ư�� Endpoint�� ���ε����־� Socket�� Endpoint�� ���� �� ���� ������ �� �ְ� ����.
    // Listening(8)�� Socket�� ���ÿ� ó���� �� �ִ� �ִ� ���� ��.

    return true;    // ���� ���� �� ������ �Ϸ�Ǹ� true�� ��ȯ���־� ������ �˸�.

    // ����: ���� Socket�� �����ϰ�, Client�� ������ ������ �غ� �Ϸ� ����. Ŭ���̾�Ʈ�� ���� ��û�� ��ٸ�. 
}


// �������� Ŭ���̾�Ʈ�� ������ �����ϰ�, ����� Ŭ���̾�Ʈ�� ������ �α׿� ����� �Լ�.
// ListenerSocket�� �ִ��� �˻� �� Ŭ���̾�Ʈ�� ���� ��û�� �ִ��� Ȯ��.
// ���� ��� ���̾��� �����Ͱ� ������ ���� ���� �� Ŭ���̾�Ʈ ������ �α׿� ���.
void AATCPChatServer::TCPSocketListener()
{
    if (!ListenerSocket)
    {
        // ListenerSocket�� �ִ��� �˻�.
        return;
    }

    RemoteAddress = FIPv4Endpoint();
    // ���� �ʱ�ȭ, ���� Ŭ���̾�Ʈ ����Ǹ� Ŭ���̾�Ʈ �ּ� ���� ���� ����.

    if (ListenerSocket->HasPendingData(PendingDataSize))    // HasPendingData()�� ���Ͽ� ������� Ŭ���̾�Ʈ�� ���� ��û�� �ִ��� Ȯ��.
    {
        ConnectionSocket = ListenerSocket->Accept(TEXT("TCPChatServer"));
        // Accept() �Լ��� ����Ͽ� Ŭ���̾�Ʈ ����  ��û ���� �� Ŭ���̾�Ʈ�� ����� ���� ����, ConnectingSocket�� �Ҵ�.
        if (ConnectionSocket)   // �Ҵ�Ǿ�����
        {
            RemoteAddressString = RemoteAddress.ToString();
            // Ŭ���̾�Ʈ�� �ּ� ������ ���ڿ��� ����.

            UE_LOG(LogTemp, Warning, TEXT("Client connected: %s"), RemoteAddress);
            // Ŭ���̾�Ʈ�� ���� ������ �α� ���.
            ConnectionSocket->Close();
            // ����� Ŭ���̾�Ʈ���� ���� ����� ������ ���� ����.
        }
    }
}
 
// �� ATCPChatServer Ŭ������ Ŭ���̾�Ʈ�� ������ �����ϰ� ó���ϴ� ���� ������ �����Ѵ�.
// Ŭ���̾�Ʈ�� ���� �� ����� �����ϰ� ���ο� Ŭ���̾�Ʈ�� ������ �����.