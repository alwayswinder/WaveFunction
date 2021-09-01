// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/ICursor.h"
#include "Framework/Application/IInputProcessor.h"
#include "WFCTileAssetToolkit.h"

class FSlateApplication;
struct FAnalogInputEvent;
struct FKeyEvent;
struct FPointerEvent;
class UWFCTileAsset;

class FWFCInputProcessor
	: public TSharedFromThis<FWFCInputProcessor>
	, public IInputProcessor
{
public:
	virtual ~FWFCInputProcessor();

	static void Create();

	static FWFCInputProcessor& Get();

	void Cleanup();
	void AddNewTab(TWeakPtr<SDockTab> Tab, UWFCTileAsset* Asset);
	void RemoveTab(TWeakPtr<SDockTab> Tab);
	//~ IInputProcess overrides
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override;
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent,
		const FPointerEvent* InGestureEvent) override;

	virtual const TCHAR* GetDebugName() const override { return TEXT("WFCInteraction"); }

private:
	FWFCInputProcessor();
	TMap<TWeakPtr<SDockTab>, UWFCTileAsset*> TabMap;
};
