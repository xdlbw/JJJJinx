// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "MultiPlayerSessionSubsystem.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState->PlayerArray.Num();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		UMultiPlayerSessionSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiPlayerSessionSubsystem>();
		check(Subsystem);

		if (NumberOfPlayers == Subsystem->DesiredNumPublicConnections) {
			UWorld* World = GetWorld();
			if (World) {
				//ÎÞ·ìÂÃÐÐ
				bUseSeamlessTravel = true;

				FString MatchType = Subsystem->DesiredMatchType;
				if (MatchType == "FreeForAll") {
					World->ServerTravel(FString("Game/Maps/BlasterMap?listen"));
				}
				else if (MatchType == "Teams") {
					World->ServerTravel(FString("Game/Maps/Teams?listen"));
				}
				else if (MatchType == "CTF") {
					World->ServerTravel(FString("Game/Maps/CTF?listen"));
				}
			}
		}
	}
}