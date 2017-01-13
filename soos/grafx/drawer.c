#include <3ds.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "grafx/drawer.h"
#include "math/util.h"

#include "fs/fsw.h"

#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "misc/stb_image.h"


void unloadicon(IIcon* icon)
{
    gfxw_freetex(icon->img);
    icon->img = 0;
    free(icon);
}

int loadiconbuf(const u8* buf, u32 size, IIcon* icon)
{
    memset(icon, 0, sizeof(IIcon));
    
    void* ob = 0;
    ob = stbi_load_from_memory(buf, size, &icon->w, &icon->h, 0, 4);
            
    if(!ob)
    {
        puts("[STBI] Image load error");
        return 1;
    }
    
    icon->img = gfxw_rgba2tex(ob, icon->w, icon->h);
    free(ob);
    if(!icon->img)
    {
        printf("[DRAW] Can't load image, out of VRAM");
        return -1;
    }
    
    return 0;
}

int loadicon(char* path, IIcon* icon)
{
    u8* buf;
    u64 size = 0;
    
    FILE* f = fopen(path, "rb");
    if(f <= 0)
    {
        printf("[DRAW] Can't load image: (%i) %s\n> %s\n", errno, strerror(errno), path);
        return errno;
    }
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    buf = malloc(size);
    if(!buf)
    {
        fclose(f);
        puts("[DRAW] Out of memory");
        return -1;
    }
    fseek(f, 0, SEEK_SET);
    fread(buf, 1, size, f);
    fclose(f);
    
    memset(icon, 0, sizeof(IIcon));
    
    int ret = loadiconbuf(buf, size, icon);
    free(buf);
    return ret;
}

int loadtxmap(char* path, IIcon*** icon, u16* count)
{
    u16* bufptr;
    u64 size = 0;
    
    printf("[DRAW] Loading texture map:\n> %s \n", path);
    
    int res;
    
    FILE* f = fopen(path, "rb");
    if(f <= 0)
    {
        printf("[DRAW] Can't load image: (%i) %s\n> %s\n", errno, strerror(errno), path);
        return errno;
    }
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    bufptr = malloc(size);
    if(!bufptr)
    {
        fclose(f);
        puts("[DRAW] Out of memory");
        return -1;
    }
    fseek(f, 0, SEEK_SET);
    fread(bufptr, 1, size, f);
    fclose(f);
    
    u16 cnt = *bufptr;
    if(count) *count = cnt;
    
    printf("[DRAW] Icon count: %u \n", cnt);
    
    IIcon** icn = malloc(cnt * sizeof(IIcon*));
    
    if(!icn) return -2;
    
    *icon = icn;
    
    u16 iter = 0;
    
    u8* ptr = bufptr + 1;
    
    while(iter != cnt)
    {
        IIcon* iii = malloc(sizeof(IIcon));
        if(!iii)
        {
            puts("[DRAW] Can't allocate IIcon");
        }
        
        memset(iii, 0, sizeof(IIcon));
        
        *icn = iii;
        
        u32 fsize = *((u32*)ptr);
        
        ptr += 4;
        
        if(loadiconbuf(ptr, fsize, iii))
        {
            free(iii);
            *icn = 0;
            puts("[DRAW] Skipping invalid texture");
        }
        
        icn++;
        
        ptr += fsize;
        
        iter++;
    }
    
    free(bufptr);
    
    printf("[DRAW] %u icons loaded\n", iter);
    
    return 0;
}

