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

namespace fxtap::beatmap
{
	export enum class FindError
	{
		MallocFailed,
		FxtapFolderNotFound,
		BadFile,
	};

	export struct Note
	{
		uint16_t AccumulatedStartTime;
		uint16_t Duration;

		[[nodiscard]] inline bool IsTap() const
		{
			return Duration == 0U;
		}

		[[nodiscard]] inline bool IsHold() const
		{
			return Duration > 0U;
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

	export class Beatmap
	{
		std::string title_;
		std::string artist_;
		std::string version_;
		double overall_difficulty_;
		std::vector<std::vector<Note>> columns_;

		Beatmap(
			std::string title,
			std::string artist,
			std::string version,
			const double overall_difficulty,
			std::vector<std::vector<Note>> columns
		)
			: title_(std::move(title))
			  , artist_(std::move(artist))
			  , version_(std::move(version))
			  , overall_difficulty_(overall_difficulty)
			  , columns_(std::move(columns))
		{
		}

	public:
		[[nodiscard]]
		inline auto Title() const
		{
			return std::string_view(title_);
		}

		[[nodiscard]]
		inline auto Artist() const
		{
			return std::string_view(artist_);
		}

		[[nodiscard]]
		inline auto Version() const
		{
			return std::string_view(version_);
		}

		[[nodiscard]]
		inline auto OverallDifficulty() const
		{
			return overall_difficulty_;
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
			return columns_ | std::views::transform([](const auto& column) { return column.size(); });
		}

		[[nodiscard]]
		static std::variant<LoadError, Beatmap> Load(const std::string& path)
		{
			std::ifstream file(path, std::ios::binary);
			return Load(file);
		}

		[[nodiscard]]
		static std::variant<LoadError, Beatmap> Load(std::ifstream& file)
		{
			if (!file.is_open())
				return LoadError::FileNotFound;

			std::array<char, 8> header{};
			file.read(header.data(), 8);

			if (file.bad())
				return LoadError::BadHeader;

			if (std::string(header.data(), 8) != "FXT@2602")
				return LoadError::FormatNotSupported;

			double overallDifficulty;
			file.read(reinterpret_cast<char*>(&overallDifficulty), sizeof(overallDifficulty));

			if (file.bad())
				return LoadError::BadOverallDifficulty;

			uint8_t titleLength;
			file.read(reinterpret_cast<char*>(&titleLength), sizeof(titleLength));

			if (file.bad())
				return LoadError::BadTitleLength;

			std::vector<char> titleBuffer(titleLength);
			file.read(titleBuffer.data(), titleLength);

			if (file.bad())
				return LoadError::BadTitle;

			std::string title(titleBuffer.begin(), titleBuffer.end());

			uint8_t artistLength;
			file.read(reinterpret_cast<char*>(&artistLength), sizeof(artistLength));

			if (file.bad())
				return LoadError::BadArtistLength;

			std::vector<char> artistBuffer(artistLength);
			file.read(artistBuffer.data(), artistLength);

			if (file.bad())
				return LoadError::BadArtist;

			std::string artist(artistBuffer.begin(), artistBuffer.end());

			uint8_t versionLength;
			file.read(reinterpret_cast<char*>(&versionLength), sizeof(versionLength));

			if (file.bad())
				return LoadError::BadVersionLength;

			std::vector<char> versionBuffer(versionLength);
			file.read(versionBuffer.data(), versionLength);

			if (file.bad())
				return LoadError::BadVersion;

			std::string version(versionBuffer.begin(), versionBuffer.end());

			uint8_t columnCount;
			file.read(reinterpret_cast<char*>(&columnCount), sizeof(columnCount));

			if (file.bad())
				return LoadError::BadColumnCount;

			std::vector<uint16_t> columnSizes(columnCount);
			file.read(reinterpret_cast<char*>(columnSizes.data()),
			          static_cast<long long>(sizeof(uint16_t) * columnSizes.size()));

			if (file.bad())
				return LoadError::BadColumnSizes;

			std::vector<std::vector<Note>> columns(columnCount);

			for (const auto& [i, size] : std::views::enumerate(columnSizes))
			{
				columns[i].resize(size);
				file.read(reinterpret_cast<char*>(columns[i].data()), static_cast<long long>(size * sizeof(Note)));

				if (file.bad())
					return LoadError::BadNote;
			}

			return Beatmap(std::move(title), std::move(artist), std::move(version), overallDifficulty, std::move(columns));
		}
	};
}
