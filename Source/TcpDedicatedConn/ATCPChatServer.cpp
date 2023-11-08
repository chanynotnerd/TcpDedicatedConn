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
    // 게임 플레이가 되면 서버의 IP주소와 Port를 설정하고 StartTCPReceiver() 함수를 호출하여 TCP 서버 시작.


    // 서버에 IP주소와 포트 주소 설정.
    const FString ServerIP = TEXT("127.0.0.1"); 
    const int32 ServerPort = 7777; 

    // 위에서 지정한 IP와 Port로 StartTCPReceiver() 호출해서 TCP 서버 시작.
    if (!StartTCPReceiver(TEXT("ChatServer"), ServerIP, ServerPort))
    {
        // 오류 검출내용.
        UE_LOG(LogTemp, Error, TEXT("Failed to start the TCP server."));
    }
}

// Called every frame
void AATCPChatServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// StartTCPReceiver(), TCP 서버 시작 및 클라이언트 연결 대기 함수. 서버의 메인 시작점
bool AATCPChatServer::StartTCPReceiver(const FString& SocketName, const FString& IP, const int32 Port)
{
    // CreateTCPConnectionListener() 함수 호출하며 ListenerSocket 생성 및 설정.
    bool bSuccess = CreateTCPConnectionListener(SocketName, IP, Port);
    if (bSuccess)
    {
        // FTimerHandle과 FTimerDelegate를 사용하여 TCPSocketListener()를 주기적으로 호출(SetTimer() 사용)
        // 이로 인해 서버는 계속하여 클라이언트의 연결을 확인하고 처리할 수 있음.
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

// 클라이언트로부터 메세지를 보내는 함수 SendChatMessageToClient()
void AATCPChatServer::SendChatMessageToClient(const FString& Message)
{
    // ConnectionSocket이 null값이 아닌지 확인. null이면 클라이언트와 연결되지 않은 상태임.
    if (ConnectionSocket != nullptr)
    {
        FTCHARToUTF8 Convert(*Message); // FString을 UTF8로 변환. 이로 인해 바이트 배열로 변환 가능.
        // 바이트 배열 생성 및 변환한 UTF8 문자열 추가.
        TArray<uint8> SendBuffer;
        SendBuffer.Append((uint8*)Convert.Get(), Convert.Length());
        // Send() 사용하여 바꾼 바이트배열을 클라이언트에 뿌려줌.
        int32 BytesSent;    // BytesSent 변수는 실제로 클라이언트한테 보낸 바이트 수를 저장.
        ConnectionSocket->Send(SendBuffer.GetData(), SendBuffer.Num(), BytesSent);
    }
}


// 클라이언트로부터 메세지를 받는 함수
FString AATCPChatServer::ReceiveChatMessageFromClient()
{
    // 받은 메세지를 저장할 FString 변수 선언.
    FString ReceivedMessage;

    // 연결되었으면
    if (ConnectionSocket != nullptr)
    {
        // ConnectionSocket에 대기중인 데이터가 있는지 확인. 있으면 받아서 ReceivedMessage에 저장.
        uint32 Size;
        while (ConnectionSocket->HasPendingData(Size))
        {
            // 바이트배열로 생성하여 ConnectionSocket에서 데이터를 읽어옴.
            TArray<uint8> ReceiveBuffer;
            ReceiveBuffer.SetNumUninitialized(FMath::Min(Size, 65507u));

            // Recv() 호출하여 데이터를 읽음. Read 변수는 실제로 읽어들인 바이트 수를 저장.
            int32 Read = 0;
            ConnectionSocket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), Read);

            // 바이트 배열을 UTF8로 변환 후 다시 FString으로 변환.
            // 클라이언트로부터 받은 메세지를 FString 형태로 사용하기 위함.
            ReceivedMessage = FString(UTF8_TO_TCHAR((char*)ReceiveBuffer.GetData()));
        }
    }

    // 받은 메세지를 반환.
    return ReceivedMessage;
}



