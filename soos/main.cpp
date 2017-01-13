#include <3ds.h>

extern "C"
{
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <setjmp.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "fs/fsw.h"
}

#include <exception>

//int __stacksize__ = 0x18000;


extern "C"
{
#include "consoleshit.h"
#include "misc/nnerr.h"
/*
#include "fs/fsw.h"
#include "leveltools.h"
*/
}

#include "game/game.hpp"
#include "game/menu/debugmain.hpp"

using MM::Game;
using MM::GAME::MENU::DebugMain;

static jmp_buf __exc;
static int  __excno;
PrintConsole console;

#define max(x,y) ((x)<(y)?(y):(x))
#define min(x,y) ((x)<(y)?(x):(y))

//#define initdefault()\
({\
    /*puts("[MAIN] Initializing ExtFS");*/\
    res = fsw_init_bin(ARCHIVE_EXTDATA, MEDIATYPE_SD, extid, seldir);\
    if(res < 0) { NNERR(res); fsw_free(); hangmacro(); }\
})

//#define initsave()\
({\
    /*puts("[MAIN] Initializing SaveFS");*/\
    res = fsw_init_bin(ARCHIVE_USER_SAVEDATA, MEDIATYPE_SD, 0x0004000000000000ULL | (extid << 8), seldir);\
    if(res < 0) { NNERR(res); fsw_free(); hangmacro(); }\
})

//#define updatesave()\
({\
    FS_Archive ar = fsw_get();\
    res = FSUSER_ControlArchive(ar, ARCHIVE_ACTION_COMMIT_SAVE_DATA, 0, 0, 0, 0);\
    if(res < 0) { NNERR(res); fsw_free(); hangmacro(); }\
})

#define hangmacro()\
({\
    puts("Press a key to exit...");\
    while(aptMainLoop())\
    {\
        hidScanInput();\
        if(hidKeysDown())\
        {\
            goto killswitch;\
        }\
        gfxw_flushbuffer();\
        gfxw_wait4vblank();\
    }\
})

void _ded()
{
    gfxSetScreenFormat(GFX_TOP, GSP_RGB565_OES);
    gfxSetDoubleBuffering(GFX_TOP, false);
    gfxw_swapbuffer();
    gfxw_swapbuffer();
    gfxw_flushbuffer();
    
    puts("\e[0m\n\n- The application has crashed\n\n");
    
    try
    {
        throw;
    }
    catch(std::exception &e)
    {
        printf("std::exception: %s\n", e.what());
    }
    catch(Result res)
    {
        printf("Result: %08X\n", res);
        NNERR(res);
    }
    catch(int e)
    {
        printf("(int) %i\n", e);
    }
    catch(...)
    {
        puts("<unknown exception>");
    }
    
    puts("\n");
    
    hangmacro();
    
    killswitch:
    longjmp(__exc, 1);
}

