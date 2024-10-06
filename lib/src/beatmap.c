#include <fxTap/beatmap.h>
#include <fxTap/endian-utility.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#ifdef FXTAP_CORE_HAS_DIRENT
#include <dirent.h>
#endif

Tolerance Tolerance_FromOverallDifficulty(double overallDifficulty)
{
    const double od3 = 3 * overallDifficulty;

    return (Tolerance) {
        .Perfect = BASE_TOLERANCE_PERFECT,
        .Great = (int32_t) ((double) BASE_TOLERANCE_GREAT - od3),
        .Good = (int32_t) ((double) BASE_TOLERANCE_GOOD - od3),
        .Ok = (int32_t) ((double) BASE_TOLERANCE_OK - od3),
        .Meh = (int32_t) ((double) BASE_TOLERANCE_MEH - od3),
        .Miss = (int32_t) ((double) BASE_TOLERANCE_MISS - od3),
    };
}

BeatmapError Metadata_LoadFromFile(Metadata *metadata, FILE *file)
{
    if (1 > fread(metadata, sizeof(Metadata), 1, file))
        return BeatmapError_ReadMetadataFailed;

    if (EnvironmentIsBigEndian())
    {
        for (int column = 0; column < MAX_COLUMN_COUNT; column += 1)
            SwapBytes(metadata->SizeOfColumn[column]);

        SwapBytes(metadata->OverallDifficulty);
    }

    return BeatmapError_OK;
}

BeatmapError Beatmap_LoadFromFile(Beatmap *beatmap, FILE *file)
{
    const BeatmapError error = Metadata_LoadFromFile(&beatmap->Metadata, file);

    if (error)
        return error;

    beatmap->Tolerance = Tolerance_FromOverallDifficulty(beatmap->Metadata.OverallDifficulty);

    // Load notes
    const int TotalNotesCount = Beatmap_NoteCount(beatmap);

    Note *notesBuffer = calloc(TotalNotesCount, sizeof(Note));

    if (notesBuffer == NULL)
        return BeatmapError_MallocFailed;

    beatmap->Notes[0] = notesBuffer;
    int accumulatedNotesCount = beatmap->Metadata.SizeOfColumn[0];

    for (int column = 1; column < MAX_COLUMN_COUNT; column += 1)
    {
        beatmap->Notes[column] = notesBuffer + accumulatedNotesCount;
        accumulatedNotesCount += beatmap->Metadata.SizeOfColumn[column];
    }

    if (TotalNotesCount > fread(notesBuffer, sizeof(Note), TotalNotesCount, file))
    {
        free(notesBuffer);
        return BeatmapError_ReadNotesFailed;
    }

    if (EnvironmentIsBigEndian())
    {
        for (int column = 0; column < 8; column += 1)
        {
            for (int note = 0; note < beatmap->Metadata.SizeOfColumn[column]; note += 1)
            {
                SwapBytes(beatmap->Notes[column][note].AccumulatedStartTime);
                SwapBytes(beatmap->Notes[column][note].Duration);
            }
        }
    }

    return BeatmapError_OK;
}

Beatmap* Beatmap_New_LoadFromPath(const char *path, BeatmapError *error)
{
    Beatmap *beatmap = malloc(sizeof(Beatmap));

    if (beatmap == NULL)
    {
        *error = BeatmapError_MallocFailed;
        return NULL;
    }

    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        free(beatmap);
        *error = BeatmapError_FileNotFound;
        return NULL;
    }

    *error = Beatmap_LoadFromFile(beatmap, file);

    if (*error)
    {
        free(beatmap);
        fclose(file);
        return NULL;
    }

    if (fclose(file) != 0)
    {
        free(beatmap);
        *error = BeatmapError_CannotCloseFile;
        return NULL;
    }

    *error = BeatmapError_OK;
    return beatmap;
}

void Beatmap_Free(Beatmap *beatmap)
{
    free(beatmap->Notes[0]);
    free(beatmap);
}

