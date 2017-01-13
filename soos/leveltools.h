#pragma once

#include <3ds.h>


typedef struct
{
    int nth;
    char lvlname[0x41];
    char miin[17];
    char miia[17];
    u32 code2;
    u64 lock;
} LevelEntry;

u32 crc32(u8* ptr, u32 i, u32 n);
void lt_chend(u8* bufptr);
void lt_mkle(u8* ptr);
void lt_mkbe(u8* ptr);
void lt_parsele(LevelEntry* le, u8 cy, u8* ptr);
