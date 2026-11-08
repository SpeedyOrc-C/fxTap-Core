#include <cassert>
#include <print>
#include <variant>

import fxtap;
using namespace std;
using namespace fxtap;

static void TestLoadBeatmap()
{
	const auto errorOrBeatmap = Beatmap::Load("dream.fxt");
	assert(holds_alternative<Beatmap>(errorOrBeatmap));
	const auto& beatmap = get<Beatmap>(errorOrBeatmap);

	println("Beatmap loaded.");
	println("Title: {}", beatmap.title());
	println("Artist: {}", beatmap.artist());
	println("Version: {}", beatmap.version());
	print("There're {} column(s) with sizes: ", beatmap.ColumnCount());
	for (const auto size : beatmap.ColumnsNoteCount())
		print("{} ", size);
	println();
}

static void TestConfig()
{
	using namespace config;

	constexpr Config config;

	const auto saveError = config.Save();
	assert(!saveError.has_value());
	println("Config saved.");

	const auto errorOrConfig = Config::Load();
	assert(holds_alternative<Config>(errorOrConfig));
	println("Config loaded.");
}

int main()
{
	TestLoadBeatmap();
	TestConfig();
	return 0;
}
