#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
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
	FXT_Beatmap beatmap = {
		.Metadata = {
			.SizeOfColumn = {2, 0, 0, 0, 0, 0, 0, 0},
			.OverallDifficulty = 0,
		},
		.Notes = {
			(FXT_Note[2]){
				{.AccumulatedStartTime = 1000, .Duration = 1000},
				{.AccumulatedStartTime = 2000, .Duration = 1000},
			}
		},
	};

	beatmap.Tolerance = FXT_Tolerance_FromOverallDifficulty(0);

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
	FXT_BeatmapError error;
	FXT_Beatmap *beatmap = FXT_Beatmap_Load("FXTAP/WELUVLAM.fxt", &error);
	{
		if (error)
		{
			printf("Failed to load the beatmap. Error: %d", error);
			return false;
		}

		printf("Title: %s\n", beatmap->Metadata.Title);
		printf("Artist: %s\n", beatmap->Metadata.Artist);
		printf("Overall Difficulty: %lf\n", beatmap->Metadata.OverallDifficulty);
		printf("Number of Columns: %i\n", FXT_Beatmap_ColumnCount(beatmap));
	}
	FXT_Beatmap_Free(beatmap);

	FXT_Config_Error configError;
	FXT_Config *config = Config_New_LoadFromDisk(&configError);
	{
		if (config == NULL)
		{
			printf("Failed to load the config, and failed to create it.");
			return false;
		}

		config->PhysicalKeyOfFxTapKey[FxTapKey_K4] = 'X';

		if (!Config_SaveToDisk(config))
		{
			printf("Failed to save the config.");
			return false;
		}
	}
	free(config);

	return true;
}

bool Test_QueryBeatmaps()
{
	FXT_FindError error;
	FXT_BeatmapFindEntries *entries = BeatmapFindEntries_New_InsideDirectory("FXTAP", &error);

	if (entries == NULL)
		return false;

	printf("There are %i beatmaps under folder FXTAP.\n", entries->Count);

	for (int i = 0; i < entries->Count; i += 1)
		printf("%i. %s: %s\n", i + 1, entries->Entries[i].FileName, entries->Entries[i].Metadata.Title);

	FXT_BeatmapFindEntries_Free(entries);

	return true;
}

bool Test_Config()
{
	FXT_Config_Error error;
	FXT_Config *config = Config_New_LoadFromDisk(&error);

	if (config == NULL)
		return false;

	printf("Notes falling time: %i ms\n", config->NotesFallingTime);
	printf("Keys: ");
	for (int i = 0; i < FXT_MaxColumnCount; i += 1)
		printf("%i ", config->PhysicalKeyOfFxTapKey[i]);
	printf("\n");

	free(config);

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
	FXT_BeatmapError error;
	FXT_Beatmap *beatmap = FXT_Beatmap_Load("FXTAP/WELUVLAM.fxt", &error);

	assert(beatmap != NULL);

	FXT_Game fxTap;
	FXT_Game_Init(&fxTap, beatmap);

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

	FXT_Beatmap_Free(beatmap);

	return true;
}

int main(void)
{
	return !(
		Run(Test_FileLoading, "File Loading") |
		Run(Test_Hold, "Hold") |
		Run(Test_QueryBeatmaps, "Query beatmaps") |
		Run(Test_Config, "Config") |
		Run(Test_RendererController, "RendererController")
	);
}
