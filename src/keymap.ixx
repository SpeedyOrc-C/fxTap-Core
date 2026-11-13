module;

#include <optional>

export module fxtap:keymap;

import :base;
import :config;

using namespace std;
using namespace fxtap;
using namespace fxtap::config;

namespace fxtap::keymap
{
	export struct KeyMapper
	{
		static inline optional<Key> Beatmania_4K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K1;
			case ColumnNo::C2: return Key::K2;
			case ColumnNo::C3: return Key::K4;
			case ColumnNo::C4: return Key::K5;
			default: return nullopt;
			}
		}

		static inline optional<Key> Beatmania_5K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K1;
			case ColumnNo::C2: return Key::K2;
			case ColumnNo::C3: return Key::K3;
			case ColumnNo::C4: return Key::K4;
			case ColumnNo::C5: return Key::K5;
			default: return nullopt;
			}
		}

		static inline optional<Key> Beatmania_6K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K1;
			case ColumnNo::C2: return Key::K2;
			case ColumnNo::C3: return Key::K3;
			case ColumnNo::C4: return Key::K4;
			case ColumnNo::C5: return Key::K5;
			case ColumnNo::C6: return Key::K6;
			default: return nullopt;
			}
		}

		static inline optional<Key> Beatmania_7K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K1;
			case ColumnNo::C2: return Key::K2;
			case ColumnNo::C3: return Key::K3;
			case ColumnNo::C4: return Key::K4;
			case ColumnNo::C5: return Key::K5;
			case ColumnNo::C6: return Key::K6;
			case ColumnNo::C7: return Key::K7;
			default: return nullopt;
			}
		}

		static inline optional<Key> Beatmania_8K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K1;
			case ColumnNo::C2: return Key::K2;
			case ColumnNo::C3: return Key::K3;
			case ColumnNo::C4: return Key::K4;
			case ColumnNo::C5: return Key::K5;
			case ColumnNo::C6: return Key::K6;
			case ColumnNo::C7: return Key::K7;
			case ColumnNo::C8: return Key::K8;
			default: return nullopt;
			}
		}

		static inline optional<Key> Beatmania_8KL(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::S1;
			case ColumnNo::C2: return Key::K1;
			case ColumnNo::C3: return Key::K2;
			case ColumnNo::C4: return Key::K3;
			case ColumnNo::C5: return Key::K4;
			case ColumnNo::C6: return Key::K5;
			case ColumnNo::C7: return Key::K6;
			case ColumnNo::C8: return Key::K7;
			default: return nullopt;
			}
		}

		static inline optional<Key> Beatmania_8KR(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K1;
			case ColumnNo::C2: return Key::K2;
			case ColumnNo::C3: return Key::K3;
			case ColumnNo::C4: return Key::K4;
			case ColumnNo::C5: return Key::K5;
			case ColumnNo::C6: return Key::K6;
			case ColumnNo::C7: return Key::K7;
			case ColumnNo::C8: return Key::S1;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_1K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K5;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_2K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K4;
			case ColumnNo::C2: return Key::K6;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_3K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K4;
			case ColumnNo::C2: return Key::K5;
			case ColumnNo::C3: return Key::K6;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_4K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K3;
			case ColumnNo::C2: return Key::K4;
			case ColumnNo::C3: return Key::K6;
			case ColumnNo::C4: return Key::K7;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_5K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K3;
			case ColumnNo::C2: return Key::K4;
			case ColumnNo::C3: return Key::K5;
			case ColumnNo::C4: return Key::K6;
			case ColumnNo::C5: return Key::K7;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_6K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K2;
			case ColumnNo::C2: return Key::K3;
			case ColumnNo::C3: return Key::K4;
			case ColumnNo::C4: return Key::K6;
			case ColumnNo::C5: return Key::K7;
			case ColumnNo::C6: return Key::K8;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_6KL(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::S1;
			case ColumnNo::C2: return Key::K3;
			case ColumnNo::C3: return Key::K4;
			case ColumnNo::C4: return Key::K5;
			case ColumnNo::C5: return Key::K6;
			case ColumnNo::C6: return Key::K7;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_6KR(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K3;
			case ColumnNo::C2: return Key::K4;
			case ColumnNo::C3: return Key::K5;
			case ColumnNo::C4: return Key::K6;
			case ColumnNo::C5: return Key::K7;
			case ColumnNo::C6: return Key::S1;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_7K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K2;
			case ColumnNo::C2: return Key::K3;
			case ColumnNo::C3: return Key::K4;
			case ColumnNo::C4: return Key::K5;
			case ColumnNo::C5: return Key::K6;
			case ColumnNo::C6: return Key::K7;
			case ColumnNo::C7: return Key::K8;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_8K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K1;
			case ColumnNo::C2: return Key::K2;
			case ColumnNo::C3: return Key::K3;
			case ColumnNo::C4: return Key::K4;
			case ColumnNo::C6: return Key::K6;
			case ColumnNo::C7: return Key::K7;
			case ColumnNo::C8: return Key::K8;
			case ColumnNo::C9: return Key::K9;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_8KL(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::S1;
			case ColumnNo::C2: return Key::K2;
			case ColumnNo::C3: return Key::K3;
			case ColumnNo::C4: return Key::K4;
			case ColumnNo::C5: return Key::K5;
			case ColumnNo::C6: return Key::K6;
			case ColumnNo::C7: return Key::K7;
			case ColumnNo::C8: return Key::K8;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_8KR(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K2;
			case ColumnNo::C2: return Key::K3;
			case ColumnNo::C3: return Key::K4;
			case ColumnNo::C4: return Key::K5;
			case ColumnNo::C5: return Key::K6;
			case ColumnNo::C6: return Key::K7;
			case ColumnNo::C7: return Key::K8;
			case ColumnNo::C8: return Key::S1;
			default: return nullopt;
			}
		}

		static inline optional<Key> DJMAX_9K(const VisualColumnNo column)
		{
			switch (column)
			{
			case ColumnNo::C1: return Key::K1;
			case ColumnNo::C2: return Key::K2;
			case ColumnNo::C3: return Key::K3;
			case ColumnNo::C4: return Key::K4;
			case ColumnNo::C5: return Key::K5;
			case ColumnNo::C6: return Key::K6;
			case ColumnNo::C7: return Key::K7;
			case ColumnNo::C8: return Key::K8;
			case ColumnNo::C9: return Key::K9;
			default: return nullopt;
			}
		}
	};

	export typedef optional<Key> (*KeyFromColumn)(ColumnNo);

	export optional<KeyFromColumn>
	FetchKeyMapper(const size_t column_count, const KeyMapStyle key_map_style)
	{
		switch (key_map_style)
		{
		case KeyMapStyle::DJMAX:
			switch (column_count)
			{
			case 1: return KeyMapper::DJMAX_1K;
			case 2: return KeyMapper::DJMAX_2K;
			case 3: return KeyMapper::DJMAX_3K;
			case 4: return KeyMapper::DJMAX_4K;
			case 5: return KeyMapper::DJMAX_5K;
			case 6: return KeyMapper::DJMAX_6K;
			case 7: return KeyMapper::DJMAX_7K;
			case 8: return KeyMapper::DJMAX_8K;
			case 9: return KeyMapper::DJMAX_9K;
			default: return nullopt;
			}
		case KeyMapStyle::Beatmania:
			switch (column_count)
			{
			case 4: return KeyMapper::Beatmania_4K;
			case 5: return KeyMapper::Beatmania_5K;
			case 6: return KeyMapper::Beatmania_6K;
			case 7: return KeyMapper::Beatmania_7K;
			case 8: return KeyMapper::Beatmania_8K;
			default: return nullopt;
			}
		default:
			return nullopt;
		}
	}
}
