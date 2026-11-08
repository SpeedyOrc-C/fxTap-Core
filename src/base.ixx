module;

#include <string>

export module fxtap:base;

using namespace std;

namespace fxtap
{
	export enum class ColumnNo
	{
		C1, C2, C3, C4, C5, C6, C7, C8, C9, C10
	};

	/**
	 * @brief The original column number in the beatmap.
	 */
	export typedef ColumnNo BeatmapColumnNo;

	/**
	 * @brief The column number shown on the screen, and from user input.
	 */
	export typedef ColumnNo VisualColumnNo;

	export enum class Key
	{
		K1, K2, K3, K4, K5, K6, K7, K8, K9, S1,
	};

	export enum class GradeLevel
	{
		Miss, Meh, Ok, Good, Great, Perfect,
	};

	export string_view ShowKey(const Key key)
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
		default: throw exception();
		}
	}
}
