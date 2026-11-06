#include <cassert>
#include <print>
#include <variant>

import fxtap;
using namespace std;
using namespace fxtap;

static void TestLoadBeatmap()
{
	using namespace beatmap;

	const auto errorOrBeatmap = Beatmap::Load("dream.fxt");
	assert(holds_alternative<Beatmap>(errorOrBeatmap));
	const auto& beatmap = get<Beatmap>(errorOrBeatmap);

	println("Beatmap loaded.");
	println("Title: {}", beatmap.Title());
	println("Artist: {}", beatmap.Artist());
	println("Version: {}", beatmap.Version());
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
	assert(holds_alternative<config::Config>(errorOrConfig));
	println("Config loaded.");
}

int main()
{
	TestLoadBeatmap();
	TestConfig();
	return 0;
}
