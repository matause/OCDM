#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "vecmath.h"

vec nullvec = (vec){0.0F, 0.0F};

void vec_add(vec* src, vec in)
{
    src->x += in.x;
    src->y += in.y;
}

void vec_sub(vec* src, vec in)
{
    src->x -= in.x;
    src->y -= in.y;
}

void vec_neg(vec* src)
{
    src->x *= -1;
    src->y *= -1;
}

float vec_dist(vec v1, vec v2)
{
    int x = fabsf(v2.x - v1.x);
    int y = fabsf(v2.y - v1.y);
    //return sqrtf(x*x + y*y);
    return sqrtf(fmaf(x, x, fmaf(y, y, 0)));
}

void vec_mulv(vec* src, vec other)
{
    src->x *= other.x;
    src->y *= other.y;
}

void vec_muln(vec* src, float other)
{
    src->x *= other;
    src->y *= other;
}

void vec_divv(vec* src, vec other)
{
    src->x /= other.x;
    src->y /= other.y;
}

void vec_divn(vec* src, float other)
{
    src->x /= other;
    src->y /= other;
}

float vec_len(vec v)
{
    return vec_dist(nullvec, v);
}

void vec_lerp(vec* src, vec dest, float alpha)
{
    src->x = lerpf(src->x, dest.x, alpha);
    src->y = lerpf(src->y, dest.y, alpha);
}

void vec_lerp8(vec* src, vec dest, u8 alpha)
{
    vec_lerp(src, dest, alpha / 255.0F);
}

void vec_dupe(vec* out, vec src)
{
    out->x = src.x;
    out->y = src.y;
}

void vec_norm(vec* v)
{
    float len = vec_len(*v);
    if(len == 0.0F) return;
    v->x = v->x / len;
    v->x = v->x / len;
}

void vec_norm1(vec* v)
{
    float len = max(fabsf(v->x), fabsf(v->y));
    if(len == 0.0F) return;
    v->x = v->x / len;
    v->y = v->y / len;
}

void vec_setlen(vec* v, float len)
{
    vec_norm(v);
    vec_muln(v, len);
}

void vec_setlen1(vec* v, float len)
{
    vec_norm1(v);
    vec_muln(v, len);
}

int vec_eq(vec v1, vec v2, float precision)
{
    return (fabsf(v1.x - v2.x) < precision) && (fabsf(v1.y - v2.y) < precision);
}
