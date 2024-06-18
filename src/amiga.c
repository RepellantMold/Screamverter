#include "log.h"
#include "main.h"

#include "fmt/mod.h"

#define MOD_MAGIC_COUNT 17
#define MOD_MAGIC_SIZE  4
static const char* MOD_MAGIC[MOD_MAGIC_COUNT] = {
    /* taken from OpenMPT's source code (soundlib/Load_mod.cpp) */

    "M.K.", "PATT", "NSMS", "LARD", "M&K!", "FEST", "N.T.", "M\0\0\0",
    "FA04", "FLT4", "EXO4", "TDZ4", ".M.K", "4CHN", "04CH", "04CN"};

/* clang-format off */

static const u16 period_table[] = {
  1712,1616,1525,1440,1357,1281,1209,1141,1077,1017,961, 907,
  856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,
  428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,
  214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
  107, 101,  95,  90,  85,  80,  76,  71,  67,  64,  60,  57
};

/* clang-format on */

/* code borrowed (with modifications) from ntcheck by cs127 (with permission) */
int check_valid_mod(FILE* MODfile) {
  int fseekres = fseek(MODfile, MOD_MAGIC_PTR, SEEK_SET);
  size_t freadres;
  char magic[MOD_MAGIC_SIZE];
  register size_t i = 0;

  if (fseekres) {
    print_error("could not find the magic bytes (%s).\n", strerror(errno));
    return FOC_NOT_MOD_FILE;
  }

  freadres = fread(magic, 1, 4, MODfile);
  if (freadres < 4) {
    print_error(feof(MODfile) ? "could not read the magic bytes (file ends prematurely).\n"
                              : "could not read the magic bytes.\n");
    return FOC_NOT_MOD_FILE;
  }

  for (; i < MOD_MAGIC_COUNT; ++i)
    if (!memcmp(magic, MOD_MAGIC[i], MOD_MAGIC_SIZE))
      return FOC_SUCCESS;

  print_error("this file is not a valid 4-channel Amiga module file.\n");
  return FOC_NOT_MOD_FILE;
}

u8 convert_pattern_period_to_note(u16 period) {
  register u8 count = 42;

  for (; count < 131; ++count)
    if (period >= period_table[count])
      return count - 24;

  return 0xFF;
}

static u16 c_speed[16] = {8363, 8413, 8463, 8529, 8581, 8651, 8723, 8757,
                          7895, 7941, 7985, 8046, 8107, 8169, 8232, 8280};

u16 convert_finetune_value_to_c_frequency(u8 finetune) { return c_speed[finetune & 15]; }
