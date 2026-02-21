#include <stddef.h>
#include <fxTap/keymap.h>

FxTapKey KeyMapper_DJMAX_1K(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K5;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_2K(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K4;
	case FxTapColumn_2: return FxTapKey_K6;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_3K(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K4;
	case FxTapColumn_2: return FxTapKey_K5;
	case FxTapColumn_3: return FxTapKey_K6;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_4K(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K3;
	case FxTapColumn_2: return FxTapKey_K4;
	case FxTapColumn_3: return FxTapKey_K6;
	case FxTapColumn_4: return FxTapKey_K7;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_5K(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K3;
	case FxTapColumn_2: return FxTapKey_K4;
	case FxTapColumn_3: return FxTapKey_K5;
	case FxTapColumn_4: return FxTapKey_K6;
	case FxTapColumn_5: return FxTapKey_K7;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_6K(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K2;
	case FxTapColumn_2: return FxTapKey_K3;
	case FxTapColumn_3: return FxTapKey_K4;
	case FxTapColumn_4: return FxTapKey_K6;
	case FxTapColumn_5: return FxTapKey_K7;
	case FxTapColumn_6: return FxTapKey_K8;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_6KL(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_S1;
	case FxTapColumn_2: return FxTapKey_K3;
	case FxTapColumn_3: return FxTapKey_K4;
	case FxTapColumn_4: return FxTapKey_K5;
	case FxTapColumn_5: return FxTapKey_K6;
	case FxTapColumn_6: return FxTapKey_K7;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_6KR(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K3;
	case FxTapColumn_2: return FxTapKey_K4;
	case FxTapColumn_3: return FxTapKey_K5;
	case FxTapColumn_4: return FxTapKey_K6;
	case FxTapColumn_5: return FxTapKey_K7;
	case FxTapColumn_6: return FxTapKey_S1;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_7K(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K2;
	case FxTapColumn_2: return FxTapKey_K3;
	case FxTapColumn_3: return FxTapKey_K4;
	case FxTapColumn_4: return FxTapKey_K5;
	case FxTapColumn_5: return FxTapKey_K6;
	case FxTapColumn_6: return FxTapKey_K7;
	case FxTapColumn_7: return FxTapKey_K8;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_8K(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K1;
	case FxTapColumn_2: return FxTapKey_K2;
	case FxTapColumn_3: return FxTapKey_K3;
	case FxTapColumn_4: return FxTapKey_K4;
	case FxTapColumn_6: return FxTapKey_K6;
	case FxTapColumn_7: return FxTapKey_K7;
	case FxTapColumn_8: return FxTapKey_K8;
	case FxTapColumn_9: return FxTapKey_K9;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_8KL(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_S1;
	case FxTapColumn_2: return FxTapKey_K2;
	case FxTapColumn_3: return FxTapKey_K3;
	case FxTapColumn_4: return FxTapKey_K4;
	case FxTapColumn_5: return FxTapKey_K5;
	case FxTapColumn_6: return FxTapKey_K6;
	case FxTapColumn_7: return FxTapKey_K7;
	case FxTapColumn_8: return FxTapKey_K8;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_8KR(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K2;
	case FxTapColumn_2: return FxTapKey_K3;
	case FxTapColumn_3: return FxTapKey_K4;
	case FxTapColumn_4: return FxTapKey_K5;
	case FxTapColumn_5: return FxTapKey_K6;
	case FxTapColumn_6: return FxTapKey_K7;
	case FxTapColumn_7: return FxTapKey_K8;
	case FxTapColumn_8: return FxTapKey_S1;
	default: unreachable();
	}
}

FxTapKey KeyMapper_DJMAX_9K(const FxTapColumn column)
{
	switch (column)
	{
	case FxTapColumn_1: return FxTapKey_K1;
	case FxTapColumn_2: return FxTapKey_K2;
	case FxTapColumn_3: return FxTapKey_K3;
	case FxTapColumn_4: return FxTapKey_K4;
	case FxTapColumn_5: return FxTapKey_K5;
	case FxTapColumn_6: return FxTapKey_K6;
	case FxTapColumn_7: return FxTapKey_K7;
	case FxTapColumn_8: return FxTapKey_K8;
	case FxTapColumn_9: return FxTapKey_K9;
	default: unreachable();
	}
}

const char *FxTapKey_ToString(const FxTapKey key)
{
	switch (key)
	{
	case FxTapKey_K1: return "K1";
	case FxTapKey_K2: return "K2";
	case FxTapKey_K3: return "K3";
	case FxTapKey_K4: return "K4";
	case FxTapKey_K5: return "K5";
	case FxTapKey_K6: return "K6";
	case FxTapKey_K7: return "K7";
	case FxTapKey_K8: return "K8";
	case FxTapKey_K9: return "K9";
	case FxTapKey_S1: return "S1";
	default: unreachable();
	}
}
