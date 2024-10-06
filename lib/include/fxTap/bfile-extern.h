#ifndef FXTAP_CORE_BFILE_EXTERN_H
#define FXTAP_CORE_BFILE_EXTERN_H

#include <fxTap/lib-config.h>

#if defined(FXTAP_CORE_ON_GINT)

// The source below is copied from Lephenixnoir's gint:

#include <stdint.h>

#ifndef GINT_BFILE

//---
// Common file access functions
//---

/* Remove a file or folder (also works if the entry does not exist). */
extern int BFile_Remove(uint16_t const *path);

/* Rename a file (can move folders; Fugue only). */
extern int BFile_Rename(uint16_t const *oldpath, uint16_t const *newpath);

#define BFile_File    1
#define BFile_Folder  5

/* BFile_Create(): Create a new file or folder

   The file or directory must not exist. For a file the size pointer must point
   to the desired file size (which is fixed), for a folder it is ignored. With
   CASIOWIN this is the final size of the file. With Fugue the file can be
   resized dynamically and is usually created with initial size 0.

   @path  FONTCHARACTER file path
   @type  Entry type (BFile_File or BFile_Folder)
   @size  Pointer to file size if type is BFile_File, use NULL otherwise */
extern int BFile_Create(uint16_t const *path, int type, int *size);

#define BFile_ReadOnly        0x01
#define BFile_WriteOnly       0x02
#define BFile_ReadWrite       (BFile_ReadOnly | BFile_WriteOnly)
#define BFile_Share           0x80
#define BFile_ReadShare       (BFile_ReadOnly | BFile_Share)
#define BFile_ReadWriteShare  (BFile_ReadWrite | BFile_Share)

/* BFile_Open(): Open a file for reading or writing

   The file must exist, even when opening in writing mode. The meaning of the
   [Share] flag isn't clear; I believe it's simply ignored in the CASIOWIN
   filesystem.

   @path  FONTCHARACTER file path
   @mode  Desired access mode
   Returns a file descriptor (or a negative error code). */
extern int BFile_Open(uint16_t const *path, int mode);

/* Close an open file descriptor. */
extern int BFile_Close(int fd);

/* Get the size of an open file. */
extern int BFile_Size(int fd);

/* BFile_Write(): Write data to an open file

   WARNING: The CASIOWIN fs is known to become inconsistent if an odd number of
            bytes is written with BFile_Write(). Always keep it even!

   With CASIOWIN, returns the new file offset after writing (or an error code).
   With Fugue, returns the amount of data written (or an error code). */
extern int BFile_Write(int fd, void const *data, int even_size);

/* BFile_Read(): Read data from an open file

   The extra argument [whence] specifies where data is read from in the style
   of pread(2), and is supported with both filesystems.

   * If [whence >= 0], it is taken as an absolute location within the file;
   * If [whence == -1], BFile_Read() reads from the current position.

   With Fugue this function can read past end of file and return the requested
   amount of bytes even when the file does not have enough data to read that
   much. It seems that extra bytes read as zeros. Reading past the end does
   *not* extend the file size.

   With CASIOWIN, returns how much data can be read from the updated file
   offset (ie. how many bytes until end of file), or an error code.
   With Fugue, returns the amount of data read (or an error code). */
extern int BFile_Read(int handle, void *data, int size, int whence);

/* BFile_Seek(): Seek to a relative or absolute position within an open file

   With CASIOWIN, moves [offset] bytes relative to the current position, and
   returns how much data can be read from the new position. BFile_Seek(fd, 0)
   combined with BFile_Size(fd) can be used to determine the current position.

   With Fugue, moves to the absolute position [offset] within the file, and
   returns the amount of allocated space following the new position (usually
   larger than the amount of data until end-of-file because files are allocated
   in units of 4096 bytes). There is no way to seek relative to the current
   position unless the target is precomputed with BFile_GetPos(). */
extern int BFile_Seek(int fd, int offset);

/* BFile_GetPos(): Get the current position in an open file

   This call does not exist in the CASIOWIN interface, so this function always
   returns -1 on models with a CASIOWIN filesystem.

   This call exists in Fugue, however some Fugue models have their syscall list
   inherited from the CASIOWIN era and don't have an entry point for it (or if
   there's one it's escape scrutiny so far).

   * Prizm / Graph 90+E / fx-CG series: this function works as intended.
   * Graph 35+E II / G-III series: the call is missing, returns -1.

   For the latter models there is no easily reliable way of knowing the current
   position within an open file! */
int BFile_GetPos(int fd);

//---
// Error codes
//---

