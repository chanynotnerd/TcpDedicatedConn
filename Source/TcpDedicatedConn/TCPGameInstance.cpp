// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPGameInstance.h"

UTCPGameInstance::UTCPGameInstance()
{
    // 클라이언트 및 서버 객체 초기화
    ChatClient = nullptr;
    ChatServer = nullptr;
}

void UTCPGameInstance::Init()
{
    Super::Init();

    // 서버 IP 주소 및 포트 설정
    const FString ServerIP = TEXT("127.0.0.1");
    const int32 ServerPort = 7777;

    // 클라이언트 객체 생성 및 초기화
    ChatClient = GetWorld()->SpawnActor<AATCPChatClient>();
    if (ChatClient)
    {
        ChatClient->ConnectToServer(ServerIP, ServerPort);
    }

    // 서버 객체 생성 및 초기화
    ChatServer = GetWorld()->SpawnActor<AATCPChatServer>();
    if (ChatServer)
    {
        ChatServer->StartTCPReceiver(TEXT("ChatServer"), ServerIP, ServerPort);
    }
}
