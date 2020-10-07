// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWidgetScoreEntry.h"

#include "ScoreEntry.h"

#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

void UBaseWidgetScoreEntry::NativeOnListItemObjectSet(UObject* ListItemObject) {
	auto entry = Cast<UScoreEntry>(ListItemObject);
	if (entry) {
		auto position = Super::WidgetTree->FindWidget<UTextBlock>("Position_Text");
		auto name = WidgetTree->FindWidget<UTextBlock>("Name_Text");
		auto score = WidgetTree->FindWidget<UTextBlock>("Score_Text");
		if (position)
			position->SetText(FText::FromString(FString::FromInt(entry->PlayerPosition)));
		if (name)
			name->SetText(FText::FromString(entry->PlayerName));
		if (score)
			score->SetText(FText::FromString(FString::FromInt(entry->PlayerScore)));
	}
}
