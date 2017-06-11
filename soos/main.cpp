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

#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <exception>

#include "misc/ini.hpp"

using std::string;
using std::fstream;
using std::cout;
using std::endl;
using MM::Ini;

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
        //NNERR(res);
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
  
  Ini ini;
  
  Handle fsu = 0;
  
  u64 vl_eid[] = {0x1A03, 0x1A04, 0x1A05};
  
  // =====[PREINIT]=====
  
  consoleInit(GFX_TOP, &console);
  puts("Hello!");
  
  osSetSpeedupEnable(1);
  
  puts("[MAIN] Loading config");
  
  do
  {
      FILE* f = fopen("/OCDM/.cfg/main.ini", "r");
      if(f <= 0)
      {
          f = fopen("/OCDM/.cfg/main.ini", "w");
          if(f > 0)
          {
              fputs("\n", f);
              fflush(f);
              fclose(f);
          }
      }
      else fclose(f);
      
      
      fstream fo("/OCDM/.cfg/main.ini", fstream::in);
      
      fo >> ini;
      
      fo.close();
  }
  while(0);
  
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
  mkdir("/OCDM/snapshot", 0777);
  mkdir("/OCDM/download", 0777);
  
  if((__excno = setjmp(__exc))) goto killswitch;
  
#ifdef _3DS
  std::set_unexpected(_ded);
  std::set_terminate(_ded);
#endif
  
  puts("[MAIN] Initializing game");
  
  game = new Game(kDown, kHeld, kUp, cpos, touch, extid, saveid, fsu, ini);
  
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
    
    if((kDown & KEY_B) && game->menu->HandleBack()) game->PopMenuAuto();
    
    gfxw_flushbuffer();
    gfxw_swapbuffer();
    gfxw_wait4vblank();
  }
  
  do
  {
      fstream fo("/OCDM/.cfg/main.ini", fstream::out);
      
      fo << ini << endl;
      
      fo.close();
  }
  while(0);

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