int main()
{
  // =====[PROGINIT]=====
  
  gfxw_init();
  
  // =====[VARS]=====
  
  u32 kDown;
  u32 kHeld;
  u32 kUp;
  circlePosition cpos;
  touchPosition touch;
  
  int cx, cy;
  
  s16 deadX, deadY;
  deadX = 6;
  deadY = 6;
  
  u64 extid = 0;
  u64 saveid = 0;
  
  Result res = 0;
  
  Game* game = nullptr;
  
  Handle fsu = 0;
  
  u64 vl_eid[] = {0x1A04, 0x1A05};
  
  // =====[PREINIT]=====
  
  consoleInit(GFX_TOP, &console);
  puts("Hello!");
  
  osSetSpeedupEnable(1);
  
  puts("[MAIN] Initializing RomFS");
  
  res = romfsInit();
  if(res < 0)
  {
      printf("[MAIN] Failed to init RomFS: %08X\n", res);
  }
  
  APT_GetProgramID(&saveid);
  
  if(envIsHomebrew())
  {
      puts("[MAIN] Initializing custom FS handle");
      
      res = srvGetServiceHandleDirect(&fsu, "fs:USER");
      if(res < 0)
      {
          printf("Failed to get FS handle: %08X\n", res);
          hangmacro();
      }
      res = FSUSER_InitializeWithSdkVersion(fsu, 0xB0400C8);
      if(res < 0)
      {
          printf("Failed to initialize FS: %08X\n", res);
          hangmacro();
      }
      
      
      extid = (saveid >> 8) & 0xFFFF;
      
      for(cy = 0; cy != sizeof(vl_eid) / sizeof(vl_eid[0]); cy++)
      {
          if(extid == vl_eid[cy])
          {
              saveid = 0;
              
              break;
          }
      }
      
      if(saveid)
      {
          printf("Invalid ProgramID: %016llX\nPlease put OCDM.xml next to OCDM.3dsx!\n", saveid);
          hangmacro();
      }
  }
  else
  {
      puts("[MAIN] Searching for Mario Maker 3DS extdata...");
      
      for(cy = 0; cy != sizeof(vl_eid) / sizeof(vl_eid[0]); cy++)
      {
          if(fsw_init_bin(ARCHIVE_EXTDATA, MEDIATYPE_SD, vl_eid[cy], "/") >= 0)
          {
              fsw_free();
              
              extid = vl_eid[cy];
              
              break;
          }
          fsw_free();
      }
      
      if(extid)
      {
          saveid = 0x0004000000000000ULL | (extid << 8);
      }
      else
      {
          puts("\n\e[36mCan't find any Mario Maker 3DS extdata!\n" \
                  "Welp, fug... :/\n\e[0m");
          hangmacro();
      }
  }
  
  mkdir("/OCDM", 0777);
  mkdir("/OCDM/.cfg", 0777);
  mkdir("/OCDM/export", 0777);
  mkdir("/OCDM/conv", 0777);
  mkdir("/OCDM/backup", 0777);
  
  
  if((__excno = setjmp(__exc))) goto killswitch;
  
#ifdef _3DS
  std::set_unexpected(_ded);
  std::set_terminate(_ded);
#endif
  
  puts("[MAIN] Initializing game");
  
  game = new Game(kDown, kHeld, kUp, cpos, touch, extid, saveid, fsu);
  
  puts("[MAIN] Loading complete ^^");
  
  gspWaitForVBlank();
  gspWaitForVBlank();
  gspWaitForVBlank();
  
  sf2d_set_clear_color(RGBA8(0, 43, 54, 0xFF));
  sf2d_set_vblank_wait(0);
  //gfxSetScreenFormat(GFX_TOP, GSP_BGR8_OES);
  //gfxSetDoubleBuffering(GFX_TOP, true);
  
  game->PushMenu(new DebugMain);
  
  // =====[RUN]=====
  
  while (aptMainLoop())
  {
    hidScanInput();
    kDown = hidKeysDown();
    kHeld = hidKeysHeld();
    kUp = hidKeysUp();
    hidCircleRead(&cpos);
    if(kHeld & KEY_TOUCH) hidTouchRead(&touch);
    
    if(kHeld & KEY_SELECT)
    {
        break;
    }
    
    if(abs(cpos.dx) < deadX) cpos.dx = 0;
    if(abs(cpos.dy) < deadY) cpos.dy = 0;
    
    cx = game->Tick();
    
    if(cx)
    {
        printf("[MAIN] Game loop exception!\n> %08X\n", cx);
        if(cx == 0xFEEDDEAD) break;
        hangmacro();
    }
    
    game->Render();
    
    if(kDown & KEY_B) game->PopMenuAuto();
    
    /*
    if(kDown & (KEY_A | KEY_START))
                {
                    cy = kDown & KEY_START ? 0: s->files.sel;
                    int le = kDown & KEY_START ? s->files.cnt : s->files.sel + 1;
                    
                    for(; cy != le; cy++)
                    {
                        char buf[0x80];
                        
                        if(_scr == SCR_EXPORT)
                        {
                            snprintf(buf, sizeof(buf), "/OCDM/export/course%03i", s->files.levels[cy].nth);
                            FILE* f = fopen(buf, "wb");
                            if(f <= 0) continue;
                            u8* bufptr = 0;
                            u64 size;
                            snprintf(buf, sizeof(buf), "course%03i", s->files.levels[cy].nth);
                            initdefault();
                            res = fsw_read2buf(buf, &size, (char**)&bufptr);
                            fsw_free();
                            if(bufptr)
                            {
                                fwrite(bufptr, 1, size, f);
                                fflush(f);
                                
                                u32 n = 0xFFFFFFFF;
                                u8* ptr = bufptr + 0x1C;
                                for(cx = 0x2A000; cx; cx--)
                                {
                                    n = n ^ *ptr++;
                                    int j;
                                    for(j = 0; j != 8; j++)
                                        if(n & 1)
                                            n = (n >> 1) ^ 0xEDB88320; 
                                        else
                                            n >>= 1;
                                    n &= 0xFFFFFFFF;
                                }
                                
                                printf("Success #%03i\n", cy);
                                printf("CRC32: %08X %08X %08X\n", *(u32*)(bufptr), ~n, n);
                                
                                free(bufptr);
                            }
                            else printf("Fail #%03i %08X\n", cy, res);
                            
                            fclose(f);
                            
                            continue;
                        }
                        
                        if(s->files.levels[cy].nth == 220) continue;
                        
                        snprintf(buf, sizeof(buf), "/OCDM/conv/course%03i", s->files.levels[cy].nth);
                        mkdir(buf, 0777);
                        u8* bufptr = 0;
                        u64 size;
                        snprintf(buf, sizeof(buf), "course%03i", s->files.levels[cy].nth);
                        initdefault();
                        res = fsw_read2buf(buf, &size, (char**)&bufptr);
                        fsw_free();
                        u8* obufptr = bufptr;
                        if(bufptr)
                        {
                            bufptr += 0x1C;
                            
                            lt_mkbe(bufptr);
                            lt_mkbe(bufptr + 0x15000);
                            
                            snprintf(buf, sizeof(buf), "/OCDM/conv/course%03i/course_data.cdt", s->files.levels[cy].nth);
                            FILE* f = fopen(buf, "wb");
                            fwrite(bufptr, 1, 0x15000, f);
                            fflush(f);
                            fclose(f);
                            bufptr += 0x15000;
                            
                            snprintf(buf, sizeof(buf), "/OCDM/conv/course%03i/course_data_sub.cdt", s->files.levels[cy].nth);
                            f = fopen(buf, "wb");
                            fwrite(bufptr, 1, 0x15000, f);
                            fflush(f);
                            fclose(f);
                            bufptr += 0x15000;
                            
                            snprintf(buf, sizeof(buf), "/OCDM/conv/course%03i/thumbnail.bin", s->files.levels[cy].nth);
                            f = fopen(buf, "wb");
                            fwrite(bufptr, 1, 0x157C0, f);
                            fflush(f);
                            fclose(f);
                            bufptr += 0x157C0;
                            
                            snprintf(buf, sizeof(buf), "/OCDM/conv/course%03i/dummy.bin", s->files.levels[cy].nth);
                            f = fopen(buf, "wb");
                            fwrite(bufptr, 1, 0x3390, f);
                            fflush(f);
                            fclose(f);
                        }
                        
                        if(obufptr) free(obufptr);
                    }
                    
                    puts("Exporting finished");
                }

            
            if(redrva)
            {
                console.cursorX = 0;
                console.cursorY = 2;
                printf("\e[K > Magik: %08X | LockoutID: %016llX\n\n", s->prog.magik, s->prog.ID);
                
                if(s->files.cnt)
                {
                    cx = s->files.sel - ((console.consoleHeight - 6) / 2);
                    cy = cx + console.consoleHeight - 6;
                    
                    for(; cx != cy; cx++)
                    {
                        fputs("\e[K", stdout);
                        if(cx >= 0 && cx < s->files.cnt)
                            printf(" %s #%03i %s (by %s)", (s->files.sel == cx ? "Í" : "  "), s->files.levels[cx].nth, s->files.levels[cx].lvlname, s->files.levels[cx].miin);
                        puts("");
                    }
                }
                else
                {
                    puts("\n\n\n   * No files found *");
                }
                
                console.cursorX = 1;
                console.cursorY = console.consoleHeight - 1;
                
                printf("\e[36m[Select] Exit\e[0m \e[31m[A] Save selected\e[0m \e[37m[Start] Save all\e[0m ");
                
                redrva = 0;
            }
            
            if(kDown & KEY_X)
            {
                initsave();
                
                u8* bufptr = 0;
                u64 size = 0;
                fsw_read2buf("Progress", &size, (char**)&bufptr);
                if(bufptr)
                {
                    FILE* f = fopen("/OCDM/backup/Progress", "wb");
                    if(f > 0)
                    {
                        fwrite(bufptr, 1, size, f);
                        fflush(f);
                        fclose(f);
                    }
                    free(bufptr);
                }
                
                fsw_free();
            }
            
            if(kDown & KEY_Y)
            {
                FILE* f = fopen("/OCDM/backup/Progress", "rb");
                if(f > 0)
                {
                    initsave();
                    
                    u8* bufptr = 0;
                    u64 size = 0;
                    fsw_read2buf("Progress", &size, (char**)&bufptr);
                    
                    if(bufptr)
                    {
                        fread(bufptr, 1, size, f);
                        fsw_write2buf("Progress", size, (char*)bufptr);
                        updatesave();
                        free(bufptr);
                    }
                    
                    fsw_free();
                    
                    fclose(f);
                }
            }
            
            if((kHeld & KEY_A) && (kDown & KEY_START))
            {
                if(!s->files.cnt) goto scandatadir;
                
                initsave();
                
                u8* bufptr = 0;
                u64 size = 0;
                fsw_read2buf("Progress", &size, (char**)&bufptr);
                
                if(bufptr)
                {
                    u8 i = s->files.cnt;
                    memset(s->prog.slots, 0xFF, 120);
                    while(1)
                    {
                        if(i == 0) break;
                        i--;
                        
                        u8 sn = s->files.levels[i].nth;
                        int sln = rand() % 120;
                        while(1)
                        {
                            if(s->prog.slots[sln] == 0xFF)
                            {
                                s->prog.slots[sln] = sn;
                                break;
                            }
                            else if(sln++ == 120) sln = 0;
                        }
                    }
                    for(cy = 0; cy != 120; cy++)
                        *(bufptr + 0x2268 + cy) = s->prog.slots[cy];
                    
                    printf("write2buf: %08X\n", fsw_write2buf("Progress", size, (char*)bufptr));
                    
                    updatesave();
                    
                    free(bufptr);
                }
                
                fsw_free();
            }
            
            if(redrva)
            {
                console.cursorX = 0;
                console.cursorY = 2;
                
                printf("\e[K > Hash: %08X | LockoutID: %016llX\n\n\e[K", s->prog.magik, s->prog.ID);
                
                u8* ptr = s->prog.slots;
                for(cy = 0; cy != 15; cy++)
                {
                    printf("%02X %02X %02X %02X | %02X %02X %02X %02X\n", ptr[0], ptr[1], ptr[2], ptr[3], ptr[60], ptr[61], ptr[62], ptr[63]);
                    ptr += 4;
                }
                
                console.cursorX = 1;
                console.cursorY = console.consoleHeight - 1;
                
                printf("\e[34m[X] Backup\e[0m \e[32m[Y] Restore\e[0m [\e[31mA\e[0m+\e[36mSTART\e[0m] Fuzz slots");
                
                redrva = 0;
            }
        
        
            if(redrva)
            {
                console.cursorX = 0;
                console.cursorY = 3;
                
                printf("C%cedits:\n\n\n", ((svcGetSystemTick() >> 1) & 0xF) ? 'r' : 'l');
                
                puts("Sonomi-nyan~ :<     -- programmer / REer");
                puts("smb123w64gb         -- original save conversion code");
                puts("CeeDee @ GBATemp    -- application logo");
                puts("SomeGamer @ GBATemp -- course LockoutID \"exploit\"");
                
                redrva = 0;
            }
        if(kDown & KEY_B)
            {
                _scr = SCR_MAIN;
                redraw();
                goto screenswitch;
            }
            
            if(redrva)
            {
                console.cursorX = 1;
                console.cursorY = 3;
                
                printf("Unhandled screen# %i\n", _scr);
                redrva = 0;
            }
            */
    
    gfxw_flushbuffer();
    gfxw_swapbuffer();
    gfxw_wait4vblank();
  }

  // =====[END]=====
  
  killswitch:
  
  if(game) delete game;
  
  if(fsu)
  {
      fsEndUseSession();
      svcCloseHandle(fsu);
  }
  
  gfxw_exit();
  
  return 0;
}
