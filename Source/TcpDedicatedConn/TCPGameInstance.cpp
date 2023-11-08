// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPGameInstance.h"

UTCPGameInstance::UTCPGameInstance()
{
    // Ŭ���̾�Ʈ �� ���� ��ü �ʱ�ȭ
    ChatClient = nullptr;
    ChatServer = nullptr;
}

void UTCPGameInstance::Init()
{
    Super::Init();

    // ���� IP �ּ� �� ��Ʈ ����
    const FString ServerIP = TEXT("127.0.0.1");
    const int32 ServerPort = 7777;

    // Ŭ���̾�Ʈ ��ü ���� �� �ʱ�ȭ
    ChatClient = GetWorld()->SpawnActor<AATCPChatClient>();
    if (ChatClient)
    {
        ChatClient->ConnectToServer(ServerIP, ServerPort);
    }

    // ���� ��ü ���� �� �ʱ�ȭ
    ChatServer = GetWorld()->SpawnActor<AATCPChatServer>();
    if (ChatServer)
    {
        ChatServer->StartTCPReceiver(TEXT("ChatServer"), ServerIP, ServerPort);
    }
}
