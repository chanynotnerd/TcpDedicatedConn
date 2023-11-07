// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"
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
	// 클라이언트 연결 수신용 소켓
	FSocket* ConnectionSocket;
	// 연결된 클라이언트와 통신용 소켓

	FIPv4Endpoint RemoteAddress;
	// 연결된 클라이언트의 주소 객체
	uint32 PendingDataSize;
	// 소켓에 대기중인 데이터의 크기를 나타냄
	FString RemoteAddressString;
	// 연결된 클라이언트의 주소를 문자열 저장 변수

public:
	bool StartTCPReceiver(const FString& SocketName, const FString& IP, const int32 Port);
	// TCP 서버 시작 및 클라이언트 연결 대기 함수. 서버의 메인 시작점
	// 타이머로 TCPSocketListener() 함수를 주기적으로 호출.
	// 매개변수: 소켓이름, IP, Port

private:
	bool CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize = 2 * 1024 * 1024);
	// TCP연결 수신 위한 Listener 소켓 생성 및 설정 함수
	// Listener소켓 생성 및 지정된 IP주소 포트 바인딩. 클라이언트의 연결 대기 위한 소켓.
	// 매개변수: 소켓이름, IP, Port, 데이터 수신 버퍼 크기

	void TCPSocketListener();
	// 클라리언트 연결 수신 및 처리 함수
	// ListenerSocket에서 클라이언트 연결 대기 및 연결 성립시 클라리언트의 주소 기록 및 연결 쇼트.
};
