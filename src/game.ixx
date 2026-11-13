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

	export struct ToVisualAndBeatmap
	{
		VisualColumnNo to_visual;
		BeatmapColumnNo to_beatmap;
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
		const vector<ToVisualAndBeatmap> data_;

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

	export typedef int32_t TapNoteDelta;

	export struct HoldNoteDelta
	{
		int32_t head, tail;
	};

	export struct NoteResult
	{
		GradeLevel grade_level;
		variant<TapNoteDelta, HoldNoteDelta> delta;
	};

	export struct ColumnIdle
	{
	};

	export struct ColumnEnd
	{
	};

	export typedef variant<ColumnIdle, ColumnEnd, NoteResult> ColumnResult;

	export class Column
	{
		const vector<Note>& notes_;
		const ModFlags& mod_flags_;

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

		ColumnResult Update_(const int32_t time_now_ms)
		{
			if (!focused_note_no_.has_value())
				return ColumnEnd{};

			const auto focused_note_no = focused_note_no_.value();
			const auto note = notes_[focused_note_no];
			const auto note_start = accumulated_time_ms_ + note.accumulated_time_ms;
			const bool key_is_down = !was_pressed_in_previous_update_ && is_pressing;
			const bool key_is_up = was_pressed_in_previous_update_ && !is_pressing;

			if (note.IsTap())
			{
			}
			else
			{
			}

			return ColumnIdle{};
		}

	public:
		bool is_pressing = false;

		explicit Column(const vector<Note>& notes, const ModFlags& mod_flags) : notes_(notes), mod_flags_(mod_flags)
		{
		}


		ColumnResult Update(const int32_t time_now_ms)
		{
			const auto result = Update_(time_now_ms);
			was_pressed_in_previous_update_ = is_pressing;
			return result;
		}
	};

	export enum class GameResult
	{
		Idle, Restart, End,
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
		Grade grade_ = {};
		TimingDistribution timing_distribution_ = {};

	public:
		Game(const Beatmap& beatmap, const Config& config, const ModFlags& mod_flags)
			: beatmap_(beatmap)
			  , config_(config)
			  , mod_flags_(mod_flags)
			  , tolerance_(Tolerance::FromOverallDifficulty(
				  config.custom_overall_difficulty().value_or(beatmap.overall_difficulty())
			  ))
			  , column_map_(ColumnMap::FromModFlags(beatmap.ColumnCount(), mod_flags))
			  , columns_(beatmap.columns()
				  | views::transform([&mod_flags](const vector<Note>& notes)
				  {
					  return Column(notes, mod_flags);
				  })
				  | ranges::to<vector>()
			  )
		{
		}

		[[nodiscard]] bool IsPressingColumn(const uint8_t column) const
		{
			return columns_[column].is_pressing;
		}

		void SetIsPressingColumn(const uint8_t column, const bool is_pressing)
		{
			columns_[column].is_pressing = is_pressing;
		}

		GameResult Update(const int32_t time_now_ms)
		{
			const auto results
				= columns_
				| views::transform([time_now_ms](auto x) { return x.Update(time_now_ms); })
				| ranges::to<vector>();

			if (ranges::all_of(results, [](const auto& x) { return holds_alternative<ColumnEnd>(x); }))
				return GameResult::End;

			for (const auto& result : results)
			{
				if (!holds_alternative<NoteResult>(result)) continue;

				const auto [grade_level, delta] = get<NoteResult>(result);

				if (grade_level == GradeLevel::Miss)
				{
					combo_ = 0;
				}
				else
				{
					combo_ += 1;
					max_combo_ = max(max_combo_, combo_);
				}

				if (holds_alternative<TapNoteDelta>(delta))
				{
					timing_distribution_.Log(get<TapNoteDelta>(delta));
				}
				else
				{
					const auto [head, tail] = get<HoldNoteDelta>(delta);
					timing_distribution_.Log(head);
					timing_distribution_.Log(tail);
				}
			}

			return GameResult::End;
		}
	};
}
