// Copyright Epic Games, Inc. All Rights Reserved.

#include "TcpDedicatedConnGameMode.h"
#include "TcpDedicatedConnCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATcpDedicatedConnGameMode::ATcpDedicatedConnGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
