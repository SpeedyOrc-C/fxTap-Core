#include <assert.h>
#include <stdio.h>
#include <fxTap/beatmap.h>
#include <fxTap/config.h>
#include <fxTap/game.h>
#include <fxTap/render.h>

bool Run(bool (*test)(), const char *name)
{
	const int result = test();
	printf("[TEST] %s: %s\n", name, result ? "OK" : "ERROR");
	return result;
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

	FXT_Game fxTap;
	FXT_Game_Init(&fxTap, &beatmap);

	bool keys[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	FXT_Game_Update(&fxTap, 0, keys);
	keys[0] = true;
	FXT_Game_Update(&fxTap, 900, keys);
	keys[0] = false;
	FXT_Game_Update(&fxTap, 950, keys);
	keys[0] = true;
	FXT_Game_Update(&fxTap, 1000, keys);
	keys[0] = false;
	FXT_Game_Update(&fxTap, 2000, keys);
	FXT_Game_Update(&fxTap, 3000, keys);

	if (fxTap.Grades.Perfect != 1)
		return false;

	return true;
}

bool Test_FileLoading()
{
	FXT_Beatmap beatmap;
	const FXT_BeatmapError error = FXT_Beatmap_Load(&beatmap, "dream.win.fxt");

	if (error)
	{
		printf("Failed to load the beatmap. Error: %d\n", error);
		return false;
	}

	printf("Title: %s\n", beatmap.Title);
	printf("Artist: %s\n", beatmap.Artist);
	printf("Overall Difficulty: %lf\n", beatmap.OverallDifficulty);
	printf("Number of Columns: %i\n", beatmap.ColumnCount);

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
	const FXT_BeatmapError error = FXT_Beatmap_Load(&beatmap, "dream.win.fxt");

	assert(error == 0);

	FXT_Game fxTap;
	FXT_Game_Init(&fxTap, &beatmap);

	const FXT_RendererController controller = {
		.HeightAbove = 100,
		.VisibleTime = 1000,
		.RenderTap = &DummyRenderTap,
		.RenderHold = &DummyRenderHold,
	};

	const bool isKeyPressed[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	for (int time = 0; time <= 10000; time += 5)
	{
		printf("> %d\n", time);
		FXT_Game_Update(&fxTap, time, isKeyPressed);
		FXT_RendererController_Run(&controller, &fxTap, time);
	}

	FXT_Beatmap_FreeInner(&beatmap);

	return true;
}

int main(void)
{
	Run(Test_FileLoading, "File Loading");
	Run(Test_Hold, "Hold");
	Run(Test_Config, "Config");
	Run(Test_RendererController, "RendererController");

	return 0;
}
