module;

#include <string_view>

export module fxtap:keymap;

namespace fxtap::keymap
{
	export enum class Column
	{
		C1, C2, C3, C4, C5, C6, C7, C8, C9
	};

	export enum class Key
	{
		Unset, K1, K2, K3, K4, K5, K6, K7, K8, K9, S1,
	};

	export std::string_view ShowKey(const Key key)
	{
		switch (key)
		{
		case Key::K1: return "K1";
		case Key::K2: return "K2";
		case Key::K3: return "K3";
		case Key::K4: return "K4";
		case Key::K5: return "K5";
		case Key::K6: return "K6";
		case Key::K7: return "K7";
		case Key::K8: return "K8";
		case Key::K9: return "K9";
		case Key::S1: return "S1";
		default: return "Unset";
		}
	}

	export struct KeyMapper
	{
		static inline Key Beatmania_4K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K1;
			case Column::C2: return Key::K2;
			case Column::C3: return Key::K4;
			case Column::C4: return Key::K5;
			default: return Key::Unset;
			}
		}

		static inline Key Beatmania_5K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K1;
			case Column::C2: return Key::K2;
			case Column::C3: return Key::K3;
			case Column::C4: return Key::K4;
			case Column::C5: return Key::K5;
			default: return Key::Unset;
			}
		}

		static inline Key Beatmania_6K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K1;
			case Column::C2: return Key::K2;
			case Column::C3: return Key::K3;
			case Column::C4: return Key::K4;
			case Column::C5: return Key::K5;
			case Column::C6: return Key::K6;
			default: return Key::Unset;
			}
		}

		static inline Key Beatmania_7K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K1;
			case Column::C2: return Key::K2;
			case Column::C3: return Key::K3;
			case Column::C4: return Key::K4;
			case Column::C5: return Key::K5;
			case Column::C6: return Key::K6;
			case Column::C7: return Key::K7;
			default: return Key::Unset;
			}
		}

		static inline Key Beatmania_8K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K1;
			case Column::C2: return Key::K2;
			case Column::C3: return Key::K3;
			case Column::C4: return Key::K4;
			case Column::C5: return Key::K5;
			case Column::C6: return Key::K6;
			case Column::C7: return Key::K7;
			case Column::C8: return Key::K8;
			default: return Key::Unset;
			}
		}

		static inline Key Beatmania_8KL(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::S1;
			case Column::C2: return Key::K1;
			case Column::C3: return Key::K2;
			case Column::C4: return Key::K3;
			case Column::C5: return Key::K4;
			case Column::C6: return Key::K5;
			case Column::C7: return Key::K6;
			case Column::C8: return Key::K7;
			default: return Key::Unset;
			}
		}

		static inline Key Beatmania_8KR(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K1;
			case Column::C2: return Key::K2;
			case Column::C3: return Key::K3;
			case Column::C4: return Key::K4;
			case Column::C5: return Key::K5;
			case Column::C6: return Key::K6;
			case Column::C7: return Key::K7;
			case Column::C8: return Key::S1;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_1K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K5;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_2K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K4;
			case Column::C2: return Key::K6;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_3K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K4;
			case Column::C2: return Key::K5;
			case Column::C3: return Key::K6;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_4K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K3;
			case Column::C2: return Key::K4;
			case Column::C3: return Key::K6;
			case Column::C4: return Key::K7;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_5K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K3;
			case Column::C2: return Key::K4;
			case Column::C3: return Key::K5;
			case Column::C4: return Key::K6;
			case Column::C5: return Key::K7;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_6K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K2;
			case Column::C2: return Key::K3;
			case Column::C3: return Key::K4;
			case Column::C4: return Key::K6;
			case Column::C5: return Key::K7;
			case Column::C6: return Key::K8;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_6KL(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::S1;
			case Column::C2: return Key::K3;
			case Column::C3: return Key::K4;
			case Column::C4: return Key::K5;
			case Column::C5: return Key::K6;
			case Column::C6: return Key::K7;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_6KR(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K3;
			case Column::C2: return Key::K4;
			case Column::C3: return Key::K5;
			case Column::C4: return Key::K6;
			case Column::C5: return Key::K7;
			case Column::C6: return Key::S1;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_7K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K2;
			case Column::C2: return Key::K3;
			case Column::C3: return Key::K4;
			case Column::C4: return Key::K5;
			case Column::C5: return Key::K6;
			case Column::C6: return Key::K7;
			case Column::C7: return Key::K8;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_8K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K1;
			case Column::C2: return Key::K2;
			case Column::C3: return Key::K3;
			case Column::C4: return Key::K4;
			case Column::C6: return Key::K6;
			case Column::C7: return Key::K7;
			case Column::C8: return Key::K8;
			case Column::C9: return Key::K9;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_8KL(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::S1;
			case Column::C2: return Key::K2;
			case Column::C3: return Key::K3;
			case Column::C4: return Key::K4;
			case Column::C5: return Key::K5;
			case Column::C6: return Key::K6;
			case Column::C7: return Key::K7;
			case Column::C8: return Key::K8;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_8KR(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K2;
			case Column::C2: return Key::K3;
			case Column::C3: return Key::K4;
			case Column::C4: return Key::K5;
			case Column::C5: return Key::K6;
			case Column::C6: return Key::K7;
			case Column::C7: return Key::K8;
			case Column::C8: return Key::S1;
			default: return Key::Unset;
			}
		}

		static inline Key DJMAX_9K(const Column column)
		{
			switch (column)
			{
			case Column::C1: return Key::K1;
			case Column::C2: return Key::K2;
			case Column::C3: return Key::K3;
			case Column::C4: return Key::K4;
			case Column::C5: return Key::K5;
			case Column::C6: return Key::K6;
			case Column::C7: return Key::K7;
			case Column::C8: return Key::K8;
			case Column::C9: return Key::K9;
			default: return Key::Unset;
			}
		}
	};

	export typedef Key (*KeyFromColumn)(Column);
}
