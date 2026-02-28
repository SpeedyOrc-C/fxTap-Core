#pragma once

typedef enum FXT_Column
{
	FXT_Column_1,
	FXT_Column_2,
	FXT_Column_3,
	FXT_Column_4,
	FXT_Column_5,
	FXT_Column_6,
	FXT_Column_7,
	FXT_Column_8,
	FXT_Column_9,
} FXT_Column;

typedef enum FXT_Key
{
	FXT_Key_Invalid = -1,
	FXT_Key_K1,
	FXT_Key_K2,
	FXT_Key_K3,
	FXT_Key_K4,
	FXT_Key_K5,
	FXT_Key_K6,
	FXT_Key_K7,
	FXT_Key_K8,
	FXT_Key_K9,
	FXT_Key_S1,
} FXT_Key;

typedef FXT_Key (*KeyMapper)(FXT_Column);

FXT_Key KeyMapper_DJMAX_1K(FXT_Column column);

FXT_Key KeyMapper_DJMAX_2K(FXT_Column column);

FXT_Key KeyMapper_DJMAX_3K(FXT_Column column);

FXT_Key KeyMapper_DJMAX_4K(FXT_Column column);

FXT_Key KeyMapper_DJMAX_5K(FXT_Column column);

FXT_Key KeyMapper_DJMAX_6K(FXT_Column column);

FXT_Key KeyMapper_DJMAX_6KL(FXT_Column column);

FXT_Key KeyMapper_DJMAX_6KR(FXT_Column column);

FXT_Key KeyMapper_DJMAX_7K(FXT_Column column);

FXT_Key KeyMapper_DJMAX_8K(FXT_Column column);

FXT_Key KeyMapper_DJMAX_8KL(FXT_Column column);

FXT_Key KeyMapper_DJMAX_8KR(FXT_Column column);

FXT_Key KeyMapper_DJMAX_9K(FXT_Column column);

FXT_Key KeyMapper_BeatmaniaIIDX_4K(FXT_Column column);

FXT_Key KeyMapper_BeatmaniaIIDX_5K(FXT_Column column);

FXT_Key KeyMapper_BeatmaniaIIDX_6K(FXT_Column column);

FXT_Key KeyMapper_BeatmaniaIIDX_7K(FXT_Column column);

FXT_Key KeyMapper_BeatmaniaIIDX_8K(FXT_Column column);

FXT_Key KeyMapper_BeatmaniaIIDX_8KL(FXT_Column column);

FXT_Key KeyMapper_BeatmaniaIIDX_8KR(FXT_Column column);

const char *FXT_Key_ToString(FXT_Key key);
