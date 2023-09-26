# Scream Tracker 3 Module - TECHNICAL DOCUMENTATION

This is (what I hope to be) a penultimate document, references to any special information will be linked.
Made by RepellantMold, 2023.

## Important concepts

- Parapointers: A pointer in the file shifted right by 4 bits

## Main song header

| Offset (decimal and hex) |                 Size of field (bytes)                 | Explanation of field (accumulated information from various documents)                                                                                                                                                                                               |
| -----------------------: | :---------------------------------------------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|                        0 |                          28                           | Song name (in ASCIIZ)                                                                                                                                                                                                                                               |
|                  28/0x1C |                           1                           | 0x1A                                                                                                                                                                                                                                                                |
|                  29/0x1D |                           1                           | File type (always 16)                                                                                                                                                                                                                                               |
|                  30/0x1E |                           2                           | Reserved                                                                                                                                                                                                                                                            |
|                  32/0x20 |                           2                           | Number of orders (includes pattern markers)                                                                                                                                                                                                                         |
|                  34/0x22 |                           2                           | Number of instruments                                                                                                                                                                                                                                               |
|                  36/0x24 |                           2                           | Number of patterns                                                                                                                                                                                                                                                  |
|                  38/0x26 |                           2                           | Song Flags (meanings are in another table)                                                                                                                                                                                                                          |
|                  40/0x28 |                           2                           | Created with Tracker/Version [(meanings are in another table)](#created-with-tracker--version-values)                                                                                                                                                               |
|                  42/0x2A |                           2                           | [File format version][1] (apparently?), 1 = signed samples and 2 = unsigned samples                                                                                                                                                                                 |
|                  44/0x2C |                           4                           | "SCRM", used as signature                                                                                                                                                                                                                                           |
|                  48/0x30 |                           1                           | Global volume                                                                                                                                                                                                                                                       |
|                  49/0x31 |                           1                           | Initial speed/ticks per row                                                                                                                                                                                                                                         |
|                  50/0x32 |                           1                           | Initial tempo/ticks per second                                                                                                                                                                                                                                      |
|                  51/0x33 |                           1                           | Master volume - 7th bit is stereo flag, stereo if set and mono if it is not. This value otherwise only has meaning with a SoundBlaster card since [it's part of its mixing multiplication with that card.][1]                                                       |
|                  52/0x34 |                           1                           | GUS click removal, not really used in anything besides [Scream Tracker 3 where it uses the amount of specified channels divided by 2][1] while other trackers/players use volume ramping instead                                                                    |
|                  53/0x35 |                           1                           | If the value is 252/0xFC, then load panning values from the channel panning settings otherwise use default settings                                                                                                                                                 |
|                  54/0x36 |                           8                           | Mainly reserved, though some trackers write data here (e.g. Impulse Tracker writes the edit timer in this area)                                                                                                                                                     |
|                  62/0x3E |                           2                           | Special data pointer (never seen it used...)                                                                                                                                                                                                                        |
|                  64/0x40 |                          32                           | Channel settings (meanings are in another table)                                                                                                                                                                                                                    |
|                  96/0x60 |      Number of orders as specified in the header      | The first entry in this array is the index of the first pattern to play, the second entry is the second pattern, etc. If an entry is 254/0xFE then it is a "marker pattern" and is skipped during playback, and an entry of 255/0xFF signifies the end of the song. |
|                      -/- | Number of instruments as specified in the header \* 2 | Parapointers for the instruments                                                                                                                                                                                                                                    |
|                      -/- |  Number of patterns as specified in the header \* 2   | Parapointers for the patterns                                                                                                                                                                                                                                       |
|                      -/- |                          32                           | Channel panning settings - all bytes are only 4 bits long so AND it with 15 to filter out whatever is in the upper 4 bits, if master volume specifies mono then ignore anything here                                                                                |

### Song flag meanings

| Bit | Meaning                                                                                                       |
| --: | ------------------------------------------------------------------------------------------------------------- |
|   0 | ST2 vibrato (not supported in Scream Tracker 3.01+)                                                           |
|   1 | ST2 tempo (not supported in Scream Tracker 3.01+)                                                             |
|   2 | Amiga slides (not supported in Scream Tracker 3.01+)                                                          |
|   3 | 0-vol optimizations: Automatically turn off looping notes whose volume is 0 for more than 2 rows              |
|   4 | Amiga limits: Disallow any notes that go beyond the Amiga hardware limits, sliding up stops at B-5 etc.       |
|   5 | SoundBlaster: enable filter/FX (not supported in Scream Tracker 3.01+)                                        |
|   6 | Fast volume slides: volume slide are ALSO performed on the first tick when this is set and/or CWT/V is 0x1300 |
|   7 | Pointer to special data is valid (???)                                                                        |
|  8+ | Ignored/invalid                                                                                               |

### Created with tracker / version values

These are all the values that are known

Most follow the convention of being able to be easily identified using this formula; SHR 12 = tracker, AND 0x0FFF = version

| Value (hex) | Tracker                                                                                       |
| :---------: | --------------------------------------------------------------------------------------------- |
|    13xy     | Scream Tracker 3.xy [(many trackers try to disguise themselves as Scream Tracker)][4]         |
|    2xyy     | Imago Orpheus X.yy                                                                            |
|    3xyy     | Impulse Tracker X.yy                                                                          |
|    4xyy     | Schism Tracker up to 0.50 [(all later versions encode a time stamp in this field instead)][4] |
|    5xyy     | OpenMPT X.yy [(lower 2 bytes of the version are stored in a reserved area in 1.29.10.0+)][4]  |
|    6xyy     | BeRoTracker X.yy (versions from 2004 - 2012 used 0x4100 instead)                              |
|    7xyy     | CreamTracker X.yy                                                                             |
|    CA00     | Camoto / libgamemusic                                                                         |
|    0208     | [Akord/Polish localized Squeak Tracker][4]                                                    |
|    5447     | [Graoumf Tracker][4]                                                                          |

### Channel settings

All values are in decimal.

|         Value          |          Meaning           |
| :--------------------: | :------------------------: |
|           0            |           Left 1           |
|           1            |           Left 2           |
|           2            |           Left 3           |
|           3            |           Left 4           |
|           4            |           Left 5           |
|           5            |           Left 6           |
|           6            |           Left 7           |
|           7            |           Left 8           |
|           8            |          Right 1           |
|           9            |          Right 2           |
|           10           |          Right 3           |
|           11           |          Right 4           |
|           12           |          Right 5           |
|           13           |          Right 6           |
|           14           |          Right 7           |
|           15           |          Right 8           |
|           16           |       Adlib Melody 1       |
|           17           |       Adlib Melody 2       |
|           18           |       Adlib Melody 3       |
|           19           |       Adlib Melody 4       |
|           20           |       Adlib Melody 5       |
|           21           |       Adlib Melody 6       |
|           22           |       Adlib Melody 7       |
|           23           |       Adlib Melody 8       |
|           24           |       Adlib Melody 9       |
|           25           |   Adlib Percussion: Kick   |
|           26           |  Adlib Percussion: Snare   |
|           27           |   Adlib Percussion: Tom    |
|           28           |   Adlib Percussion: Top    |
|           29           |   Adlib Percussion: Hat    |
|         30-127         |      Invalid/Garbage       |
| all values above + 128 | Same meaning, but disabled |
|          255           |       Unused channel       |

## Sample headers

| Offset (decimal and hex) | Size of field (bytes) | Explanation                                                                                                                                                                                                          |
| -----------------------: | :-------------------: | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|                        0 |           1           | Type: 0 = message, 1 = sample, 2 = adlib, 3+ = adlib drums                                                                                                                                                           |
|                        1 |          12           | File name in 8.3 format                                                                                                                                                                                              |
|                  13/0x0D |           3           | Parapointer to sample data (byte 1 is the upper byte while bytes 2 and 3 are the lower word of the value)                                                                                                            |
|                  16/0x10 |           4           | Sample length (Scream Tracker 3 doesn't use the entire field since it's still limited to 64k samples)                                                                                                                |
|                  20/0x14 |           4           | Loop start (ditto)                                                                                                                                                                                                   |
|                  24/0x18 |           4           | Loop end (ditto again)                                                                                                                                                                                               |
|                  28/0x1C |           1           | Default sample volume                                                                                                                                                                                                |
|                  29/0x1D |           1           | Reserved                                                                                                                                                                                                             |
|                  30/0x1E |           1           | Packing scheme (usually 0 for unpacked, nobody's attempted to crack the "DP30ADPCM" packing method...)                                                                                                               |
|                  31/0x1F |           1           | Sample flags (bit 0 = loop, bit 1 = stereo, bit 2 = 16-bit)                                                                                                                                                          |
|                  32/0x20 |           4           | C2 speed, Scream Tracker 3 doesn't use the entire field (though some trackers do use the entire field)                                                                                                               |
|                  36/0x24 |           4           | Reserved                                                                                                                                                                                                             |
|                  38/0x26 |           2           | [Scream Tracker 3 internal value: Address of the sample in the Gravis UltraSound memory divided by 32][1], [though it has special meaning with a SoundBlaster and completely blank in Scream Tracker 3.00 files.][7] |
|                  40/0x28 |           2           | [Scream Tracker 3 internal value: SoundBlaster loop expansion flag (?)][1]                                                                                                                                           |
|                  42/0x2A |           4           | [Scream Tracker 3 internal value: "Last used position"][1]                                                                                                                                                           |
|                  48/0x30 |          28           | Sample name                                                                                                                                                                                                          |
|                  76/0x4C |           4           | "SCRS", used as signature                                                                                                                                                                                            |

## Patterns

The format for patterns go like this

- Packed size of the pattern, 2 bytes
- A single byte denoting what channel this byte belongs to, if it's 0 then it signifies the row is done.
  - There's more information that you can get depending on bits 5 - 7 and you read either 2 bytes (note/instrument/effect) or 1 byte (volume).

|   Bit | Meaning                                                                  |
| ----: | ------------------------------------------------------------------------ |
| 0 - 4 | Channel (0 - 31)                                                         |
|     5 | Note and instrument bytes are present (note is 255 if there's no effect) |
|     6 | volume byte is present (volume is 255 if nothing is specified)           |
|     7 | effect and parameter bytes are present                                   |

### Effects

All values are from 1 - 27, though it'll be presented as letters since it's how it's displayed in the interface
All parameters are in hexadecimal.

| Effect |                         Function                          | Notes                                                                                                                             |
| -----: | :-------------------------------------------------------: | --------------------------------------------------------------------------------------------------------------------------------- |
|    Axx |                 Set ticks per row (speed)                 | A parameter of 0 causes the effect to be ignored.                                                                                 |
|    Bxx |                       Position jump                       |                                                                                                                                   |
|    Cxx |                       Break pattern                       | The parameter is stored as BCD, so treat it as decimal. if the parameter is 64 or higher, the effect is ignored.                  |
|    Dxy |                       Volume slide                        | The effect slides on non-zero ticks, unless fast volume slides in the song flags is enabled in which case you slide on all ticks. |
|    Exy |                      Portamento down                      |                                                                                                                                   |
|    Fxy |                       Portamento up                       |                                                                                                                                   |
|    Gxx |                      Tone portamento                      |                                                                                                                                   |
|    Hxy |                          Vibrato                          | The memory is shared with fine vibrato.                                                                                           |
|    Ixy |                          Tremor                           |                                                                                                                                   |
|    Jxy |                         Arpeggio                          |                                                                                                                                   |
|    Kxy |                  Vibrato + volume slide                   | The effect is ignored when a fine volume slide is requested.                                                                      |
|    Lxy |              Tone portamento + volume slide               | The effect is ignored when a fine volume slide is requested.                                                                      |
|    Oxy |                     Set sample offset                     | The parameters get multiplied by 256, most implementations will output nothing if the parameters go past the loop point.          |
|    Qxy |  Note retrigger ([values for x](#note-retrigger-table))   |                                                                                                                                   |
|    Rxy |                          Tremolo                          |                                                                                                                                   |
|    Sxy | Misc. effects ([dependent on y parameter](#misc-effects)) | This effect uses effect memory, meaning it'll use the last specified parameter that was specified in the channel.                 |
|    Txx |                         Set tempo                         | If the parameter is less than 33 then the effect is ignored.                                                                      |
|    Uxy |                       Fine vibrato                        | The memory is shared with regular vibrato                                                                                         |
|    Vxx |                     Set global volume                     | The effect gets applied on the **second tick**, if the value is 0x40 or more, then it's ignored.                                  |

#### Misc. effects

| Sxx value |                    Function                    | Notes                                                                                                                                                              |
| --------: | :--------------------------------------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
|       S0x |                   Set filter                   | Disabled, no implementation details                                                                                                                                |
|       S1x |             Set glissando control              | Make tone portamento snap to the nearest semitone when enabled                                                                                                     |
|       S2x | Set [finetune](#finetune-table) according to x | An old effect from ProTracker, the manual says the effect is pointless.                                                                                            |
|       S3x |              Set vibrato waveform              | The low 2 bits specify the actual waveform, 3rd bit specifies if the waveform resets on a new note. Waveform options: 0. Sinewave 1. Ramp down 2. Square 3. Random |
|       S4x |              Set tremolo waveform              | Ditto but for tremolo.                                                                                                                                             |
|       S8x |              Set panning position              | This effect only works on GUS in Scream Tracker 3.                                                                                                                 |
|       SAx |               Old stereo control               | Allegedly removed in Scream Tracker 3.01+.                                                                                                                         |
|       SBx |                  Pattern loop                  | [The info is _global_, not per channel.][5] if 0 is specified, then it sets the loop row to the row you place the effect on.                                       |
|       SCx |              Note cut for x ticks              | If x is equal to the speed or 0 then it gets ignored.                                                                                                              |
|       SDx |             Note delay for x ticks             |                                                                                                                                                                    |
|       SEx |            Delay pattern for x rows            |                                                                                                                                                                    |
|       SFx |                  Funk repeat                   | Another effect nobody's implemented, this one is from ProTracker.                                                                                                  |

#### Finetune table

| Value (hex) | Frequency |
| ----------: | --------- |
|           0 | 7895 Hz   |
|           1 | 7941 Hz   |
|           2 | 7985 Hz   |
|           3 | 8046 Hz   |
|           4 | 8107 Hz   |
|           5 | 8169 Hz   |
|           6 | 8232 Hz   |
|           7 | 8280 Hz   |
|           8 | 8363 Hz   |
|           9 | 8413 Hz   |
|           A | 8463 Hz   |
|           B | 8529 Hz   |
|           C | 8581 Hz   |
|           D | 8651 Hz   |
|           E | 8723 Hz   |
|           F | 8757 Hz   |

#### Note retrigger table

| Value (hex) | Meaning                                          |
| ----------: | ------------------------------------------------ |
|           0 | 0                                                |
|           1 | -1                                               |
|           2 | -2                                               |
|           3 | -4                                               |
|           4 | -8                                               |
|           5 | -16                                              |
|           6 | \*2/3 [(it seems to use a table for this...)][5] |
|           7 | \*1/2                                            |
|           8 | 0                                                |
|           9 | +1                                               |
|           A | +2                                               |
|           B | +4                                               |
|           C | +8                                               |
|           D | +16                                              |
|           E | \*3/2                                            |
|           F | \*2                                              |

[1]: https://ftp.modland.com/pub/documents/format_documentation/Scream%20Tracker%20v3.20%20(.s3m).txt "Scream Tracker 3's TECH.DOC"
[2]: https://moddingwiki.shikadi.net/wiki/S3M_Format "DOS Game Modding Wiki"
[3]: https://ftp.modland.com/pub/documents/format_documentation/FireLight%20S3M%20Player%20Tutorial.txt "Firelight's S3M Player tutorial"
[4]: https://wiki.openmpt.org/Development:_Formats/S3M "OpenMPT's development page on the S3M format"
[5]: https://wiki.multimedia.cx/index.php?title=Scream_Tracker_3_Module "S3Ms entry on Multimedia Wiki"
[6]: https://ftp.modland.com/pub/documents/format_documentation/Scream%20Tracker%20v3.20%20effects%20(.s3m).txt "Effect reference, taken from the Scream Tracker 3 manual"
[7]: https://sagamusix.de/en/blog/2021/08/21/s3m-format-shenanigans/
