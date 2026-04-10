#include <stb_ds.h>
#include <stdio.h>
#include <fxTap/beatmap.h>
#include <fxTap/config.h>
#include <fxTap/database.h>
#include <fxTap/game.h>
#include <fxTap/keymap.h>
#include <fxTap/render.h>

#include "fxTap/database/view.h"

static FXT_ModOption modOption;

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
	FXT_Game_Init(&game, &beatmap, &modOption);

	auto const startTime = FXT_Game_FirstNoteStartTime(&game);
	auto const endTime = FXT_Game_LastNoteEndTime(&game);

	printf("Title: %s\n", beatmap.Title);
	printf("Artist: %s\n", beatmap.Artist);
	printf("Overall Difficulty: %lf\n", beatmap.OverallDifficulty);
	printf("Number of Columns: %i\n", beatmap.ColumnCount);
	printf("Start/End Time: %i/%i\n", startTime, endTime);

	FXT_Beatmap_FreeInner(&beatmap);

	FXT_Config config;

	const FXT_ConfigError configError = FXT_Config_Load(&config);

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
	FXT_Game_Init(&game, &beatmap, &modOption);

	bool keys[16] = {};

	FXT_Game_Update(&game, &modOption, 0, keys);
	keys[0] = true;
	FXT_Game_Update(&game, &modOption, 940, keys);
	keys[0] = false;
	FXT_Game_Update(&game, &modOption, 980, keys);
	keys[0] = true;
	FXT_Game_Update(&game, &modOption, 1000, keys);
	keys[0] = false;
	FXT_Game_Update(&game, &modOption, 1050, keys);
	keys[0] = true;
	FXT_Game_Update(&game, &modOption, 3010, keys);
	keys[0] = false;
	FXT_Game_Update(&game, &modOption, 3011, keys);

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
	FXT_Game_Init(&game, &beatmap, &modOption);

	bool keys[16] = {};

	FXT_Game_Update(&game, &modOption, 0, keys);
	keys[0] = true;
	FXT_Game_Update(&game, &modOption, 900, keys);
	keys[0] = false;
	FXT_Game_Update(&game, &modOption, 950, keys);
	keys[0] = true;
	FXT_Game_Update(&game, &modOption, 1000, keys);
	keys[0] = false;
	FXT_Game_Update(&game, &modOption, 2000, keys);
	FXT_Game_Update(&game, &modOption, 3000, keys);

	return game.Grades.Perfect == 1;
}

bool Test_Config()
{
	FXT_Config config;
	const FXT_ConfigError error = FXT_Config_Load(&config);

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
	FXT_Game_Init(&game, &beatmap, &modOption);

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
		FXT_Game_Update(&game, &modOption, time, isKeyPressed);
		FXT_RendererController_Run(&controller, &game, &modOption, time);
	}

	FXT_Beatmap_FreeInner(&beatmap);

	return true;
}

static void PrintDatabaseView(const FXT_DatabaseView *view)
{
	for (size_t i = 0; i < view->GroupCount; i += 1)
	{
		auto const group = view->Groups[i];
		printf("%zu. %s\n", i + 1, group.Title);

		for (size_t j = 0; j < group.Size; j += 1)
		{
			auto const member = group.Members[j];
			printf("    %zu. %s\n", j + 1, member->value.Version);
		}
	}
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

	auto error = FXT_Database_SyncFromFileSystem(&db);

	assert(error == 0);

	FXT_DatabaseView view;
	error = FXT_DatabaseView_Init(&view, &db);

	assert(error == 0);

	FXT_DatabaseView_SortDsc(&view);
	PrintDatabaseView(&view);

	FXT_DatabaseView_SortAsc(&view);
	PrintDatabaseView(&view);

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
