#pragma once

#include <fxTap/game.h>

typedef void (*FXT_TapRenderer)(int column, double positionBottom);

typedef void (*FXT_HoldRenderer)(int column, double positionBottom, double positionTop);

typedef struct FXT_RendererController
{
	double HeightAbove;

	// Time needed for note to fall onto the line after it appears from the top.
	FXT_TimeMs VisibleTime;

	// Render methods are injected to this controller, and will be called automatically.
	FXT_TapRenderer RenderTap;
	FXT_HoldRenderer RenderHold;
} FXT_RendererController;

void FXT_RendererController_Run(const FXT_RendererController *controller, const FXT_Game *game, FXT_TimeMs timeNow);
