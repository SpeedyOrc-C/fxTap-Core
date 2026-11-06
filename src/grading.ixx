module;

#include <cstdint>

export module fxtap:grading;

namespace fxtap::grading
{
	export struct Tolerance
	{
		std::int16_t PerfectMs;
		std::int16_t GreatMs;
		std::int16_t GoodMs;
		std::int16_t OkMs;
		std::int16_t MehMs;
		std::int16_t MissMs;

		static Tolerance FromOverallDifficulty(const double od)
		{
			const double od3 = 3 * od;

			return Tolerance{
				.PerfectMs = static_cast<int16_t>(24.0 - od),
				.GreatMs = static_cast<int16_t>(64.0 - od3),
				.GoodMs = static_cast<int16_t>(97.0 - od3),
				.OkMs = static_cast<int16_t>(127.0 - od3),
				.MehMs = static_cast<int16_t>(151.0 - od3),
				.MissMs = static_cast<int16_t>(188.0 - od3),
			};
		}
	};

	// Based on https://osu.ppy.sh/wiki/Gameplay/Accuracy#osu!mania
	export struct Grade
	{
		std::uint16_t Perfect;
		std::uint16_t Great;
		std::uint16_t Good;
		std::uint16_t Ok;
		std::uint16_t Meh;
		std::uint16_t Miss;

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
			const float max = this->Perfect;
			const float n300 = this->Great;
			const float n200 = this->Good;
			const float n100 = this->Ok;
			const float n50 = this->Meh;
			const float miss = this->Miss;

			return (maxMultiplier * max + 300 * n300 + 200 * n200 + 100 * n100 + 50 * n50)
				/ (maxMultiplier * (max + n300 + n200 + n100 + n50 + miss));
		}
	};
}