// CreateTCPConnectionListener(), ListenerSocket 생성 및 설정.
bool AATCPChatServer::CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize)
{
    // 소켓 주소 객체를 생성. Addr이라는 스마트 포인터로 관리하여 향후 소켓 생성 및 구성에 사용될 소켓 주소 객체를 만드는 것.
    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

    // TSharedRef는 언리얼의 스마트 포인터 종류. TSharedRef<FInternetAddr>를 사용해 주어진 IP, Port를 활용하여 TCP ListenerSocket의 주소 생성.
    // FInternetAddr은 네트워크 주소 정보 클래스이므로 주소 정보를 공유하고 관리하는 변수 addr을 선언. FInternetAddr 클래스에서 네트워크 바이트로 정렬해줌.
    // ISocketSubsystem은 다양한 플랫폼과 운영체제에 대해 소켓 인터페이스를 제공하여 이식성을 유지할 수 있기 위함. 이로 인해 다양한 환경에서 안정적인 네트워크 통신 지원
    // CreateInternetAddr()은 새로운 소켓 주소 객체를 생성하며 이 객체는 네트워크 통신에서 사용되는 IP와 Port 등을 관리하는데 사용.

    bool bIsValid;
    Addr->SetIp(*IP, bIsValid);
    Addr->SetPort(Port);

    // 소켓의 유효성 검사.
    if (!bIsValid)
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid IP address: %s"), *IP);
        return false;
    }

    FIPv4Endpoint Endpoint(Addr);   // 클라이언트 주소 객체
    // Endpoint는 클라이언트와 연결 수신 시 사용하며 Addr로 Endpoint를 초기화해줌.


    // FTcpSocketBuilder를 사용하여 ListenerSocket을 생성 및 설정.
    // ListenerSocket은 클라이언트와의 연결을 대기하고 있음.
    ListenerSocket = FTcpSocketBuilder(*SocketName)
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .Listening(8);
    // FTcpSocketBuilder는 TCP 소켓을 생성하고 설정하기 위한 builder클래스
    // AsReusable()은 소켓 재사용 가능 설정. 여러 클라이언트와의 연결을 다루기 위함.
    // BoundToEndpoint(Endpoint)는 Socket을 특정 Endpoint에 바인딩해주어 Socket이 Endpoint에 도달 시 연결 수신할 수 있게 도움.
    // Listening(8)은 Socket이 동시에 처리할 수 있는 최대 연결 수.

    return true;    // 소켓 생성 및 설정이 완료되면 true를 반환해주어 성공을 알림.

    // 정리: 서버 Socket을 설정하고, Client의 연결을 수신할 준비 완료 상태. 클라이언트의 연결 요청을 기다림. 
}


// 서버에서 클라이언트의 연결을 수신하고, 연결된 클라이언트의 정보를 로그에 남기는 함수.
// ListenerSocket이 있는지 검사 후 클라이언트의 연결 요청이 있는지 확인.
// 연결 대기 중이었던 데이터가 있으면 연결 수락 및 클라이언트 정보를 로그에 기록.
void AATCPChatServer::TCPSocketListener()
{
    if (!ListenerSocket)
    {
        // ListenerSocket이 있는지 검사.
        return;
    }

    RemoteAddress = FIPv4Endpoint();
    // 변수 초기화, 향후 클라이언트 연결되면 클라이언트 주소 정보 저장 예정.

    if (ListenerSocket->HasPendingData(PendingDataSize))    // HasPendingData()는 소켓에 대기중인 클라이언트의 연결 요청이 있는지 확인.
    {
        ConnectionSocket = ListenerSocket->Accept(TEXT("TCPChatServer"));
        // Accept() 함수를 사용하여 클라이언트 연결  요청 수락 및 클라이언트와 통신할 소켓 생성, ConnectingSocket에 할당.
        if (ConnectionSocket)   // 할당되었으면
        {
            RemoteAddressString = RemoteAddress.ToString();
            // 클라이언트의 주소 정보를 문자열로 저장.

            UE_LOG(LogTemp, Warning, TEXT("Client connected: %s"), RemoteAddress);
            // 클라이언트의 연결 정보를 로그 출력.
            ConnectionSocket->Close();
            // 연결된 클라이언트와의 소켓 통신이 끝나면 소켓 종료.
        }
    }
}
 
// 즉 ATCPChatServer 클래스는 클라이언트의 연결을 수신하고 처리하는 서버 역할을 수행한다.
// 클라이언트와 서버 간 통신을 관리하고 새로운 클라이언트의 연결을 대기함.