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
	// Ŭ���̾�Ʈ ���� ���ſ� ����
	FSocket* ConnectionSocket;
	// ����� Ŭ���̾�Ʈ�� ��ſ� ����

	FIPv4Endpoint RemoteAddress;
	// ����� Ŭ���̾�Ʈ�� �ּ� ��ü
	uint32 PendingDataSize;
	// ���Ͽ� ������� �������� ũ�⸦ ��Ÿ��
	FString RemoteAddressString;
	// ����� Ŭ���̾�Ʈ�� �ּҸ� ���ڿ� ���� ����

public:
	bool StartTCPReceiver(const FString& SocketName, const FString& IP, const int32 Port);
	// TCP ���� ���� �� Ŭ���̾�Ʈ ���� ��� �Լ�. ������ ���� ������
	// Ÿ�̸ӷ� TCPSocketListener() �Լ��� �ֱ������� ȣ��.
	// �Ű�����: �����̸�, IP, Port

private:
	bool CreateTCPConnectionListener(const FString& SocketName, const FString& IP, const int32 Port, const int32 ReceiveBufferSize = 2 * 1024 * 1024);
	// TCP���� ���� ���� Listener ���� ���� �� ���� �Լ�
	// Listener���� ���� �� ������ IP�ּ� ��Ʈ ���ε�. Ŭ���̾�Ʈ�� ���� ��� ���� ����.
	// �Ű�����: �����̸�, IP, Port, ������ ���� ���� ũ��

	void TCPSocketListener();
	// Ŭ�󸮾�Ʈ ���� ���� �� ó�� �Լ�
	// ListenerSocket���� Ŭ���̾�Ʈ ���� ��� �� ���� ������ Ŭ�󸮾�Ʈ�� �ּ� ��� �� ���� ��Ʈ.
};
