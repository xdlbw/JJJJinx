// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState->PlayerArray.Num();
	if (NumberOfPlayers == 2) {
		UWorld* World = GetWorld();
		if (World) {
			//ÎÞ·ìÂÃÐÐ
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("Game/Maps/BlasterMap?listen"));
		}
	}
}