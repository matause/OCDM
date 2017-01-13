#pragma once

#include <3ds.h>

#include "gfxw.h"

int loadicon(char* path, IIcon* icon);
int loadtxmap(char* path, IIcon*** icon, u16* count);
int loadanimap(char* path, IIcon*** icon, u16* count, u8** a, u16* anicount);
int loadiconbuf(const u8* buf, u32 size, IIcon* icon);
void unloadicon(IIcon* icon);
void drawicon(IIcon* icon, s16 x, s16 y);
void drawiconr(IIcon* icon, s16 x, s16 y, int mode);
void drawicona(IIcon* icon, s16 x, s16 y, u8 a);
void drawiconar(IIcon* icon, s16 x, s16 y, u8 a, int mode);
void drawiconac(IIcon* icon, s16 x, s16 y, u32 color);
void drawiconp(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th);
void drawiconpa(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th, u8 a);
void drawiconpac(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th, u32 color);
