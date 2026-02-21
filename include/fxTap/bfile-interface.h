#pragma once

#include <stdint.h>

#ifndef GINT_BFILE

#define BFile_File    1
#define BFile_Folder  5

#define BFile_ReadOnly        0x01
#define BFile_WriteOnly       0x02
#define BFile_ReadWrite       (BFile_ReadOnly | BFile_WriteOnly)
#define BFile_Share           0x80
#define BFile_ReadShare       (BFile_ReadOnly | BFile_Share)
#define BFile_ReadWriteShare  (BFile_ReadWrite | BFile_Share)

#endif

extern int BFile_Open(uint16_t const *path, int mode);

extern int BFile_Close(int fd);

extern int BFile_Remove(uint16_t const *path);

extern int BFile_Read(int handle, void *data, int size, int whence);

extern int BFile_Create(uint16_t const *path, int type, int *size);

extern int BFile_Write(int fd, void const *data, int even_size);

extern uint16_t *fs_path_normalize_fc(char const *path);
