#pragma once

typedef struct
{
    u32 MiiID;
    u64 SysID;
    u32 MiiDate;
    u8 MAC[6];
    u16 padding_zero0;
    u16 Mii_opt;
    u16 name[10];
    u16 size;
    u8 design[0x10];
    u8 copying;
    u8 padding_zero1[7];
    u16 authorname[10];
    u32 something;
} __attribute__((packed)) Mii;
