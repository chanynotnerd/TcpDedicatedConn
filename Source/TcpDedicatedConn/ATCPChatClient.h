// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "ATCPChatClient.generated.h"

UCLASS()
class TCPDEDICATEDCONN_API AATCPChatClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AATCPChatClient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	bool ConnectToServer(const FString& ServerIP, const int32 ServerPort);	// 서버에 연결을 시도하는 함수

	UFUNCTION(BlueprintCallable)
	void SendDataToServer(const FString& Data);	// 서버로 데이터를 전송하는 함수

	UFUNCTION(BlueprintCallable)
	FString ReceiveDataFromServer();	// 서버로부터 데이터를 수신하는 함수

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SendChatMessage(const FString& Message);
	
	FString ReceiveChatMessage();

private:
	FSocket* ConnectionSocket;	// 서버와의 연결 소켓을 관리하는 변수.

};
