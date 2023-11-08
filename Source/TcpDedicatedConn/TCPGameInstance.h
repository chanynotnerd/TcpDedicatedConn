// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ATCPChatClient.h"
#include "ATCPChatServer.h"
#include "TCPGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TCPDEDICATEDCONN_API UTCPGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UTCPGameInstance();

	virtual void Init() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AATCPChatClient* ChatClient;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AATCPChatServer* ChatServer;
	// 서버와 클라이언트들을 초기화 및 관리하기 위한 GameInstance.
	// 그리고 액터로 만든 클래스들을 직접 객체로 만들어주어 레벨이 바뀌었을때 연결이 끊기지 않게 하기 위함.
};