#define BFile_EntryNotFound       -1
#define BFile_IllegalParam        -2
#define BFile_IllegalPath         -3
#define BFile_DeviceFull          -4
#define BFile_IllegalDevice       -5
#define BFile_IllegalFilesystem   -6
#define BFile_IllegalSystem       -7
#define BFile_AccessDenied        -8
#define BFile_AlreadyLocked       -9
#define BFile_IllegalTaskID       -10
#define BFile_PermissionError     -11
#define BFile_EntryFull           -12
#define BFile_AlreadyExists       -13
#define BFile_ReadOnlyFile        -14
#define BFile_IllegalFilter       -15
#define BFile_EnumerateEnd        -16
#define BFile_DeviceChanged       -17
//#define BFile_NotRecordFile     -18     // Not used
#define BFile_IllegalSeekPos      -19
#define BFile_IllegalBlockFile    -20
//#define BFile_NoSuchDevice      -21     // Not used
//#define BFile_EndOfFile         -22     // Not used
#define BFile_NotMountDevice      -23
#define BFile_NotUnmountDevice    -24
#define BFile_CannotLockSystem    -25
#define BFile_RecordNotFound      -26
//#define BFile_NotDualRecordFile -27     // Not used
#define BFile_NoAlarmSupport      -28
#define BFile_CannotAddAlarm      -29
#define BFile_FileFindUsed        -30
#define BFile_DeviceError         -31
#define BFile_SystemNotLocked     -32
#define BFile_DeviceNotFound      -33
#define BFile_FileTypeMismatch    -34
#define BFile_NotEmpty            -35
#define BFile_BrokenSystemData    -36
#define BFile_MediaNotReady       -37
#define BFile_TooManyAlarms       -38
#define BFile_SameAlarmExists     -39
#define BFile_AccessSwapArea      -40
#define BFile_MultimediaCard      -41
#define BFile_CopyProtection      -42
#define BFile_IllegalFileData     -43

//---
// Search API
//
// The search API is described below. It's somewhat unreliable, with unclear
// semantics and a history of breaking in seemingly reasonable programs. One
// day probably we'll know how to use it properly and reliably.
//
// Note: always close search handles or trouble will ensue (eg. add-in
// discovery failing).
//---

struct BFile_FileInfo
{
    uint16_t index;
    uint16_t type;
    uint32_t file_size;
    /* Data size (file size minus the header) */
    uint32_t data_size;
    /* Is 0 if the file is complete */
    uint32_t property;
    /* Address of first fragment (do not use directly) */
    void *address;
};

#define BFile_Type_Directory  0x0000
#define BFile_Type_File       0x0001
#define BFile_Type_Addin      0x0002
#define BFile_Type_Eact       0x0003
#define BFile_Type_Language   0x0004
#define BFile_Type_Bitmap     0x0005
#define BFile_Type_MainMem    0x0006
#define BFile_Type_Temp       0x0007
#define BFile_Type_Dot        0x0008
#define BFile_Type_DotDot     0x0009
#define BFile_Type_Volume     0x000a
#define BFile_Type_Archived   0x0041

/* BFile_FindFirst(): Search the storage memory for paths matching a pattern

   This if for the storage memory only. There are only four search handles;
   make sure to close them, there is no automatic device to close them for you
   even after the add-in exists.

   Search is NOT case sensitive. '*' can be used as a wildcard, although it's
   unclear whether more than one '*' is supported, whether '*' can match in a
   directory name, whether multiple folders can be searched simultaneously, and
   whether directories can be matched.

   @pattern    FONTCHARACTER glob pattern
   @shandle    Will receive search handle (to use in BFile_FindNext/FindClose)
   @foundfile  Will receive FONTCHARACTER path of matching entry
   @fileinfo   Will receive metadata of matching entry

   On success, returns 0, stores the search handle in *shandle, and stores
   information about the first match in foundfile and fileinfo. The negative
   error code BFile_EntryNotFound should be interpreted as an empty result (ie.
   no entry matched) rather than an error.

   Returns 0 on success or a negative error code. BFile_EntryNotFound should be
   interpreted as an empty result (ie. no file matched). */
extern int BFile_FindFirst(uint16_t const *pattern, int *shandle, uint16_t *foundfile,
                    struct BFile_FileInfo *fileinfo);

/* BFile_FindNext(): Continue a search

   Continues the search for matches. The search handle is the value set in
   *shandle in BFile_FindFirst(). As before, *foundfile receives the matching
   entry's path and *fileinfo its metadata.

   Returns 0 on success. The negative error code BFile_EnumerateEnd should be
   interpreted as the end of the search (ie. all matching files have been
   returned) rather than an error. */
extern int BFile_FindNext(int shandle, uint16_t *foundfile,
                   struct BFile_FileInfo *fileinfo);

/* Close a search handle (with or without exhausting the matches). */
extern int BFile_FindClose(int shandle);

#endif //GINT_BFILE

#ifndef GINT_FS

/* fs_path_normalize(): Normalize a path to eliminate ., .. and redundant /

   This function creates a copy of the specified path which is an absolute path
   with redundant / removed and . and .. components resolved. For instance:

     "" -> "/"
     "/subfolder/./" -> "/subfolder"
     "/../subfolder/../subfolder" -> "/subfolder"
     "/../../x/y/../t" -> "/x/t"

   The new string is created with malloc() and should be free()'d after use. */
__attribute__ ((malloc))
extern char *fs_path_normalize(char const *path);

/* fs_path_normalize_fc(): Normalize a path and translate it to FONTCHARACTER

   This function is similar to fs_path_normalize(), but it creates a 16-bit
   string that starts with \\fls0\ rather than /, and can be used in direct
   calls to BFile. */
__attribute__ ((malloc))
extern uint16_t *fs_path_normalize_fc(char const *path);

#endif //GINT_FS

#endif

#endif //FXTAP_CORE_BFILE_EXTERN_H
