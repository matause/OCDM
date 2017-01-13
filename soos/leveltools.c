#include <3ds.h>

#include "leveltools.h"

#define SWAP16(o) *(u16*)(bufptr + o) = __builtin_bswap16(*(u16*)(bufptr + o))
#define SWAP32(o) *(u32*)(bufptr + o) = __builtin_bswap32(*(u32*)(bufptr + o))
#define SWAP64(o)\
do\
{\
    SWAP32(o + 0);\
    SWAP32(o + 4);\
    u32 __swap__dummy__ = *(u32*)(bufptr + o + 0);\
    *(u32*)(bufptr + o + 0) = *(u32*)(bufptr + o + 4);\
    *(u32*)(bufptr + o + 4) = __swap__dummy__;\
}\
while(0);

u32 crc32(u8* ptr, u32 i, u32 n)
{
    for(; i; i--)
    {
        n = n ^ *ptr++;
        int j;
        for(j = 0; j != 8; j++)
            if(n & 1)
                n = (n >> 1) ^ 0xEDB88320; 
            else
                n >>= 1;
    }
    
    return n;
}

void lt_chend(u8* bufptr)
{
    int i = 0;
    
    //SWAP32(0x04);
    SWAP16(0x10);
    
    SWAP64(0x18);
    
    SWAP32(0x24);
    
    for(i = 0x28; i != 0x68; i += 2)
        SWAP16(i);
    
    SWAP16(0x70);
    SWAP32(0x74);
    SWAP32(0xD8);
    SWAP32(0xDC);
    SWAP32(0xEC);
    SWAP32(0xFC);
    
    for(i = 0xF0; i != 0x145F0; i += 0x20)
    {
        SWAP32(i + 0x0);
        SWAP32(i + 0x4);
        SWAP16(i + 0x8);
        SWAP32(i + 0xC);
        SWAP32(i + 0x10);
        SWAP32(i + 0x14);
        SWAP16(i + 0x1A);
        SWAP16(i + 0x1C);
    }
    
    SWAP32(0x14F50);
    SWAP32(0x14F54);
    SWAP32(0x14F58);
    SWAP32(0x14F5C);
    
    /*for(i = 0x145F6; i != 0x15006; i += 8)
        SWAP16(i);*/
}

void lt_mkle(u8* ptr)
{
    if(!*(u16*)(ptr + 4)) lt_chend(ptr);
    *(u32*)(ptr + 8) = ~crc32(ptr + 0x10, 0x15000 - 0x10, 0xFFFFFFFF);
}

void lt_mkbe(u8* ptr)
{
    if(!*(u16*)(ptr + 6)) lt_chend(ptr);
    *(u32*)(ptr + 8) = __builtin_bswap32(crc32(ptr + 0x10, 0x15000 - 0x10, 0));
}

void lt_parsele(LevelEntry* e, u8 cy, u8* bufptr)
{
    
    if(cy == 220)
    {
        strcpy(e->lvlname, "<LockoutDummy220>");
        strcpy(e->miin, "Nintendo");
        strcpy(e->miia, "Nintendo");
        e->code2 = -1;
        e->nth = cy;
        e->lock = -1ULL;
    }
    else 
    {
        utf16_to_utf8(e->lvlname, bufptr + 0x44, sizeof(e->lvlname) - 1);
        utf16_to_utf8(e->miin, bufptr + 0xAE, sizeof(e->miin) - 1);
        utf16_to_utf8(e->miia, bufptr + 0xDC, sizeof(e->miia) - 1);
        e->code2 = *(u32*)(bufptr + 0x38);
        e->nth = cy;
        e->lock = *(u64*)(bufptr + 0x10);
    }
}


