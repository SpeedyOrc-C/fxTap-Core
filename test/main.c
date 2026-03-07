#include <stb_ds.h>
#include <stdio.h>
#include <fxTap/beatmap.h>
#include <fxTap/config.h>
#include <fxTap/database.h>
#include <fxTap/game.h>
#include <fxTap/keymap.h>
#include <fxTap/render.h>

bool Run(bool (*test)(), const char *name)
{
	printf("=== %s ===\n", name);
	const int result = test();
	puts(result ? "" : "[ERROR]\n");
	return result;
}

bool Test_FileLoading()
{
	FXT_Beatmap beatmap;
	const FXT_BeatmapError error = FXT_Beatmap_Load(&beatmap, "dream.fxt");

	if (error)
	{
		printf("Failed to load the beatmap. Error: %d\n", error);
		return false;
	}

	FXT_Game game;
	FXT_Game_Init(&game, &beatmap);

	auto const startTime = FXT_Game_FirstNoteStartTime(&game);
	auto const endTime = FXT_Game_LastNoteEndTime(&game);

	printf("Title: %s\n", beatmap.Title);
	printf("Artist: %s\n", beatmap.Artist);
	printf("Overall Difficulty: %lf\n", beatmap.OverallDifficulty);
	printf("Number of Columns: %i\n", beatmap.ColumnCount);
	printf("Start/End Time: %i/%i\n", startTime, endTime);

	FXT_Beatmap_FreeInner(&beatmap);

	FXT_Config config;

	const FXT_Config_Error configError = FXT_Config_Load(&config);

	if (configError != 0)
	{
		printf("Failed to load the config, and failed to create it.\n");
		return false;
	}

	config.PhysicalKeyOfFxTapKey[FXT_Key_K4] = 'X';

	if (FXT_Config_Save(&config))
	{
		printf("Failed to save the config.\n");
		return false;
	}

	return true;
}

bool Test_Tap()
{
	uint16_t columnSize[] = {2, 0, 0, 0, 0, 0, 0, 0};

	const FXT_Beatmap beatmap = {
		.ColumnSize = columnSize,
		.ColumnCount = 1,
		.OverallDifficulty = 0,
		.Notes = (FXT_Note[2]){
			{.AccumulatedStartTime = 1000, .Duration = 0},
			{.AccumulatedStartTime = 2000, .Duration = 0},
		},
	};

	FXT_Game game;
	FXT_Game_Init(&game, &beatmap);

	bool keys[16] = {};

	FXT_Game_Update(&game, 0, keys);
	keys[0] = true;
	FXT_Game_Update(&game, 940, keys);
	keys[0] = false;
	FXT_Game_Update(&game, 980, keys);
	keys[0] = true;
	FXT_Game_Update(&game, 1000, keys);
	keys[0] = false;
	FXT_Game_Update(&game, 1050, keys);
	keys[0] = true;
	FXT_Game_Update(&game, 3010, keys);
	keys[0] = false;
	FXT_Game_Update(&game, 3011, keys);

	return game.Grades.Perfect == 1 && game.Grades.Great == 1;
}

bool Test_Hold()
{
	uint16_t columnSize[] = {2, 0, 0, 0, 0, 0, 0, 0};

	const FXT_Beatmap beatmap = {
		.ColumnSize = columnSize,
		.ColumnCount = 1,
		.OverallDifficulty = 0,
		.Notes = (FXT_Note[2]){
			{.AccumulatedStartTime = 1000, .Duration = 1000},
			{.AccumulatedStartTime = 2000, .Duration = 1000},
		},
	};

	FXT_Game game;
	FXT_Game_Init(&game, &beatmap);

	bool keys[16] = {};

	FXT_Game_Update(&game, 0, keys);
	keys[0] = true;
	FXT_Game_Update(&game, 900, keys);
	keys[0] = false;
	FXT_Game_Update(&game, 950, keys);
	keys[0] = true;
	FXT_Game_Update(&game, 1000, keys);
	keys[0] = false;
	FXT_Game_Update(&game, 2000, keys);
	FXT_Game_Update(&game, 3000, keys);

	return game.Grades.Perfect == 1;
}

