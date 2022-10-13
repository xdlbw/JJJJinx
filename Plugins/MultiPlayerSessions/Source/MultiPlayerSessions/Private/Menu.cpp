// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiPlayerSessionSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"

void UMenu::MenuSetup(int32 NumOfPublicConnections, FString TypeOfMatch, FString LobbyPath) {
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();    //添加到视口
	SetVisibility(ESlateVisibility::Visible);   //设置可见性
	bIsFocusable = true;      //获取鼠标

	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeUIOnly InputModeData;     //输入模式设置为仅UI
			InputModeData.SetWidgetToFocus(TakeWidget());    //UI控件聚焦
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);   //鼠标不锁定
			PlayerController->SetInputMode(InputModeData);   //设置玩家控制器输入为InputModeData
			PlayerController->SetShowMouseCursor(true);      //鼠标可见
		}
	}
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance) {
		MultiPlayerSessionSubsystem = GameInstance->GetSubsystem<UMultiPlayerSessionSubsystem>();
	}

	if (MultiPlayerSessionSubsystem) {
		MultiPlayerSessionSubsystem->MultiPlayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiPlayerSessionSubsystem->MultiPlayerOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSession);
		MultiPlayerSessionSubsystem->MultiPlayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiPlayerSessionSubsystem->MultiPlayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiPlayerSessionSubsystem->MultiPlayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize()) {
		return false;
	}

	if (HostButton) {
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton) {
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Yellow,
				FString(TEXT("Session Created Successfully"))
			);
		}

		UWorld* World = GetWorld();
		if (World) {
			World->ServerTravel(PathToLobby);
		}
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session"))
			);
		}
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiPlayerSessionSubsystem == nullptr) {
		return;
	}

	for (auto Result : SessionResults) {
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType) {
			MultiPlayerSessionSubsystem->JoinSession(Result);
			return;
		}
	}
	if (!bWasSuccessful || SessionResults.Num() == 0) {
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem) {
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController) {
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success) {
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiPlayerSessionSubsystem) {
		MultiPlayerSessionSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (MultiPlayerSessionSubsystem) {
		MultiPlayerSessionSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController) {
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
