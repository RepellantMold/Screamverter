#ifndef __CRC32_H
#define __CRC32_H

u32 crc32_for_byte(u32 byte);
u32 crc32(const unsigned char* input, usize size);

#endif