
#include "nnerr.h"

//marcus@Werkstaetiun:~/Asztal$ ./enumfucker.lua ./CTR_SDK/include/nn/Result.h | sed 's/\"\([^"]*\)\"/\"\U\1\"/g'
char* nnerr_level(u32 input)
{
    switch (input & 0x1F)
    {
        case 1: return "INFO";
        case 0: return "SUCCESS";
        case 31: return "FATAL";
        case 30: return "RESET";
        case 29: return "REINIT";
        case 28: return "USAGE";
        case 27: return "PERMANENT";
        case 26: return "TEMPORARY";
        case 25: return "STATUS";
        case 24: return "END";
        default: return "???";
    }
}

char* nnerr_summary(u32 input)
{
    switch (input & 0x3F)
    {
        case 0: return "SUCCESS";
        case 1: return "NOTHING HAPPENED";
        case 2: return "WOULD BLOCK";
        case 3: return "OUT OF RESOURCE";
        case 4: return "NOT FOUND";
        case 5: return "INVALID STATE";
        case 6: return "NOT SUPPORTED";
        case 7: return "INVALID ARGUMENT";
        case 8: return "WRONG ARGUMENT";
        case 9: return "CANCELLED";
        case 10: return "STATUS CHANGED";
        case 11: return "INTERNAL ERROR";
        case 63: return "INVALID RESULT VALUE";
        default: return "UNKNOWN ERROR";
    }
}

char* nnerr_module(u32 input)
{
    switch (input & 0xFF)
    {
        case 0: return "COMMON";
        case 1: return "KERNEL";
        case 2: return "UTIL";
        case 3: return "FILE SERVER";
        case 4: return "LOADER SERVER";
        case 5: return "TCB";
        case 6: return "OS";
        case 7: return "DBG";
        case 8: return "DMNT";
        case 9: return "PDN";
        case 10: return "GX";
        case 11: return "I2C";
        case 12: return "GPIO";
        case 13: return "DD";
        case 14: return "CODEC";
        case 15: return "SPI";
        case 16: return "PXI";
        case 17: return "FS";
        case 18: return "DI";
        case 19: return "HID";
        case 20: return "CAMERA";
        case 21: return "PI";
        case 22: return "PM";
        case 23: return "PMLOW";
        case 24: return "FSI";
        case 25: return "SRV";
        case 26: return "NDM";
        case 27: return "NWM";
        case 28: return "SOCKET";
        case 29: return "LDR";
        case 30: return "ACC";
        case 31: return "ROMFS";
        case 32: return "AM";
        case 33: return "HIO";
        case 34: return "UPDATER";
        case 35: return "MIC";
        case 36: return "FND";
        case 37: return "MP";
        case 38: return "MPWL";
        case 39: return "AC";
        case 40: return "HTTP";
        case 41: return "DSP";
        case 42: return "SND";
        case 43: return "DLP";
        case 44: return "HIOLOW";
        case 45: return "CSND";
        case 46: return "SSL";
        case 47: return "AMLOW";
        case 48: return "NEX";
        case 49: return "FRIENDS";
        case 50: return "RDT";
        case 51: return "APPLET";
        case 52: return "NIM";
        case 53: return "PTM";
        case 54: return "MIDI";
        case 55: return "MC";
        case 56: return "SWC";
        case 57: return "FATFS";
        case 58: return "NGC";
        case 59: return "CARD";
        case 60: return "CARDNOR";
        case 61: return "SDMC";
        case 62: return "BOSS";
        case 63: return "DBM";
        case 64: return "CFG";
        case 65: return "PS";
        case 66: return "CEC";
        case 67: return "IR";
        case 68: return "UDS";
        case 69: return "PL";
        case 70: return "CUP";
        case 71: return "GYROSCOPE";
        case 72: return "MCU";
        case 73: return "NS";
        case 74: return "NEWS";
        case 75: return "RO";
        case 76: return "GD";
        case 77: return "CARDSPI";
        case 78: return "EC";
        case 79: return "WEBBRS";
        case 80: return "TEST";
        case 81: return "ENC";
        case 82: return "PIA";
        case 254: return "APPLICATION";
        case 255: return "INVALID RESULT VALUE";
        default: return "INVALID MODULE";
    }
}

char* nnerr_description(u32 input)
{
    switch (input & 0x3FF)
    {
        case 0: return "SUCCESS";
        case 1000: return "INVALID SELECTION";
        case 1001: return "TOO LARGE";
        case 1002: return "NOT AUTHORIZED";
        case 1003: return "ALREADY DONE";
        case 1004: return "INVALID SIZE";
        case 1005: return "INVALID ENUM VALUE";
        case 1006: return "INVALID COMBINATION";
        case 1007: return "NO DATA";
        case 1008: return "BUSY";
        case 1009: return "MISALIGNED ADDRESS";
        case 1010: return "MISALIGNED SIZE";
        case 1011: return "OUT OF MEMORY";
        case 1012: return "NOT IMPLEMENTED";
        case 1013: return "INVALID ADDRESS";
        case 1014: return "INVALID POINTER";
        case 1015: return "INVALID HANDLE";
        case 1016: return "NOT INITIALIZED";
        case 1017: return "ALREADY INITIALIZED";
        case 1018: return "NOT FOUND";
        case 1019: return "CANCEL REQUESTED";
        case 1020: return "ALREADY EXISTS";
        case 1021: return "OUT OF RANGE";
        case 1022: return "TIMEOUT";
        case 1023: return "INVALID RESULT VALUE";
        default: return "IDK, LOL";
    }
}
