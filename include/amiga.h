#ifndef __AMIGA_H
#define __AMIGA_H
#include "ext.h"

int check_valid_mod(FILE* MODfile);
u8 convert_pattern_period_to_note(u16 period);
u16 convert_finetune_value_to_c_frequency(u8 finetune);

#endif
