#include <fxTap/beatmap-casiowin.h>
#include <fxTap/endian-utility.h>
#include <stdlib.h>

BeatmapError Metadata_LoadFromFile_BFile(Metadata *metadata, int bfile)
{
    if (sizeof(Metadata) > BFile_Read(bfile, metadata, sizeof(Metadata), -1))
        return BeatmapError_ReadMetadataFailed;

    for (int column = 0; column < MAX_COLUMN_COUNT; column += 1)
        SwapBytes(metadata->SizeOfColumn[column]);

    SwapBytes(metadata->OverallDifficulty);

    return BeatmapError_OK;
}

BeatmapError Beatmap_LoadFromFile_BFile(Beatmap *beatmap, int bfile)
{
    const BeatmapError error = Metadata_LoadFromFile_BFile(&beatmap->Metadata, bfile);

    if (error)
        return error;

    Tolerance_FromOverallDifficulty(&beatmap->Tolerance, beatmap->Metadata.OverallDifficulty);

    // Load notes
    const int TotalNotesCount = Beatmap_NoteCount(beatmap);

    Note *notesBuffer = calloc(TotalNotesCount, sizeof(Note));

    if (notesBuffer == NULL)
        return BeatmapError_MallocFailed;

    beatmap->Notes[0] = notesBuffer;
    int accumulatedNotesCount = beatmap->Metadata.SizeOfColumn[0];

    for (char column = 1; column < MAX_COLUMN_COUNT; column += 1)
    {
        beatmap->Notes[column] = notesBuffer + accumulatedNotesCount;
        accumulatedNotesCount += beatmap->Metadata.SizeOfColumn[column];
    }

    if (sizeof(Note) * TotalNotesCount > BFile_Read(bfile, notesBuffer, (int) sizeof(Note) * TotalNotesCount, -1))
    {
        free(notesBuffer);
        return BeatmapError_ReadNotesFailed;
    }

    for (int column = 0; column < 8; column += 1)
    {
        for (int note = 0; note < beatmap->Metadata.SizeOfColumn[column]; note += 1)
        {
            SwapBytes(beatmap->Notes[column][note].AccumulatedStartTime);
            SwapBytes(beatmap->Notes[column][note].Duration);
        }
    }

    return BeatmapError_OK;
}

Beatmap *Beatmap_New_LoadFromPath_BFile(const char *path, BeatmapError *error)
{
    uint16_t *pathCasiowin = fs_path_normalize_fc(path);

    if (pathCasiowin == NULL)
        return NULL;

    int bfile = BFile_Open(pathCasiowin, BFile_ReadOnly);

    if (bfile < 0)
    {
        free(pathCasiowin);
        *error = BeatmapError_FileNotFound;
        return NULL;
    }

    Beatmap *beatmap = malloc(sizeof(Beatmap));

    if (beatmap == NULL)
    {
        free(pathCasiowin);
        BFile_Close(bfile);
        *error = BeatmapError_MallocFailed;
        return NULL;
    }

    *error = Beatmap_LoadFromFile_BFile(beatmap, bfile);

    if (*error)
    {
        free(pathCasiowin);
        free(beatmap);
        BFile_Close(bfile);
        return NULL;
    }

    free(pathCasiowin);
    BFile_Close(bfile);

    *error = BeatmapError_OK;
    return beatmap;
}
