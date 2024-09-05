#ifndef FXTAP_CORE_KEYMAP_H
#define FXTAP_CORE_KEYMAP_H

typedef enum FxTapColumn
{
    FxTapColumn_1,
    FxTapColumn_2,
    FxTapColumn_3,
    FxTapColumn_4,
    FxTapColumn_5,
    FxTapColumn_6,
    FxTapColumn_7,
    FxTapColumn_8,
    FxTapColumn_9,
} FxTapColumn;

typedef enum FxTapKey
{
    FxTapKey_K1,
    FxTapKey_K2,
    FxTapKey_K3,
    FxTapKey_K4,
    FxTapKey_K5,
    FxTapKey_K6,
    FxTapKey_K7,
    FxTapKey_K8,
    FxTapKey_K9,
    FxTapKey_S1,
} FxTapKey;

typedef FxTapKey (*KeyMapper)(FxTapColumn);

FxTapKey KeyMapper_DJMAX_1K(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_2K(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_3K(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_4K(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_5K(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_6K(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_6KL(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_6KR(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_7K(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_8K(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_8KL(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_8KR(FxTapColumn column);
FxTapKey KeyMapper_DJMAX_9K(FxTapColumn column);

FxTapKey KeyMapper_BeatmaniaIIDX_4K(FxTapColumn column);
FxTapKey KeyMapper_BeatmaniaIIDX_5K(FxTapColumn column);
FxTapKey KeyMapper_BeatmaniaIIDX_6K(FxTapColumn column);
FxTapKey KeyMapper_BeatmaniaIIDX_7K(FxTapColumn column);
FxTapKey KeyMapper_BeatmaniaIIDX_8K(FxTapColumn column);
FxTapKey KeyMapper_BeatmaniaIIDX_8KL(FxTapColumn column);
FxTapKey KeyMapper_BeatmaniaIIDX_8KR(FxTapColumn column);

const char *FxTapKey_ToString(FxTapKey key);

#endif //FXTAP_CORE_KEYMAP_H
