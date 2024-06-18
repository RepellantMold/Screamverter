#ifndef __MOD_H__
#define __MOD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ext.h"
#include "envcheck.h"

typedef struct {
  char name[22];
  u16 length;
  u8 fine_tune;
  u8 default_volume;
  u16 loop_start;
  u16 loop_length;
} mod_sample_header_t;

typedef struct {
  char title[20];
  mod_sample_header_t samples[31];
  u8 total_orders;
  u8 restart_position;
  u8 orders[128];
} mod_song_header_t;

typedef struct {
  u16 period;
  u8 instrument;
  u8 effect;
  u8 effect_parameter;
} mod_pattern_t;

/* taken from cs127's ntcheck */
#define MOD_MAXCHN       4
#define MOD_ROWNUM       64
#define MOD_MAXPAT       63
#define MOD_ORDNUM_MAX   128
#define MOD_MAXSMP       31
#define MOD_EVENTSIZE    4
#define MOD_PATSIZE      ((MOD_EVENTSIZE) * (MOD_MAXCHN) * (MOD_ROWNUM))
#define MOD_SMPSIZE      30

#define MOD_ORDNUM_PTR   0x03B6
#define MOD_ORDERS_PTR   0x03B8
#define MOD_MAGIC_PTR    0x0438
#define MOD_PATTERNS_PTR 0x043C
#define MOD_SAMPLES_PTR  0x0014
#define MOD_SMPTUNE_RPTR 0x0018

extern mod_song_header_t mod_song_header;

extern u32 mod_pcm_pointers[MOD_MAXSMP];
extern u16 mod_pcm_lens[MOD_MAXSMP];
extern u8 mod_order_array[MOD_ORDNUM_MAX];

#endif
