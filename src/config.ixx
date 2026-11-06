module;

#include <cstdint>
#include <fstream>
#include <optional>
#include <string>
#include <variant>

export module fxtap:config;

namespace fxtap::config
{
	export enum class LoadError
	{
		FileNotFound,
		BadContent,
	};

	export enum class SaveError
	{
		FileNotFound,
		BadWrite,
	};

	export enum class Language
	{
		EnUs, ZhCn,
	};

	export enum class KeyMapStyle
	{
		DJMAX, Beatmania, CoOp
	};

	export const std::string DefaultPath = "fxTap.cfg";

	export struct Config
	{
		uint8_t physical_key_of_fx_tap_key[10] = {};
		int16_t notes_falling_time_ms = 1000;
		std::optional<uint8_t> custom_overall_difficulty10 = std::nullopt;
		uint8_t column_width = 10;
		uint8_t tap_note_height = 4;
		Language language : 8 = Language::EnUs;
		KeyMapStyle key_map_style : 8 = KeyMapStyle::Beatmania;

		[[nodiscard]]
		static std::variant<LoadError, Config> Load()
		{
			return Load(DefaultPath);
		}

		[[nodiscard]]
		static std::variant<LoadError, Config> Load(const std::string& path)
		{
			std::ifstream file(path, std::ios::binary);
			return Load(file);
		}

		[[nodiscard]]
		static std::variant<LoadError, Config> Load(std::ifstream& file)
		{
			if (!file.is_open())
				return LoadError::FileNotFound;

			Config config{};

			file.read(reinterpret_cast<char*>(&config), sizeof(Config));

			if (file.bad())
				return LoadError::BadContent;

			return config;
		}

		[[nodiscard]]
		std::optional<SaveError> Save() const
		{
			return Save(DefaultPath);
		}

		[[nodiscard]]
		std::optional<SaveError> Save(const std::string& path) const
		{
			std::ofstream file(path, std::ios::binary);
			return Save(file);
		}

		[[nodiscard]]
		std::optional<SaveError> Save(std::ofstream& file) const
		{
			if (!file.is_open())
				return SaveError::FileNotFound;

			file.write(reinterpret_cast<const char*>(this), sizeof(Config));

			if (file.bad())
				return SaveError::BadWrite;

			return std::nullopt;
		}
	};
}
