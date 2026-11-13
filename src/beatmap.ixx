module;

#include <cstdint>
#include <fstream>
#include <generator>
#include <memory>
#include <ranges>
#include <string>
#include <variant>
#include <vector>

export module fxtap:beatmap;

using namespace std;

namespace fxtap::beatmap
{
	export struct Note
	{
		/**
		 * @brief Time difference between this note and the previous one in the same column.
		 * @details Reason for not using absolute time is to save space.
		 */
		uint16_t accumulated_time_ms;
		uint16_t duration_ms;

		[[nodiscard]] inline bool IsTap() const
		{
			return duration_ms == 0;
		}

		[[nodiscard]] inline bool IsHold() const
		{
			return duration_ms > 0;
		}
	};

	export enum class LoadError
	{
		FileNotFound,
		BadHeader,
		FormatNotSupported,
		BadOverallDifficulty,
		BadTitleLength,
		BadTitle,
		BadArtistLength,
		BadArtist,
		BadVersionLength,
		BadVersion,
		BadColumnCount,
		BadColumnSizes,
		BadNote,
	};
}

namespace fxtap
{
	using namespace beatmap;

	export class Beatmap
	{
		const string title_;
		const string artist_;
		const string version_;
		const double overall_difficulty_;
		const vector<vector<Note>> columns_;

		Beatmap(
			string title,
			string artist,
			string version,
			const double overall_difficulty,
			vector<vector<Note>> columns
		)
			: title_(move(title))
			  , artist_(move(artist))
			  , version_(move(version))
			  , overall_difficulty_(overall_difficulty)
			  , columns_(move(columns))
		{
		}

	public:
		[[nodiscard]]
		inline auto title() const
		{
			return string_view(title_);
		}

		[[nodiscard]]
		inline auto artist() const
		{
			return string_view(artist_);
		}

		[[nodiscard]]
		inline auto version() const
		{
			return string_view(version_);
		}

		[[nodiscard]]
		inline auto overall_difficulty() const
		{
			return overall_difficulty_;
		}

		[[nodiscard]]
		inline const auto& columns() const
		{
			return columns_;
		}

		[[nodiscard]]
		inline auto ColumnCount() const
		{
			return columns_.size();
		}

		[[nodiscard]]
		inline auto NoteCountAt(const int column) const
		{
			return columns_[column].size();
		}

		[[nodiscard]]
		auto ColumnsNoteCount() const
		{
			return columns_ | views::transform([](const auto& column) { return column.size(); });
		}

		[[nodiscard]]
		static variant<LoadError, Beatmap> Load(const string& path)
		{
			ifstream file(path, ios::binary);
			return Load(file);
		}

		[[nodiscard]]
		static variant<LoadError, Beatmap> Load(ifstream& file)
		{
			if (!file.is_open())
				return LoadError::FileNotFound;

			array<char, 8> header{};
			file.read(header.data(), 8);

			if (file.bad())
				return LoadError::BadHeader;

			if (string(header.data(), 8) != "FXT@2602")
				return LoadError::FormatNotSupported;

			double overallDifficulty;
			file.read(reinterpret_cast<char*>(&overallDifficulty), sizeof(overallDifficulty));

			if (file.bad())
				return LoadError::BadOverallDifficulty;

			uint8_t titleLength;
			file.read(reinterpret_cast<char*>(&titleLength), sizeof(titleLength));

			if (file.bad())
				return LoadError::BadTitleLength;

			vector<char> titleBuffer(titleLength);
			file.read(titleBuffer.data(), titleLength);

			if (file.bad())
				return LoadError::BadTitle;

			string title(titleBuffer.begin(), titleBuffer.end());

			uint8_t artistLength;
			file.read(reinterpret_cast<char*>(&artistLength), sizeof(artistLength));

			if (file.bad())
				return LoadError::BadArtistLength;

			vector<char> artistBuffer(artistLength);
			file.read(artistBuffer.data(), artistLength);

			if (file.bad())
				return LoadError::BadArtist;

			string artist(artistBuffer.begin(), artistBuffer.end());

			uint8_t versionLength;
			file.read(reinterpret_cast<char*>(&versionLength), sizeof(versionLength));

			if (file.bad())
				return LoadError::BadVersionLength;

			vector<char> versionBuffer(versionLength);
			file.read(versionBuffer.data(), versionLength);

			if (file.bad())
				return LoadError::BadVersion;

			string version(versionBuffer.begin(), versionBuffer.end());

			uint8_t columnCount;
			file.read(reinterpret_cast<char*>(&columnCount), sizeof(columnCount));

			if (file.bad())
				return LoadError::BadColumnCount;

			vector<uint16_t> columnSizes(columnCount);
			file.read(reinterpret_cast<char*>(columnSizes.data()),
			          static_cast<long long>(sizeof(uint16_t) * columnSizes.size()));

			if (file.bad())
				return LoadError::BadColumnSizes;

			vector<vector<Note>> columns(columnCount);

			for (const auto& [i, size] : views::enumerate(columnSizes))
			{
				columns[i].resize(size);
				file.read(reinterpret_cast<char*>(columns[i].data()), static_cast<long long>(size * sizeof(Note)));

				if (file.bad())
					return LoadError::BadNote;
			}

			return Beatmap(move(title), move(artist), move(version), overallDifficulty, move(columns));
		}
	};
}
