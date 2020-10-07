// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "UObject/UObjectGlobals.h"

#include "BaseWidgetScoreEntry.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class NG_TEST_MO_API UBaseWidgetScoreEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	//Sets the widgets getting the values from the ScoreEntry UObject
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

};
