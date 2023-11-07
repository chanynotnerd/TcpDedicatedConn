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

bool AATCPChatClient::ConnectToServer(const FString& ServerIP, const int32 ServerPort)
{
	// ������ �����ϴ� �ڵ�
	TSharedRef<FInternetAddr> ServerAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid;
	ServerAddress->SetIp(*ServerIP, bIsValid);
	ServerAddress->SetPort(ServerPort);

	if (!bIsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("��ȿ���� ���� IP �ּ�: %s"), *ServerIP);
		return false;
	}

	ConnectionSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("ChatSocket"));
	if (ConnectionSocket)
	{
		if (ConnectionSocket->Connect(*ServerAddress))
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

void AATCPChatClient::SendDataToServer(const FString& Data)
{
	if (ConnectionSocket)
	{
		int32 BytesSent = 0;
		FTCHARToUTF8 ConvertedData(*Data);	// TCHAR�� UTF8�� �ٲ���
		bool bSendStatus = ConnectionSocket->Send((uint8*)ConvertedData.Get(), ConvertedData.Length(), BytesSent);

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

FString AATCPChatClient::ReceiveDataFromServer()
{
	FString ReceivedData = FString();

	if (ConnectionSocket)
	{
		uint32 Size;
		while (ConnectionSocket->HasPendingData(Size))
		{
			int32 BytesRead = 0;
			uint8* Data = new uint8[Size];
			if (ConnectionSocket->Recv(Data, Size, BytesRead))
			{
				ReceivedData = FString(UTF8_TO_TCHAR(Data));
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
