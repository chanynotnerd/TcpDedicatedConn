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
	bool ConnectToServer(const FString& ServerIP, const int32 ServerPort);	// 서버 연결

	UFUNCTION(BlueprintCallable)
	void SendDataToServer(const FString& Data);	// 서버로 데이터 전송

	UFUNCTION(BlueprintCallable)
	FString ReceiveDataFromServer();	// 서버로부터 데이터 수신

private:
	FSocket* ConnectionSocket;

};
