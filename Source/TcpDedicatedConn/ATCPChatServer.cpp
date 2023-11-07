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
        // Accept() 함수를 상요하여 클라이언트 연결  요청 수락 및 클라이언트와 통신할 소켓 생성, ConnectingSocket에 할당.
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