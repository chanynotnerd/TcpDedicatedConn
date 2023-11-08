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

	// 서버 IP 주소 및 포트 정보
	const FString ServerIP = TEXT("127.0.0.1"); // 서버 IP 주소
	const int32 ServerPort = 7777; // 서버 포트
}

// Called every frame
void AATCPChatClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ConnectToServer(), 주어진 IP주소와 Port번호로 서버에 연결을 시도하는 함수.
bool AATCPChatClient::ConnectToServer(const FString& ServerIP, const int32 ServerPort)
{
	// 서버에 연결하는 코드, 주소를 생성하고 IP와 Port를 설정해줌.
	TSharedRef<FInternetAddr> ServerAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool bIsValid;
	ServerAddress->SetIp(*ServerIP, bIsValid);
	ServerAddress->SetPort(ServerPort);

	// 유효성 검사
	if (!bIsValid)
	{
		UE_LOG(LogTemp, Error, TEXT("유효하지 않은 IP 주소: %s"), *ServerIP);
		return false;
	}

	ConnectionSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("ChatSocket"));
	// ChatSocket을 생성
	if (ConnectionSocket)
	{
		if (ConnectionSocket->Connect(*ServerAddress))	// ConnectionSocket으로 서버에 연결을 시도.
		{
			UE_LOG(LogTemp, Warning, TEXT("서버에 연결되었습니다."));
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("서버에 연결하지 못했습니다."));
			ConnectionSocket->Close();
			return false;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("소켓을 생성하지 못했습니다."));
	return false;

}

// SendDataToServer(), 클라이언트가 서버로 데이터를 전송하는 과정을 담당함.
void AATCPChatClient::SendDataToServer(const FString& Data)
{
	if (ConnectionSocket)	// ConnectionSocket이 유효하다면

	{
		int32 BytesSent = 0;
		FTCHARToUTF8 ConvertedData(*Data);	// TCHAR를 UTF8로 변환
		bool bSendStatus = ConnectionSocket->Send((uint8*)ConvertedData.Get(), ConvertedData.Length(), BytesSent);
		// Send()를 호출하여 서버로 데이터를 전송해줌.

		if (bSendStatus)
		{
			UE_LOG(LogTemp, Warning, TEXT("데이터를 서버로 전송했습니다: %s"), *Data);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("데이터를 서버로 전송하지 못했습니다."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("서버와의 연결이 없습니다."));
	}
}

// ReceiveDataFromServer()는 클라이언트가 서버로부터 데이터를 수신하는 과정을 담당.
FString AATCPChatClient::ReceiveDataFromServer()
{
	FString ReceivedData = FString();	    // 받은 메세지를 저장할 FString 변수 선언.

	if (ConnectionSocket)	// ConnectionSocket이 유효하다면?
	{
		uint32 Size;
		while (ConnectionSocket->HasPendingData(Size))	// 수신 대기중인 데이터가 있는지 확인.
		{
			int32 BytesRead = 0;
			uint8* Data = new uint8[Size];
			if (ConnectionSocket->Recv(Data, Size, BytesRead))	// 수신 대기중인 데이터가 있으면 데이터 수신
			{
				ReceivedData = FString(UTF8_TO_TCHAR(Data));	// 데이터 변환
				delete[] Data;
				UE_LOG(LogTemp, Warning, TEXT("서버로부터 데이터를 수신했습니다: %s"), *ReceivedData);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("서버로부터 데이터를 수신하지 못했습니다."));
				delete[] Data;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("서버와의 연결이 없습니다."));
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
		UE_LOG(LogTemp, Warning, TEXT("받은 메시지: %s"), *ReceivedMessage);
	}
	return ReceivedMessage;
}
