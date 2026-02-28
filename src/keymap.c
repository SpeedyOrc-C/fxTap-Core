#include <fxTap/keymap.h>

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
