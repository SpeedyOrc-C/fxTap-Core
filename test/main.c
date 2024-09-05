#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fxTap/beatmap.h>
#include <fxTap/config.h>
#include <fxTap/game.h>
#include <fxTap/render.h>

bool Run(bool (*test)(), const char* name)
{
    int result = test();
    printf("[TEST] %s: %s\n", name, result ? "OK" : "ERROR");
    return result;
}

bool Test_Hold()
{
    Beatmap beatmap = {
        .Metadata = {
            .SizeOfColumn = {2, 0, 0, 0, 0, 0, 0, 0},
            .OverallDifficulty = 0,
        },
        .Notes = {
            (Note[2]){
                {.AccumulatedStartTime = 1000, .Duration = 1000},
                {.AccumulatedStartTime = 2000, .Duration = 1000},
            }
        },
    };

    Tolerance_FromOverallDifficulty(&beatmap.Tolerance, 0);

    FxTap fxTap;
    FxTap_Init(&fxTap, &beatmap);

    bool keys[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    FxTap_Update(&fxTap, 0, keys);
    keys[0] = true;
    FxTap_Update(&fxTap, 900, keys);
    keys[0] = false;
    FxTap_Update(&fxTap, 950, keys);
    keys[0] = true;
    FxTap_Update(&fxTap, 1000, keys);
    keys[0] = false;
    FxTap_Update(&fxTap, 2000, keys);
    FxTap_Update(&fxTap, 3000, keys);

    if (fxTap.Grades.Perfect != 1)
        return false;

    return true;
}

bool Test_FileLoading()
{
    BeatmapError error;
    Beatmap *beatmap = Beatmap_New_LoadFromPath("FXTAP/WELUVLAM.fxt", &error);
    {
        if (error)
        {
            printf("Failed to load the beatmap. Error: %d", error);
            return false;
        }

        printf("Title: %s\n", beatmap->Metadata.Title);
        printf("Artist: %s\n", beatmap->Metadata.Artist);
        printf("Overall Difficulty: %lf\n", beatmap->Metadata.OverallDifficulty);
        printf("Number of Columns: %i\n", Beatmap_ColumnCount(beatmap));
    }
    Beatmap_Free(beatmap);

    ConfigError configError;
    Config *config = Config_New_LoadFromDisk(&configError);
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
    FindError error;
    BeatmapFindEntries *entries = BeatmapFindEntries_New_InsideDirectory("FXTAP", &error);

    if (entries == NULL)
        return false;

    printf("There are %i beatmaps under folder FXTAP.\n", entries->Count);

    for (int i = 0; i < entries->Count; i += 1)
        printf("%i. %s: %s\n", i + 1, entries->Entries[i].FileName, entries->Entries[i].Metadata.Title);

    BeatmapFindEntries_Free(entries);

    return true;
}

bool Test_Config()
{
    ConfigError error;
    Config *config = Config_New_LoadFromDisk(&error);

    if (config == NULL)
        return false;

    printf("Notes falling time: %i ms\n", config->NotesFallingTime);
    printf("Keys: ");
    for (int i = 0; i < MAX_COLUMN_COUNT; i += 1)
        printf("%i ", config->PhysicalKeyOfFxTapKey[i]);
    printf("\n");

    free(config);

    return true;
}

void DummyRenderTap(char column, double positionBottom)
{
    printf("%d: %.5f\n", column + 1, positionBottom);
}

void DummyRenderHold(char column, double positionBottom, double positionTop)
{
    printf("%d: %.5f ~ %.5f\n", column + 1, positionBottom, positionTop);
}

bool Test_RendererController()
{
    BeatmapError error;
    Beatmap *beatmap = Beatmap_New_LoadFromPath("FXTAP/WELUVLAM.fxt", &error);

    assert(beatmap != NULL);

    FxTap fxTap;
    FxTap_Init(&fxTap, beatmap);

    RendererController controller = {
        .HeightAbove = 100,
        .VisibleTime = 1000,
        .RenderTap = &DummyRenderTap,
        .RenderHold = &DummyRenderHold,
    };

    const bool isKeyPressed[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    for (int time = 0; time <= 10000; time += 5)
    {
        printf("> %d\n", time);
        FxTap_Update(&fxTap, time, isKeyPressed);
        RendererController_Run(&controller, &fxTap, time);
    }

    Beatmap_Free(beatmap);

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
