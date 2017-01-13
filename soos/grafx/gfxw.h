#pragma once


#include <3ds.h>
#include <sf2d/sf2d.h>

extern s32 __screenw;
extern s32 __screenh;

#define SCREENW __screenw
#define SCREENH __screenh

typedef sf2d_texture gfxw_texture;

#define TOPW 400
#define TOPH 240
#define BOTW 320
#define BOTH 240

typedef enum
{
    TEXFLAG_NONE = 0,
    TEXFLAG_FLIPX = 1,
    TEXFLAG_FLIPY = 2
} gfxw_texflag;

typedef struct
{
     u16 w;
     u16 h;
     gfxw_texflag flags;
     gfxw_texture* img;
} IIcon;

extern IIcon* pixel;

Result gfxw_init();
Result gfxw_exit();

gfxw_texture* gfxw_rgba2tex(u8* tex, u16 w, u16 h);
void gfxw_freetex(gfxw_texture* tex);

void gfxw_addflag(IIcon* icon, gfxw_texflag flag);
void gfxw_delflag(IIcon* icon, gfxw_texflag flag);

void gfxw_drawicon(IIcon* icon, s16 x, s16 y);
void gfxw_drawicona(IIcon* icon, s16 x, s16 y, u8 a);
void gfxw_drawiconc(IIcon* icon, s16 x, s16 y, u32 color);

void gfxw_drawiconp(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th);
void gfxw_drawiconpa(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th, u8 a);
void gfxw_drawiconpc(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th, u32 color);

void gfxw_drawline(s16 x1, s16 y1, s16 x2, s16 y2, int color);
void gfxw_drawrect(s16 x, s16 y, s16 w, s16 h, int color);
void gfxw_fillrect(s16 x, s16 y, s16 w, s16 h, int color);

void gfxw_swapbuffer();
void gfxw_flushbuffer();
void gfxw_wait4vblank();

void gfxw_startscreen(int screen);
void gfxw_endscreen();
int gfxw_getscreen();
