#include <fxTap/keymap.h>

KeyMapper FXT_FetchKeyMapper(const FXT_Beatmap *beatmap, const FXT_Config *config)
{
	switch (config->KeyMapStyle)
	{
	case FXT_KeyMapStyle_DJMAX:
		switch (beatmap->ColumnCount)
		{
		case 1: return &KeyMapper_DJMAX_1K;
		case 2: return &KeyMapper_DJMAX_2K;
		case 3: return &KeyMapper_DJMAX_3K;
		case 4: return &KeyMapper_DJMAX_4K;
		case 5: return &KeyMapper_DJMAX_5K;
		case 6: return &KeyMapper_DJMAX_6K;
		case 7: return &KeyMapper_DJMAX_7K;
		case 8: return &KeyMapper_DJMAX_8K;
		case 9: return &KeyMapper_DJMAX_9K;
		default: return nullptr;
		}
	case FXT_KeyMapStyle_Beatmania:
		switch (beatmap->ColumnCount)
		{
		case 4: return &KeyMapper_Beatmania_4K;
		case 5: return &KeyMapper_Beatmania_5K;
		case 6: return &KeyMapper_Beatmania_6K;
		case 7: return &KeyMapper_Beatmania_7K;
		case 8: return &KeyMapper_Beatmania_8K;
		default: return nullptr;
		}
	default:
		return nullptr;
	}
}

const char *FXT_Key_ToString(const FXT_Key key)
{
	switch (key)
	{
	case FXT_Key_K1: return "K1";
	case FXT_Key_K2: return "K2";
	case FXT_Key_K3: return "K3";
	case FXT_Key_K4: return "K4";
	case FXT_Key_K5: return "K5";
	case FXT_Key_K6: return "K6";
	case FXT_Key_K7: return "K7";
	case FXT_Key_K8: return "K8";
	case FXT_Key_K9: return "K9";
	case FXT_Key_S1: return "S1";
	default: return nullptr;
	}
}
