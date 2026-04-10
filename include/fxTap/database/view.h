#pragma once

#include <fxTap/database.h>

typedef FXT_DatabaseKV *FXT_BeatmapGroupMember;
typedef FXT_BeatmapGroupMember *Members;

typedef struct
{
	size_t Size;
	const char *Title;
	Members Members;
} FXT_BeatmapGroup;

typedef FXT_BeatmapGroup *Groups;

typedef struct FXT_DatabaseView
{
	size_t FocusedGroupIndex;
	size_t FocusedVersionIndex;
	size_t VersionWindowSize;
	size_t VersionWindowHeadIndex;
	size_t GroupCount;
	Groups Groups;
} FXT_DatabaseView;

[[nodiscard]]
FXT_DatabaseError FXT_DatabaseView_Init(FXT_DatabaseView *view, const FXT_Database *database);

FXT_BeatmapGroupMember FXT_DatabaseView_FocusedMember(const FXT_DatabaseView *view);

void FXT_DatabaseView_SortAsc(FXT_DatabaseView *view);

void FXT_DatabaseView_SortDsc(FXT_DatabaseView *view);

void FXT_DatabaseView_NextVersion(FXT_DatabaseView *view);

void FXT_DatabaseView_PreviousVersion(FXT_DatabaseView *view);

void FXT_DatabaseView_NextGroup(FXT_DatabaseView *view);

void FXT_DatabaseView_PreviousGroup(FXT_DatabaseView *view);
