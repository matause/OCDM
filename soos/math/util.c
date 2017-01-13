#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <3ds.h>

#include "util.h"

float lerpf(float v0, float v1, float t)
{
    return fmaf(t, v1, fmaf(-t, v0, v0));
}

u32 lerp32(u64 v0, u64 v1, u8 t)
{
    return ((((v1 & 0xFFFFFFFF) * t) >> 8) + (((v0 & 0xFFFFFFFF) * (0xFF - t)) >> 8)) + 1;
}

u8 lerp8(u16 v0, u16 v1, u8 t)
{
    return ((((v1 & 0xFF) * t) >> 8) + (((v0 & 0xFF) * (0xFF - t)) >> 8)) + 1;
}

s32 lerps32(s64 v0, s64 v1, u8 t)
{
    return ((((v1 & 0xFFFFFFFF) * t) >> 8) + (((v0 & 0xFFFFFFFF) * (0xFF - t)) >> 8)) + 1;
}

u32 blend(u32 v0, u32 v1, u8 alpha)
{
    return (lerp8(v0 >> 24, v1 >> 24, alpha) << 24) | (lerp8(v0 >> 16, v1 >> 16, alpha) << 16) | (lerp8(v0 >> 8, v1 >> 8, alpha) << 8) | 0xFF;
}
