#include <3ds.h>


//#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
//#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

float lerpf(float v0, float v1, float t);
u32 lerp32(u64 v0, u64 v1, u8 t);
u8 lerp8(u16 v0, u16 v1, u8 t);
s32 lerps32(s64 v0, s64 v1, u8 t);
u32 blend(u32 v0, u32 v1, u8 alpha);
