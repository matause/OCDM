#include <3ds.h>
#include <stdlib.h>

#include "grafx/drawer.h"

static int currscreen = -1;

s32 __screenw = 1;
s32 __screenh = 1;

Result gfxw_init()
{
    Result res = sf2d_init();
    if(res < 0) return res;
    
    return res;
}

Result gfxw_exit()
{
    return sf2d_fini();
}

gfxw_texture* gfxw_rgba2tex(u8* tex, u16 w, u16 h)
{
    return sf2d_create_texture_mem_RGBA8(tex, w, h, TEXFMT_RGBA8, SF2D_PLACE_RAM);
}

void gfxw_freetex(gfxw_texture* tex)
{
    sf2d_free_texture(tex);
}

void gfxw_addflag(IIcon* icon, gfxw_texflag flag)
{
    icon->flags |= flag;
}

void gfxw_delflag(IIcon* icon, gfxw_texflag flag)
{
    icon->flags &= ~flag;
}

void gfxw_drawicon(IIcon* icon, s16 x, s16 y)
{
    float sx = 1.0F;
    float sy = 1.0F;
    
    if(icon->flags & TEXFLAG_FLIPX)
    {
        sx = -1.0F;
        x += icon->w;
    }
    if(icon->flags & TEXFLAG_FLIPY)
    {
        sy = -1.0F;
        y += icon->h;
    }
    
    sf2d_draw_texture_part_scale(icon->img, x, y, 0, 0, icon->w, icon->h, sx, sy);
}

void gfxw_drawicona(IIcon* icon, s16 x, s16 y, u8 a)
{
    float sx = 1.0F;
    float sy = 1.0F;
    
    if(icon->flags & TEXFLAG_FLIPX)
    {
        sx = -1.0F;
        x += icon->w;
    }
    if(icon->flags & TEXFLAG_FLIPY)
    {
        sy = -1.0F;
        y += icon->h;
    }
    
    sf2d_draw_texture_part_scale_blend(icon->img, x, y, 0, 0, icon->w, icon->h, sx, sy, 0x00FFFFFF | (a << 24));
}

void gfxw_drawiconc(IIcon* icon, s16 x, s16 y, u32 color)
{
    float sx = 1.0F;
    float sy = 1.0F;
    
    if(icon->flags & TEXFLAG_FLIPX)
    {
        sx = -1.0F;
        x += icon->w;
    }
    if(icon->flags & TEXFLAG_FLIPY)
    {
        sy = -1.0F;
        y += icon->h;
    }
    
    sf2d_draw_texture_part_scale_blend(icon->img, x, y, 0, 0, icon->w, icon->h, sx, sy, color);
}

void gfxw_drawiconp(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th)
{
    float sx = 1.0F;
    float sy = 1.0F;
    
    if(icon->flags & TEXFLAG_FLIPX)
    {
        sx = -1.0F;
        x += icon->w;
    }
    if(icon->flags & TEXFLAG_FLIPY)
    {
        sy = -1.0F;
        y += icon->h;
    }
    
    sf2d_draw_texture_part_scale(icon->img, x, y, tx, ty, tw, th, sx, sy);
}

void gfxw_drawiconpa(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th, u8 a)
{
    float sx = 1.0F;
    float sy = 1.0F;
    
    if(icon->flags & TEXFLAG_FLIPX)
    {
        sx = -1.0F;
        x += icon->w;
    }
    if(icon->flags & TEXFLAG_FLIPY)
    {
        sy = -1.0F;
        y += icon->h;
    }
    
    sf2d_draw_texture_part_scale_blend(icon->img, x, y, tx, ty, tw, th, sx, sy, 0x00FFFFFF | (a << 24));
}

void gfxw_drawiconpc(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th, u32 color)
{
    float sx = 1.0F;
    float sy = 1.0F;
    
    if(icon->flags & TEXFLAG_FLIPX)
    {
        sx = -1.0F;
        x += icon->w;
    }
    if(icon->flags & TEXFLAG_FLIPY)
    {
        sy = -1.0F;
        y += icon->h;
    }
    
    sf2d_draw_texture_part_scale_blend(icon->img, x, y, tx, ty, tw, th, sx, sy, color);
}

void gfxw_drawline(s16 x1, s16 y1, s16 x2, s16 y2, int color)
{
    sf2d_draw_line(x1, y1, x2, y2, color);
}

void gfxw_drawrect(s16 x, s16 y, s16 w, s16 h, int color)
{
    sf2d_draw_line(x, y, x + w - 1, y, color);
    sf2d_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    sf2d_draw_line(x, y, x, y + h - 1, color);
    sf2d_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void gfxw_fillrect(s16 x, s16 y, s16 w, s16 h, int color)
{
    if(!(color >> 24)) color |= 0xFF << 24;
    sf2d_draw_rectangle(x, y, w, h, color);
}

void gfxw_swapbuffer()
{
    sf2d_swapbuffers();
}

void gfxw_flushbuffer()
{
    gfxFlushBuffers();
}

void gfxw_wait4vblank()
{
    gspWaitForVBlank();
}

void gfxw_startscreen(int screen)
{
    currscreen = screen;
    __screenw = screen ? 320 : 400;
    __screenh = 240;
    sf2d_start_frame(screen, GFX_LEFT);
}

void gfxw_endscreen()
{
    currscreen = -1;
    __screenw = 1;
    __screenh = 1;
    sf2d_end_frame();
}

int gfxw_getscreen()
{
    return currscreen;
}