int Beatmap_ColumnCount(const Beatmap *beatmap)
{
    int fromColumn = -1;

    for (int column = 0; column < MAX_COLUMN_COUNT; column += 1)
    {
        if (beatmap->Metadata.SizeOfColumn[column] > 0)
        {
            fromColumn = column;
            break;
        }
    }

    if (fromColumn == -1)
        return 0;

    int toColumn = MAX_COLUMN_COUNT + 1;

    for (int column = MAX_COLUMN_COUNT - 1; column >= 0; column -= 1)
    {
        if (beatmap->Metadata.SizeOfColumn[column] > 0)
        {
            toColumn = column;
            break;
        }
    }

    return toColumn - fromColumn + 1;
}

int Beatmap_NoteCount(const Beatmap *beatmap)
{
    int count = 0;

    for (int column = 0; column < MAX_COLUMN_COUNT; column += 1)
        count += beatmap->Metadata.SizeOfColumn[column];

    return count;
}

#ifdef FXTAP_CORE_HAS_DIRENT

BeatmapFindEntries *BeatmapFindEntries_New_InsideDirectory(const char *path, FindError *error)
{
    BeatmapFindEntries *entries = malloc(sizeof(BeatmapFindEntries));

    if (entries == NULL)
    {
        *error = FindError_MallocFailed;
        return NULL;
    }

    DIR *directory = opendir(path);

    if (directory == NULL)
    {
        free(entries);
        *error = FindError_FxtapFolderNotFound;
        return NULL;
    }

    entries->Count = 0;

    entries->Entries = calloc(1, sizeof(BeatmapFindEntry));

    while (true)
    {
        // Read an entry
        struct dirent *entry = readdir(directory);

        if (entry == NULL)
            break;

        // Check whether the file extension is *.FXT
        unsigned long fileNameLength = strlen(entry->d_name);

        if (fileNameLength < 5)
            continue;

        if (toupper(entry->d_name[fileNameLength - 3]) != 'F' ||
            toupper(entry->d_name[fileNameLength - 2]) != 'X' ||
            toupper(entry->d_name[fileNameLength - 1]) != 'T')
        {
            continue;
        }

        // Extend the file name list
        entries->Count += 1;

        BeatmapFindEntry *extendedEntries =
            realloc(entries->Entries, entries->Count * sizeof(BeatmapFindEntry));

        if (extendedEntries == NULL)
        {
            closedir(directory);

            for (int i = 0; i < entries->Count - 1; i += 1)
                free(entries->Entries[i].FileName);
            free(entries->Entries);
            free(entries);
            *error = FindError_MallocFailed;
            return NULL;
        }

        entries->Entries = extendedEntries;

        // Append the entry's file name to the list
        char* fileName = malloc(strlen(entry->d_name) + 1);

        if (fileName == NULL)
        {
            closedir(directory);

            for (int i = 0; i < entries->Count - 1; i += 1)
                free(entries->Entries[i].FileName);
            free(entries->Entries);
            free(entries);
            *error = FindError_MallocFailed;
            return NULL;
        }

        strcpy(fileName, entry->d_name);

        entries->Entries[entries->Count - 1].FileName = fileName;

        // Compose the beatmap's path
        char filePath[strlen(path) + 1 + strlen(fileName) + 1];

        sprintf(filePath, "%s/%s", path, fileName);

        // Read metadata
        FILE *file = fopen(filePath, "rb");

        assert(file != NULL);

        if (Metadata_LoadFromFile(&entries->Entries[entries->Count - 1].Metadata, file))
        {
            closedir(directory);

            for (int i = 0; i < entries->Count; i += 1)
                free(entries->Entries[i].FileName);
            free(entries->Entries);
            free(entries);
            *error = FindError_BadFile;
            return NULL;
        }

        fclose(file);
    }

    closedir(directory);

    *error = FindError_OK;
    return entries;
}

#endif

void BeatmapFindEntries_Free(BeatmapFindEntries *entries)
{
    for (int i = 0; i < entries->Count; i += 1)
        free(entries->Entries[i].FileName);
    free(entries->Entries);
    free(entries);
}
