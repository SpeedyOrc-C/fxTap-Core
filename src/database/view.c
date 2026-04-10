#include <stb_ds.h>
#include <stdlib.h>
#include <fxTap/database.h>
#include <fxTap/database/view.h>

static int FXT_BeatmapGroupMember_Compare_Void(const void *a, const void *b)
{
	return strcmp(
		(*(FXT_BeatmapGroupMember *) a)->value.Version,
		(*(FXT_BeatmapGroupMember *) b)->value.Version
	);
}

FXT_DatabaseError FXT_DatabaseView_Init(FXT_DatabaseView *view, const FXT_Database *database)
{
	auto const db = *database;

	auto const beatmapCount = shlenu(db);

	view->GroupCount = 0;

	for (size_t beatmapI = 0; beatmapI < beatmapCount; beatmapI += 1)
	{
		auto const kv = &db[beatmapI];

		if (view->GroupCount == 0)
		{
			view->Groups = malloc(sizeof(FXT_BeatmapGroup));

			if (view->Groups == nullptr)
				goto fail;

			view->GroupCount = 1;
			view->Groups[0] = (FXT_BeatmapGroup){
				.Size = 0,
				.Title = nullptr,
				.Members = nullptr
			};
		}

		for (size_t groupI = 0; groupI < view->GroupCount; groupI += 1)
		{
			auto const group = &view->Groups[groupI];

			if (group->Size == 0)
			{
				group->Members = malloc(sizeof(FXT_BeatmapGroupMember));

				if (group->Members == nullptr)
					goto fail;

				group->Size = 1;
				group->Title = kv->value.Title;
				group->Members[0] = kv;

				goto inserted_one;
			}

			if (strcmp(group->Title, kv->value.Title) == 0)
			{
				auto const newMembers = realloc(group->Members, (group->Size + 1) * sizeof(FXT_BeatmapGroupMember));

				if (newMembers == nullptr)
					goto fail;

				group->Members = newMembers;
				group->Members[group->Size] = kv;
				group->Size += 1;

				goto inserted_one;
			}
		}

		auto const newGroups = realloc(view->Groups, (view->GroupCount + 1) * sizeof(FXT_BeatmapGroup));

		if (newGroups == nullptr)
			goto fail;

		view->Groups = newGroups;

		auto const group = &view->Groups[view->GroupCount];
		*group = (FXT_BeatmapGroup){.Size = 0, .Title = nullptr, .Members = nullptr};

		group->Members = malloc(sizeof(FXT_BeatmapGroupMember));

		if (group->Members == nullptr)
			goto fail;

		group->Size = 1;
		group->Title = kv->value.Title;
		group->Members[0] = kv;

		view->GroupCount += 1;

	inserted_one:;
	}

	for (size_t groupI = 0; groupI < view->GroupCount; groupI += 1)
	{
		auto const group = view->Groups[groupI];
		qsort(group.Members, group.Size, sizeof(FXT_BeatmapGroupMember),
		      FXT_BeatmapGroupMember_Compare_Void);
	}

	view->VersionWindowSize = 3;
	view->VersionWindowHeadIndex = 0;

	return 0;

fail:
	for (size_t groupI = 0; groupI < view->GroupCount; groupI += 1)
		if (view->Groups[groupI].Size > 0)
			free(view->Groups[groupI].Members);

	if (view->GroupCount > 0) free(view->Groups);

	return FXT_DatabaseError_View_MallocFailed;
}

FXT_BeatmapGroupMember FXT_DatabaseView_FocusedMember(const FXT_DatabaseView *view)
{
	return view->Groups[view->FocusedGroupIndex].Members[view->FocusedVersionIndex];
}

static int FXT_BeatmapGroup_Compare_Void(const void *a, const void *b)
{
	return strcmp(
		((const FXT_BeatmapGroup *) a)->Title,
		((const FXT_BeatmapGroup *) b)->Title
	);
}

static int FXT_BeatmapGroup_Compare_Reverse_Void(const void *a, const void *b)
{
	return -FXT_BeatmapGroup_Compare_Void(a, b);
}

void FXT_DatabaseView_SortAsc(FXT_DatabaseView *view)
{
	view->FocusedGroupIndex = 0;
	view->FocusedVersionIndex = 0;
	view->VersionWindowHeadIndex = 0;

	qsort(view->Groups, view->GroupCount, sizeof(FXT_BeatmapGroup),
	      FXT_BeatmapGroup_Compare_Reverse_Void);
}

void FXT_DatabaseView_SortDsc(FXT_DatabaseView *view)
{
	view->FocusedGroupIndex = 0;
	view->FocusedVersionIndex = 0;
	view->VersionWindowHeadIndex = 0;

	qsort(view->Groups, view->GroupCount, sizeof(FXT_BeatmapGroup),
	      FXT_BeatmapGroup_Compare_Void);
}

void FXT_DatabaseView_NextVersion(FXT_DatabaseView *view)
{
	if (view->FocusedVersionIndex + 1 < view->Groups[view->FocusedGroupIndex].Size)
	{
		view->FocusedVersionIndex += 1;

		if (view->FocusedVersionIndex >= view->VersionWindowHeadIndex + view->VersionWindowSize)
			view->VersionWindowHeadIndex += 1;
	}
	else
		FXT_DatabaseView_NextGroup(view);
}

void FXT_DatabaseView_PreviousVersion(FXT_DatabaseView *view)
{
	if (view->FocusedVersionIndex > 0)
	{
		view->FocusedVersionIndex -= 1;

		if (view->FocusedVersionIndex < view->VersionWindowHeadIndex)
			view->VersionWindowHeadIndex -= 1;
	}
	else
	{
		FXT_DatabaseView_PreviousGroup(view);
		view->FocusedVersionIndex = view->Groups[view->FocusedGroupIndex].Size - 1;

		if (view->VersionWindowSize <= view->Groups[view->FocusedGroupIndex].Size)
			view->VersionWindowHeadIndex = view->Groups[view->FocusedGroupIndex].Size - view->VersionWindowSize;
	}
}

void FXT_DatabaseView_NextGroup(FXT_DatabaseView *view)
{
	view->FocusedVersionIndex = 0;
	view->VersionWindowHeadIndex = 0;

	if (view->FocusedGroupIndex + 1 < view->GroupCount)
		view->FocusedGroupIndex += 1;
	else
		view->FocusedGroupIndex = 0;
}

void FXT_DatabaseView_PreviousGroup(FXT_DatabaseView *view)
{
	view->FocusedVersionIndex = 0;
	view->VersionWindowHeadIndex = 0;

	if (view->FocusedGroupIndex > 0)
		view->FocusedGroupIndex -= 1;
	else
		view->FocusedGroupIndex = view->GroupCount - 1;
}
