#include <stddef.h>
#include <fxTap/keymap.h>

FXT_Key KeyMapper_DJMAX_1K(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K5;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_2K(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K4;
	case FXT_Column_2: return FXT_Key_K6;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_3K(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K4;
	case FXT_Column_2: return FXT_Key_K5;
	case FXT_Column_3: return FXT_Key_K6;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_4K(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K3;
	case FXT_Column_2: return FXT_Key_K4;
	case FXT_Column_3: return FXT_Key_K6;
	case FXT_Column_4: return FXT_Key_K7;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_5K(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K3;
	case FXT_Column_2: return FXT_Key_K4;
	case FXT_Column_3: return FXT_Key_K5;
	case FXT_Column_4: return FXT_Key_K6;
	case FXT_Column_5: return FXT_Key_K7;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_6K(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K2;
	case FXT_Column_2: return FXT_Key_K3;
	case FXT_Column_3: return FXT_Key_K4;
	case FXT_Column_4: return FXT_Key_K6;
	case FXT_Column_5: return FXT_Key_K7;
	case FXT_Column_6: return FXT_Key_K8;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_6KL(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_S1;
	case FXT_Column_2: return FXT_Key_K3;
	case FXT_Column_3: return FXT_Key_K4;
	case FXT_Column_4: return FXT_Key_K5;
	case FXT_Column_5: return FXT_Key_K6;
	case FXT_Column_6: return FXT_Key_K7;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_6KR(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K3;
	case FXT_Column_2: return FXT_Key_K4;
	case FXT_Column_3: return FXT_Key_K5;
	case FXT_Column_4: return FXT_Key_K6;
	case FXT_Column_5: return FXT_Key_K7;
	case FXT_Column_6: return FXT_Key_S1;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_7K(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K2;
	case FXT_Column_2: return FXT_Key_K3;
	case FXT_Column_3: return FXT_Key_K4;
	case FXT_Column_4: return FXT_Key_K5;
	case FXT_Column_5: return FXT_Key_K6;
	case FXT_Column_6: return FXT_Key_K7;
	case FXT_Column_7: return FXT_Key_K8;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_8K(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K1;
	case FXT_Column_2: return FXT_Key_K2;
	case FXT_Column_3: return FXT_Key_K3;
	case FXT_Column_4: return FXT_Key_K4;
	case FXT_Column_6: return FXT_Key_K6;
	case FXT_Column_7: return FXT_Key_K7;
	case FXT_Column_8: return FXT_Key_K8;
	case FXT_Column_9: return FXT_Key_K9;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_8KL(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_S1;
	case FXT_Column_2: return FXT_Key_K2;
	case FXT_Column_3: return FXT_Key_K3;
	case FXT_Column_4: return FXT_Key_K4;
	case FXT_Column_5: return FXT_Key_K5;
	case FXT_Column_6: return FXT_Key_K6;
	case FXT_Column_7: return FXT_Key_K7;
	case FXT_Column_8: return FXT_Key_K8;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_8KR(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K2;
	case FXT_Column_2: return FXT_Key_K3;
	case FXT_Column_3: return FXT_Key_K4;
	case FXT_Column_4: return FXT_Key_K5;
	case FXT_Column_5: return FXT_Key_K6;
	case FXT_Column_6: return FXT_Key_K7;
	case FXT_Column_7: return FXT_Key_K8;
	case FXT_Column_8: return FXT_Key_S1;
	default: unreachable();
	}
}

FXT_Key KeyMapper_DJMAX_9K(const FXT_Column column)
{
	switch (column)
	{
	case FXT_Column_1: return FXT_Key_K1;
	case FXT_Column_2: return FXT_Key_K2;
	case FXT_Column_3: return FXT_Key_K3;
	case FXT_Column_4: return FXT_Key_K4;
	case FXT_Column_5: return FXT_Key_K5;
	case FXT_Column_6: return FXT_Key_K6;
	case FXT_Column_7: return FXT_Key_K7;
	case FXT_Column_8: return FXT_Key_K8;
	case FXT_Column_9: return FXT_Key_K9;
	default: unreachable();
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
	default: unreachable();
	}
}
