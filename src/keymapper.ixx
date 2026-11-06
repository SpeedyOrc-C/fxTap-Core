module;

#include <optional>

export module fxtap:keymapper;

import :config;
import :keymap;

using namespace std;
using namespace fxtap::config;
using namespace fxtap::keymap;

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