int loadanimap(char* path, IIcon*** icon, u16* count, u8** a, u16* anicount)
{
    u8* buf;
    u64 size = 0;
    
    printf("[DRAW] Loading animation map:\n> %s \n", path);
    
    int res;
    
    FILE* f = fopen(path, "rb");
    if(f <= 0)
    {
        printf("[DRAW] Can't load image: (%i) %s\n> %s\n", errno, strerror(errno), path);
        return errno;
    }
    
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    buf = malloc(size);
    if(!buf)
    {
        fclose(f);
        puts("[DRAW] Out of memory");
        return -1;
    }
    fseek(f, 0, SEEK_SET);
    fread(buf, 1, size, f);
    fclose(f);
    
    u8* bzf = buf;
    
    u16 anicnt = *((u16*)bzf);
    
    printf("[DRAW] Animation frame count: %i\n", anicnt);
    
    bzf += 2;
    
    u8* al = malloc(anicnt);
    
    for(res = 0; res != anicnt; res++)
    {
        al[res] = *(bzf++);
    }
    
    res = 0;
    
    *a = al;
    *anicount = anicnt;
    
    u16 cnt = *((u16*)(bzf));
    if(count) *count = cnt;
    
    printf("[DRAW] Icon count: %u \n", cnt);
    
    IIcon** icn = malloc(cnt * sizeof(IIcon*));
    
    if(!icn) return -2;
    
    *icon = icn;
    
    u16 iter = 0;
    
    u8* ptr = (u8*)(bzf + 2);
    
    while(iter != cnt)
    {
        IIcon* iii = malloc(sizeof(IIcon));
        if(!iii)
        {
            puts("[DRAW] Can't allocate IIcon");
        }
        
        memset(iii, 0, sizeof(IIcon));
        
        *(icn++) = iii;
        
        u32 fsize = *((u32*)ptr);
        
        ptr += 4;
        
        loadiconbuf(ptr, fsize, iii);
        
        ptr += fsize;
        
        iter++;
    }
    
    free(buf);
    
    printf("[DRAW] %u icons loaded\n", iter);
    
    return 0;
}

void drawicon(IIcon* icon, s16 x, s16 y) 
{
    gfxw_drawicon(icon, x, SCREENH - (y + icon->h));
}

void drawiconr(IIcon* icon, s16 x, s16 y, int mode)
{
    int startx = (mode & 1) ? x % icon->w : x;
    int starty = (mode & 2) ? y % icon->h : y;
    int posx = startx;
    int posy;
    
    while(posx < SCREENW)
    {
        posy = starty;
        while(posy < SCREENH)
        {
            drawicon(icon, posx, posy);
            
            if(!(mode & 2)) break;
            
            posy += icon->h;
        }
        posx += icon->w;
        
        if(!(mode & 1)) break;
    }
}

void drawicona(IIcon* icon, s16 x, s16 y, u8 a)
{
    gfxw_drawicona(icon, x, SCREENH - (y + icon->h), a);
}

void drawiconar(IIcon* icon, s16 x, s16 y, u8 a, int mode)
{
    int startx = (mode & 1) ? x % icon->w : x;
    int starty = (mode & 2) ? y % icon->h : y;
    int posx = startx;
    int posy;
    
    while(posx < SCREENW)
    {
        posy = starty;
        while(posy < SCREENH)
        {
            drawicona(icon, posx, posy, a);
            
            if(!(mode & 2)) break;
            
            posy += icon->h;
        }
        posx += icon->w;
        
        if(!(mode & 1)) break;
    }
}

void drawiconac(IIcon* icon, s16 x, s16 y, u32 color)
{
    gfxw_drawiconc(icon, x, SCREENH - (y + icon->h), color);
}

void drawiconp(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th)
{
    gfxw_drawiconp(icon, x, SCREENH - (y + icon->h), tx, ty, tw, th);
}

void drawiconpa(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th, u8 a)
{
    gfxw_drawiconpa(icon, x, SCREENH - (y + icon->h), tx, ty, tw, th, a);
}

void drawiconpac(IIcon* icon, s16 x, s16 y, u16 tx, u16 ty, u16 tw, u16 th, u32 color)
{
    gfxw_drawiconpc(icon, x, SCREENH - (y + icon->h), tx, ty, tw, th, color);
}
