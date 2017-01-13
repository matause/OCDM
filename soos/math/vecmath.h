#pragma once

#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"


typedef struct
{
    float x;
    float y;
} vec;

void vec_add(vec* src, vec in);
void vec_sub(vec* src, vec in);
void vec_neg(vec* src);
float vec_dist(vec v1, vec v2);
void vec_mulv(vec* src, vec other);
void vec_muln(vec* src, float other);
void vec_divv(vec* src, vec other);
void vec_divn(vec* src, float other);
float vec_len(vec v);
void vec_lerp(vec* src, vec dest, float alpha);
void vec_lerp8(vec* src, vec dest, u8 alpha);
void vec_dupe(vec* out, vec src);
void vec_norm(vec* v);
void vec_norm1(vec* v);
void vec_setlen(vec* v, float len);
void vec_setlen1(vec* v, float len);
int vec_eq(vec v1, vec v2, float precision);

