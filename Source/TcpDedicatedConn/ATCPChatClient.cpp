#include "ATCPChatClient.h"
#include "Networking.h"

// Sets default values
AATCPChatClient::AATCPChatClient()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConnectionSocket = nullptr;
}

// Called when the game starts or when spawned
void AATCPChatClient::BeginPlay()
{
	Super::BeginPlay();

	// ���� IP �ּ� �� ��Ʈ ����
	const FString ServerIP = TEXT("127.0.0.1"); // ���� IP �ּ�
	const int32 ServerPort = 7777; // ���� ��Ʈ
}

// Called every frame
void AATCPChatClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ConnectToServer(), �־��� IP�ּҿ� Port��ȣ�� ������ ������ �õ��ϴ� �Լ�.
bool AATCPChatClient::ConnectToServer(const FString& ServerIP, const int32 ServerPort)
{
	// ������ �����ϴ� �ڵ�, �ּҸ� �����ϰ� IP�� Port�� ��������.
	TSharedRef<FInternetAddr> ServerAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid;
	ServerAddress->SetIp(*ServerIP, bIsValid);
	ServerAddress->SetPort(ServerPort);

	// ��ȿ�� �˻�
	if (!bIsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("��ȿ���� ���� IP �ּ�: %s"), *ServerIP);
		return false;
	}

	ConnectionSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("ChatSocket"));
	// ChatSocket�� ����
	if (ConnectionSocket)
	{
		if (ConnectionSocket->Connect(*ServerAddress))	// ConnectionSocket���� ������ ������ �õ�.
		{
			UE_LOG(LogTemp, Warning, TEXT("������ ����Ǿ����ϴ�."));
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("������ �������� ���߽��ϴ�."));
			ConnectionSocket->Close();
			return false;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("������ �������� ���߽��ϴ�."));
	return false;

}

// SendDataToServer(), Ŭ���̾�Ʈ�� ������ �����͸� �����ϴ� ������ �����.
void AATCPChatClient::SendDataToServer(const FString& Data)
{
	if (ConnectionSocket)	// ConnectionSocket�� ��ȿ�ϴٸ�

	{
		int32 BytesSent = 0;
		FTCHARToUTF8 ConvertedData(*Data);	// TCHAR�� UTF8�� ��ȯ
		bool bSendStatus = ConnectionSocket->Send((uint8*)ConvertedData.Get(), ConvertedData.Length(), BytesSent);
		// Send()�� ȣ���Ͽ� ������ �����͸� ��������.

		if (bSendStatus)
		{
			UE_LOG(LogTemp, Warning, TEXT("�����͸� ������ �����߽��ϴ�: %s"), *Data);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("�����͸� ������ �������� ���߽��ϴ�."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("�������� ������ �����ϴ�."));
	}
}

// ReceiveDataFromServer()�� Ŭ���̾�Ʈ�� �����κ��� �����͸� �����ϴ� ������ ���.
FString AATCPChatClient::ReceiveDataFromServer()
{
	FString ReceivedData = FString();	    // ���� �޼����� ������ FString ���� ����.

	if (ConnectionSocket)	// ConnectionSocket�� ��ȿ�ϴٸ�?
	{
		uint32 Size;
		while (ConnectionSocket->HasPendingData(Size))	// ���� ������� �����Ͱ� �ִ��� Ȯ��.
		{
			int32 BytesRead = 0;
			uint8* Data = new uint8[Size];
			if (ConnectionSocket->Recv(Data, Size, BytesRead))	// ���� ������� �����Ͱ� ������ ������ ����
			{
				ReceivedData = FString(UTF8_TO_TCHAR(Data));	// ������ ��ȯ
				delete[] Data;
				UE_LOG(LogTemp, Warning, TEXT("�����κ��� �����͸� �����߽��ϴ�: %s"), *ReceivedData);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("�����κ��� �����͸� �������� ���߽��ϴ�."));
				delete[] Data;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("�������� ������ �����ϴ�."));
	}

	return ReceivedData;
}

void AATCPChatClient::SendChatMessage(const FString& Message)
{
	SendDataToServer(Message);
}

FString AATCPChatClient::ReceiveChatMessage()
{
	FString ReceivedMessage = ReceiveDataFromServer();
	if (!ReceivedMessage.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("���� �޽���: %s"), *ReceivedMessage);
	}
	return ReceivedMessage;
}
