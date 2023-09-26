# STM TECHNICAL DOCUMENTATION

All of this information has been accumulated over reading some sources.
Made by RepellantMold, 2023.

## Main header

| Offset (decimal and hex) |           Size of field (bytes)           | Explanation of field (accumulated information from various documents)                                                                         |
| -----------------------: | :---------------------------------------: | --------------------------------------------------------------------------------------------------------------------------------------------- |
|                        0 |                    20                     | Song title (in ASCIIZ)                                                                                                                        |
|                  20/0x14 |                     8                     | Magic string, common value is "!Scream!" (this is not checked in either Scream Tracker 2 or 3 but other players tend to be picky about this!) |
|                  28/0x1C |                     1                     | 0x1A (checked in Scream Tracker 3 but not 2)                                                                                                  |
|                  29/0x1D |                     1                     | File type (song = 1, module = 2)                                                                                                              |
|                  30/0x1E |                     1                     | Major version                                                                                                                                 |
|                  31/0x1F |                     1                     | Minor version                                                                                                                                 |
|                  32/0x20 |                     1                     | Initial tempo                                                                                                                                 |
|                  33/0x21 |                     1                     | Number of patterns                                                                                                                            |
|                  34/0x22 |                     1                     | Global volume                                                                                                                                 |
|                  35/0x23 |                    13                     | Reserved                                                                                                                                      |
|                  48/0x30 |            Sample header \* 31            | Instrument information                                                                                                                        |
|               1024/0x410 |                    128                    | Order list (decimal 99 for blank entries)                                                                                                     |
|               1168/0x490 |       4 \* 64 \* Number of patterns       | Pattern data                                                                                                                                  |
|   after the pattern data | sample size, padded to 16 byte boundaries | Sample data                                                                                                                                   |

## Sample header

| Offset (decimal and hex) | Size of field (bytes) | Explanation of field (accumulated information from various documents) |
| -----------------------: | :-------------------: | --------------------------------------------------------------------- |
|                        0 |          12           | Sample name (in ASCIIZ)                                               |
|                  12/0x0C |           1           | 0x00                                                                  |
|                  13/0x0D |           1           | Insrtument disk                                                       |
|                  14/0x0E |           2           | Reserved (internal segment)                                           |
|                  16/0x10 |           2           | Sample length                                                         |
|                  18/0x12 |           2           | Loop start                                                            |
|                  20/0x14 |           2           | Loop end (65535/0xFFFF denotes no loop)                               |
|                  22/0x16 |           1           | Default volume                                                        |
|                  23/0x17 |           1           | Reserved                                                              |
|                  24/0x18 |           2           | C2 speed                                                              |
|                  26/0x1A |           4           | Reserved                                                              |
|                  30/0x1E |           2           | Internal segment/paragraph (12-bit pointer)                           |

## Pattern

The pattern format consists of the following...[2]

| Byte              | Bits/Meaning                                               |
|-------------------|------------------------------------------------------------|
| Note/Octave       | bits 0 - 3 = Note, bits 4 - 7 = Octave                     |
| Instrument/Volume | bits 0 - 2 = lower bits of Volume, bits 3 - 7 = Instrument |
| Effect/Volume     | bits 0 - 3 = Effect, bits 4 - 6 = high bits of Volume      |
| Parameter         | No bit-overlaps                                            |

[1]: https://raw.githubusercontent.com/libxmp/libxmp/master/docs/formats/stm.txt
[2]: https://ftp.modland.com/pub/documents/format_documentation/Scream%20Tracker%20v1.0%20(.stm).txt