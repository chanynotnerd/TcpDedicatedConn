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

};
