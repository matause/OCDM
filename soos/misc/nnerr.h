#pragma once

#include <3ds.h>
#include <stdio.h>

#define NNERR(ret)\
({\
    printf("[NN:%s] %s in module %s, because %s.\n", nnerr_level(R_LEVEL((ret))), nnerr_summary(R_SUMMARY((ret))), nnerr_module(R_MODULE((ret))), nnerr_description(R_DESCRIPTION((ret))));\
})

//marcus@Werkstaetiun:~/Asztal$ ./enumfucker.lua ./CTR_SDK/include/nn/Result.h | sed 's/\"\([^"]*\)\"/\"\U\1\"/g'
char* nnerr_level(u32 input);
char* nnerr_summary(u32 input);
char* nnerr_module(u32 input);
char* nnerr_description(u32 input);
