module;

#include <algorithm>
#include <cstdint>
#include <random>
#include <ranges>

export module fxtap:game;

import :base;
import :beatmap;
import :config;
import :grading;
import :mod;

using namespace std;
using namespace fxtap;
using namespace fxtap::beatmap;
using namespace fxtap::grading;
using namespace fxtap::config;

namespace fxtap::game
{
	/*
	Difference between actual tap time and note time.
	Negative: Early
	Positive: Late
	*/
	export typedef int32_t DeltaMs;

	export struct PlayNoteLog
	{
		GradeLevel level{};
		DeltaMs delta_ms{};
	};

	/**
	 * @brief Map a column number between visual and beatmap one.
	 * @details Mods like mirror and random will reorder the columns.
	 * Instead of changing the beatmap, we can map the column number of user input
	 * to match the beatmap.
	 */
	export class ColumnMap
	{
		// A bi-directional map.
		const vector<struct ToVisualAndBeatmap
		{
			VisualColumnNo to_visual;
			BeatmapColumnNo to_beatmap;
		}> data_;

	public:
		explicit ColumnMap(const vector<ToVisualAndBeatmap>& data) : data_(data)
		{
		}

		[[nodiscard]]
		VisualColumnNo VisualFromBeatmap(const BeatmapColumnNo beatmap) const
		{
			return data_[static_cast<int>(beatmap)].to_visual;
		}

		[[nodiscard]]
		BeatmapColumnNo BeatmapFromVisual(const VisualColumnNo visual) const
		{
			return data_[static_cast<int>(visual)].to_beatmap;
		}

		static ColumnMap FromModFlags(const size_t size, const ModFlags& flags)
		{
			if (flags.mirror()) return Mirror(size);
			if (flags.random()) return Random(size);
			return Id(size);
		}

		static ColumnMap Id(const size_t size)
		{
			return ColumnMap(
				views::iota(static_cast<uint8_t>(0), size)
				| views::transform([](const uint8_t i)
				{
					return ToVisualAndBeatmap{
						.to_visual = static_cast<ColumnNo>(i),
						.to_beatmap = static_cast<ColumnNo>(i)
					};
				})
				| ranges::to<vector>()
			);
		}

		static ColumnMap Mirror(const size_t size)
		{
			return ColumnMap(
				views::iota(static_cast<uint8_t>(0), size)
				| views::transform([size](const uint8_t i)
				{
					return ToVisualAndBeatmap{
						.to_visual = static_cast<ColumnNo>(size - i - 1),
						.to_beatmap = static_cast<ColumnNo>(size - i - 1)
					};
				})
				| ranges::to<vector>()
			);
		}

		static ColumnMap Random(const size_t size)
		{
			random_device rd;
			mt19937 gen(rd());
			auto data
				= views::iota(static_cast<uint8_t>(0), size)
				| ranges::to<vector>();
			ranges::shuffle(data, gen);

			vector<ToVisualAndBeatmap> to_visual_and_beatmap;

			for (size_t i = 0; i < size; i += 1)
			{
				to_visual_and_beatmap[i].to_visual = static_cast<ColumnNo>(data[i]);
				to_visual_and_beatmap[data[i]].to_beatmap = static_cast<ColumnNo>(i);
			}

			return ColumnMap(to_visual_and_beatmap);
		}
	};

	export class Column
	{
		const vector<Note>& notes_;

		int32_t accumulated_time_ms_ = 0;
		optional<int32_t> focused_note_no_ = notes_.empty() ? nullopt : optional(0);
		bool was_pressed_in_previous_update_ = false;

		struct
		{
			bool head_is_valid_ = false;
			bool tail_is_valid_ = false;
			int32_t head_delta_ = 0;
			int32_t tail_delta_ = 0;
		} hold_state_;

	public:
		bool is_pressing = false;

		explicit Column(const vector<Note>& notes) : notes_(notes)
		{
		}

		void Update()
		{
			was_pressed_in_previous_update_ = is_pressing;
		}
	};

	export class Game
	{
		const Beatmap& beatmap_;
		const Config& config_;
		const ModFlags& mod_flags_;
		const Tolerance& tolerance_;
		const ColumnMap column_map_;

		vector<Column> columns_;
		unsigned combo_ = 0;
		unsigned max_combo_ = 0;
		Grade grade_();
		TimingDistribution timing_distribution_();

	public:
		Game(const Beatmap& beatmap, const Config& config, const ModFlags& mod_flags)
			: beatmap_(beatmap)
			  , config_(config)
			  , mod_flags_(mod_flags)
			  , tolerance_(Tolerance::FromOverallDifficulty(
				  config.custom_overall_difficulty().value_or(beatmap.overall_difficulty())
			  ))
			  , column_map_(ColumnMap::FromModFlags(beatmap.ColumnCount(), mod_flags))
		{
		}

		void Update()
		{
		}
	};
}
