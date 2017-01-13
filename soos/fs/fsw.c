#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <3ds.h>

#include "../misc/nnerr.h"
#include "fsw.h"

#define FS_ATTRIBUTE_NONE 0 //y u remove dis? this was present in older ctrulib revisions

FS_Archive fswarch;
char root[262];

Result fsw_init_bin(FS_ArchiveID arch, FS_MediaType mediatype, u64 stuff, char* r)
{
    memset(root, 0, sizeof(root));
    strcpy(root, r);
    u32 low[3];
    low[0] = mediatype;
    *(u64*)(low + 1) = stuff;
    Result res = FSUSER_OpenArchive(&fswarch, arch, (FS_Path){PATH_BINARY, 0xC, (u8*)low});
    if(res < 0)
    {
        printf("[FSW_] Can't open archive: %08X\n", res);
        return res;
    }
    return 0;
}

void fsw_free()
{
    FSUSER_CloseArchive(fswarch);
}

FS_Archive fsw_get()
{
    return fswarch;
}

Result fsw_read2buf(const char* path, u64* size, char** bufptr)
{
    u64 temp;
    size_t len = strlen(root) + strlen(path);
    char concat[len + 1];
    memset(concat, 0, sizeof(concat));
    strcat(concat, root);
    strcat(concat, path);
    //printf("[FSW_] Reading file \"%s\"\n", concat);
    FS_Path pth = fsMakePath(PATH_ASCII, concat);
    
    if(!size)
    {
        size = &temp;
    }
    
    Handle filehand;
    
    Result res;
    res = FSUSER_OpenFile(&filehand, fswarch, pth, FS_OPEN_READ, FS_ATTRIBUTE_NONE);
    if(res < 0)
    {
        //printf("[FSW_] OpenFileDirectly error: %08X\n", res);
        //NNERR(res);
        return res;
    }
    
    res = FSFILE_GetSize(filehand, size);
    
    if(res < 0)
    {
        printf("[FSW_] GetSize error: %08X\n", res);
        NNERR(res);
        FSFILE_Close(filehand);
        return res;
    }
    
    //printf("[FSW_] Allocating %llu bytes\n", *size);
    
    void* outbuf = malloc(*size);
    
    if(!outbuf)
    {
        printf("[FSW_] Can't allocate buffer! %08X\n", -1);
        FSFILE_Close(filehand);
        return -1;
    }
    
    //printf("[FSW_] Buffer pointer: %p\n", outbuf);
    
    *bufptr = outbuf;
    
    u32 bytesread;
    
    res = FSFILE_Read(filehand, &bytesread, 0, outbuf, *size);
    if(res < 0)
    {
        //printf("[FSW_] Can't read file: %08X\n", res);
        //NNERR(res);
        FSFILE_Close(filehand);
        return res;
    }
    
    if(bytesread != *size)
    {
        printf("[FSW_] File sizes differ! %lu | %llu\n", bytesread, *size);
        FSFILE_Close(filehand);
        return -10;
    }
    
    FSFILE_Close(filehand);
    
    return 0;
}

Result fsw_scandir(const char* path, char argv[][262], u32* argc, char* ext)
{
    size_t len = strlen(root) + strlen(path);
    char concat[len + 1];
    memset(concat, 0, sizeof(concat));
    strcat(concat, root);
    strcat(concat, path);
    //printf("[FSW_] Reading directory \"%s\"\n", concat);
    FS_Path pth = fsMakePath(PATH_ASCII, concat);
    
    Handle dirhand = 0;
    
    Result res = FSUSER_OpenDirectory(&dirhand, fswarch, pth);
    if(res)
    {
        printf("[FSW_] Can't open directory: %08X\n", res);
        NNERR(res);
        return res;
    }
    
    FS_DirectoryEntry de;
    u32 read = 0;
    char namebuf[262];
    
    while(1)
    {
        res = FSDIR_Read(dirhand, &read, 1, &de);
        if(res < 0)
        {
            printf("[FSW_] Can't scan directory: %08X\n", res);
            NNERR(res);
            FSDIR_Close(dirhand);
            return res;
        }
        
        if(!read) break;
        
        memset(namebuf, 0, 262);
        utf16_to_utf8(namebuf, de.name, 262);
        
        //printf("[FSW_] > %s\n", namebuf);
        
        if(ext != 0 && ext != -1 && strcmp(namebuf + strlen(namebuf) - strlen(ext), ext)) continue;
        if(ext == -1) if(!(de.attributes & FS_ATTRIBUTE_DIRECTORY)) continue;

        memset(argv[*argc], 0, 262);
        if(de.attributes & FS_ATTRIBUTE_DIRECTORY) strcat(argv[*argc], "/");
        strcat(argv[*argc], namebuf);
        (*argc)++; // *argc++; // statement has no effect... wtf?!
    }
    
    FSDIR_Close(dirhand);
    
    return 0;
}

Result fsw_scandirent(const char* path, FS_DirectoryEntry* argv, u32* argc)
{
    size_t len = strlen(root) + strlen(path);
    char concat[len + 1];
    memset(concat, 0, sizeof(concat));
    strcat(concat, root);
    strcat(concat, path);
    //printf("[FSW_] Reading directory \"%s\"\n", concat);
    FS_Path pth = fsMakePath(PATH_ASCII, concat);
    
    Handle dirhand = 0;
    
    Result res = FSUSER_OpenDirectory(&dirhand, fswarch, pth);
    if(res < 0)
    {
        printf("[FSW_] Can't open directory: %08X\n", res);
        NNERR(res);
        return res;
    }
    
    u32 read = 0;
    
    while(1)
    {
        res = FSDIR_Read(dirhand, &read, 1, argv++);
        if(res < 0)
        {
            printf("[FSW_] Can't scan directory: %08X\n", res);
            NNERR(res);
            FSDIR_Close(dirhand);
            return res;
        }
        
        if(!read) break;
        
        (*argc)++;
    }
    
    res = FSDIR_Close(dirhand);
    
    return 0;
}

Result fsw_write2buf(const char* path, u32 size, char* bufptr)
{
    size_t len = strlen(root) + strlen(path);
    char concat[len + 1];
    memset(concat, 0, sizeof(concat));
    strcat(concat, root);
    strcat(concat, path);
    //printf("[FSW_] Writing file \"%s\"\n", concat);
    FS_Path pth = fsMakePath(PATH_ASCII, concat);

    Handle filehand;

    Result res;
    res = FSUSER_OpenFile(&filehand, fswarch, pth, FS_OPEN_WRITE, FS_ATTRIBUTE_NONE);
    if(res < 0)
    {
        printf("[FSW_] OpenFile error: %08X\n", res);
        NNERR(res);
        return res;
    }

    //printf("[FSW_] Buffer pointer: %p\n", bufptr);
    
    u32 read = 0;

    res = FSFILE_Write(filehand, &read, 0, bufptr, size, FS_WRITE_FLUSH);
    if(res < 0)
    {
        printf("[FSW_] Can't write to file: %08X\n", res);
        NNERR(res);
        FSFILE_Close(filehand);
        return res;
    }
    
    if(read != size)
    {
        printf("[FSW_] Size doesn't match! %d || %d\n", size, read);
        FSFILE_Close(filehand);
        return -1;
    }
    
    //puts("[FSW_] Flushing...");
    
    res = FSFILE_Flush(filehand);
    if(res < 0)
    {
        printf("[FSW_] Can't flush file contents: %08X\n", res);
        NNERR(res);
        FSFILE_Close(filehand);
        return res;
    }

    FSFILE_Close(filehand);
    
    return 0;
}

