#ifndef __SAMPLE_HEADER_H
#define __SAMPLE_HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ext.h"

void show_s3m_inst_header(void);
void show_mod_inst_header(mod_sample_header_t header);
u32 grab_s3m_pcm_pointer(void);
u16 grab_s3m_pcm_len(void);
void grab_mod_instrument_header_data(FILE* file);
void convert_mod_instrument_header_modtostm(stm_instrument_header_t* stm_sample_header, mod_sample_header_t mod_inst_header);
u16 grab_mod_length_value(FILE* file);
void generate_blank_stm_instrument(stm_instrument_header_t* stm_sample_header);
void grab_s3m_instrument_header_data(FILE* file, usize position);
void convert_s3m_instrument_header_s3mtostm(stm_instrument_header_t* stm_sample_header);
void write_stm_instrument_header(FILE* STMfile, stm_instrument_header_t* stm_instrument_header);
void write_stx_instrument_header(FILE* STXfile);
void handle_sample_name_s3m2stm(stm_instrument_header_t* stm_sample_header);

#endif
