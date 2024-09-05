#ifndef FXTAP_CORE_RENDER_H
#define FXTAP_CORE_RENDER_H

#include <fxTap/game.h>

typedef void (*TapRenderer)(char column, double positionBottom);

typedef void (*HoldRenderer)(char column, double positionBottom, double positionTop);

typedef struct RendererController
{
    double HeightAbove;

    // Time needed for note to fall onto the line after it appears from the top.
    FxtapTime VisibleTime;

    // Render methods are injected to this controller, and will be called automatically.
    TapRenderer RenderTap;
    HoldRenderer RenderHold;
} RendererController;

void RendererController_Run(const RendererController *controller, const FxTap *fxTap, FxtapTime timeNow);

#endif //FXTAP_CORE_RENDER_H
