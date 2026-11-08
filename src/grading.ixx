module;

#include <cmath>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cassert>

export module fxtap:grading;

import :base;

using namespace fxtap;

namespace fxtap::grading
{
	export struct Tolerance
	{
		std::int16_t perfect_ms;
		std::int16_t great_ms;
		std::int16_t good_ms;
		std::int16_t ok_ms;
		std::int16_t meh_ms;
		std::int16_t miss_ms;

		static Tolerance FromOverallDifficulty(const double od)
		{
			const double od3 = 3 * od;

			return Tolerance{
				.perfect_ms = static_cast<int16_t>(24.0 - od),
				.great_ms = static_cast<int16_t>(64.0 - od3),
				.good_ms = static_cast<int16_t>(97.0 - od3),
				.ok_ms = static_cast<int16_t>(127.0 - od3),
				.meh_ms = static_cast<int16_t>(151.0 - od3),
				.miss_ms = static_cast<int16_t>(188.0 - od3),
			};
		}
	};

	// Based on https://osu.ppy.sh/wiki/Gameplay/Accuracy#osu!mania
	export struct Grade
	{
		std::uint16_t perfect = 0;
		std::uint16_t great = 0;
		std::uint16_t good = 0;
		std::uint16_t ok = 0;
		std::uint16_t meh = 0;
		std::uint16_t miss = 0;

		[[nodiscard]] float Accuracy1() const
		{
			return Accuracy(300.0);
		}

		[[nodiscard]] float Accuracy2() const
		{
			return Accuracy(305.0);
		}

	private:
		[[nodiscard]] float Accuracy(const float maxMultiplier) const
		{
			const float max = this->perfect;
			const float n300 = this->great;
			const float n200 = this->good;
			const float n100 = this->ok;
			const float n50 = this->meh;
			const float miss = this->miss;

			return (maxMultiplier * max + 300 * n300 + 200 * n200 + 100 * n100 + 50 * n50)
				/ (maxMultiplier * (max + n300 + n200 + n100 + n50 + miss));
		}
	};

	export class TimingDistribution
	{
		const int16_t interval_count_ = 20;
		const int8_t interval_ms_ = 40;

		std::vector<uint16_t> data;

	public:
		TimingDistribution() : TimingDistribution(20, 10)
		{
		}

		TimingDistribution(const int16_t interval_count, const int8_t interval_ms)
			: interval_count_(interval_count), interval_ms_(interval_ms)
		{
			data.resize(interval_count * 2 + 1);
		}

		void Log(const int32_t timeMs)
		{
			const int32_t unboundedIndex
				= static_cast<int32_t>(std::round(static_cast<double>(timeMs) / interval_ms_))
				+ interval_count_;

			const auto index = std::clamp<int32_t>(unboundedIndex, 0, 40);

			assert(data[index] < UINT16_MAX);

			data[index] += 1;
		}
	};
}
