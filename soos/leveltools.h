#pragma once

#include "misc/mii.h"

#define FILESIZE_COURSE 0x4301C
#define FILESIZE_PROGRESS 0x8018
#define FILESIZE_DUMMY 0x1C1C

#define ADDIFF_COURSE 0x4F014
#define ADDIFF_PROGRESS 0x8000
#define ADDIFF_DUMMY 0x8C04

//below struct member names were found out by @smb123w64gb
typedef struct
{
    u8 type;                  // 0000
    u8 unknown0;              // 0001
    u8 subtype;               // 0002
    u8 x;                     // 0003
    u8 z;                     // 0004
    u8 unknown1[3];           // 0005
} MM_Effect;

typedef struct
{
    u32 x;                    // 0000
    u32 z;                    // 0004
    u16 y;                    // 0008
    u8 w;                     // 000A
    u8 h;                     // 000B
    u32 flags;                // 000C
    u32 childflags;           // 0010
    u32 NBT;                  // 0014
    u8 type;                  // 0018
    u8 child;                 // 0019
    u16 link;                 // 001A
    u16 effect;               // 001C
    u8 unknown;               // 001E
    u8 childtrans;            // 001F
} __attribute__((packed)) MM_Tile;

typedef struct
{
    u32 unk_zero;             // 0000
    u32 filever;              // 0004
    u32 crc;                  // 0008
    u32 padding_zero0;        // 000C
    u16 f_year;               // 0010
    u8 f_month;               // 0012
    u8 f_day;                 // 0013
    u8 f_hour;                // 0014
    u8 f_minute;              // 0015
    u16 padding_zero1;        // 0016
    u32 padding_arr0[4];      // 0018
    u16 lvlname[0x21];        // 0028
    u16 theme;                // 006A
    u8 padding_zero2;         // 006C
    u8 leveltype;             // 006D
    u16 padding_zero3;        // 006E
    u16 lvltime;              // 0070
    u8 scroll;                // 0072
    u8 flags;                 // 0073
    u32 lvlsize;              // 0074
    Mii creator;              // 0078
    u32 unknown0[6];          // 00D4
    u32 numtiles;             // 00EC
    MM_Tile tiles[2600];      // 00F0 //how fucking generous
    MM_Effect sfx[300];       //145F0 //again, so fucking generous ¬_¬
    u8 unknown_padding[0xB0]; //14F50
} __attribute__((packed)) MM_Course;



typedef struct
{
    u32 checksum;
    u32 unknown1[3];
    u64 LockoutID;
} __attribute__((packed)) MM_LockoutHdr;

typedef struct
{
    MM_LockoutHdr hdr;
    u32 dummypadding;
    MM_Course course;
    MM_Course subcourse;
    u8 dummy_thumbnail[0x157C0];
    u32 dummy_padding[0xCE4];
} __attribute__((packed)) MM_CourseFile;



typedef struct
{
    int nth;
    char lvlname[0x41];
    char miin[13];
    char miia[13];
    u32 code2;
    u64 lock;
} LevelEntry;

u32 crc32(u8* ptr, u32 i, u32 n);
u32 addcrc(u16* ptr, u32 i, u32 n);
void lt_chend(MM_Course* course);
void lt_mkle(MM_Course* course);
void lt_mkbe(MM_Course* course);
void lt_parsele(LevelEntry* le, u8 cy, u8* ptr);