bool Test_Config()
{
	FXT_Config config;
	const FXT_Config_Error error = FXT_Config_Load(&config);

	if (error != 0)
		return false;

	printf("Notes falling time: %i ms\n", config.NotesFallingTime);
	printf("Keys: ");
	for (int i = 0; i < FXT_MaxColumnCount; i += 1)
		printf("%i ", config.PhysicalKeyOfFxTapKey[i]);
	printf("\n");

	return true;
}

void DummyRenderTap(const int column, const double positionBottom)
{
	printf("%d: %.5f\n", column + 1, positionBottom);
}

void DummyRenderHold(const int column, const double positionBottom, const double positionTop)
{
	printf("%d: %.5f ~ %.5f\n", column + 1, positionBottom, positionTop);
}

bool Test_RendererController()
{
	FXT_Beatmap beatmap;
	const FXT_BeatmapError error = FXT_Beatmap_Load(&beatmap, "dream.fxt");

	assert(error == 0);

	FXT_Game game;
	FXT_Game_Init(&game, &beatmap);

	const FXT_RendererController controller = {
		.HeightAbove = 100,
		.VisibleTime = 1000,
		.RenderTap = &DummyRenderTap,
		.RenderHold = &DummyRenderHold,
	};

	constexpr bool isKeyPressed[16] = {};

	for (int time = 0; time <= 1000; time += 5)
	{
		printf("> %d\n", time);
		FXT_Game_Update(&game, time, isKeyPressed);
		FXT_RendererController_Run(&controller, &game, time);
	}

	FXT_Beatmap_FreeInner(&beatmap);

	return true;
}

bool Test_Database()
{
	auto const bestGradesFile = fopen("wll.tbg", "wb");

	assert(bestGradesFile != nullptr);

	const FXT_Grades bestGrades = {.Perfect = 1, .Great = 2, .Good = 3, .Ok = 4, .Meh = 5, .Miss = 6};
	fwrite(&bestGrades, sizeof(FXT_Grades), 1, bestGradesFile);
	fclose(bestGradesFile);

	FXT_Database db = nullptr;
	FXT_Database_Init(&db);

	const auto error = FXT_Database_SyncFromFileSystem(&db);

	auto const size = shlenu(db);

	const struct FXT_Database *viewOrderByNameAsc[size] = {};
	const struct FXT_Database *viewOrderByNameDsc[size] = {};

	for (size_t i = 0; i < size; i += 1)
	{
		viewOrderByNameAsc[i] = &db[i];
		viewOrderByNameDsc[i] = &db[i];
	}

	qsort(viewOrderByNameAsc, size, sizeof(FXT_Database), FXT_Database_Compare_Reverse_Void);
	qsort(viewOrderByNameDsc, size, sizeof(FXT_Database), FXT_Database_Compare_Void);

	if (error)
		return false;

	puts("== Original ==");
	for (size_t i = 0; i < size; i += 1)
		printf(
			"[%s]: %.2f - %s - %s\n",
			db[i].key,
			db[i].value.OverallDifficulty,
			db[i].value.Title,
			db[i].value.Artist
		);

	puts("== Descending ==");
	for (size_t i = 0; i < size; i += 1)
	{
		auto const x = viewOrderByNameDsc[i];
		printf(
			"[%s]: %.2f - %s - %s\n",
			x->key, x->value.OverallDifficulty, x->value.Title, x->value.Artist
		);
	}

	puts("== Ascending ==");
	for (size_t i = 0; i < size; i += 1)
	{
		auto const x = viewOrderByNameAsc[i];
		printf(
			"[%s]: %.2f - %s - %s\n",
			x->key, x->value.OverallDifficulty, x->value.Title, x->value.Artist
		);
	}

	shfree(db);

	return true;
}

int main(void)
{
	Run(Test_FileLoading, "File Loading");
	Run(Test_Tap, "Tap");
	Run(Test_Hold, "Hold");
	Run(Test_Config, "Config");
	// Run(Test_RendererController, "RendererController");
	Run(Test_Database, "Database");

	return 0;
}
