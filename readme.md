# Screamverter

Scream Tracker 3 module to Scream Tracker 2 module converter written in ANSI C.

# License

[0MIT/MIT-0/MIT No Attribution](LICENSE), it can be relicensed under users discretion under any other license. (WTFPL, GPL, BSD, standard MIT, MPL, etc.)

# Compiling

It can be built with CMake, or compiled manually (which shouldn't be too difficult to do.)

# Requirements for a decent conversion

It'd probably be a lot to ask of one person to convert an S3M file to an STM...

- 4 channels only, no more, no less!
- Sample sizes cannot exceed 65535, and no 16-bit samples
- No panning, ScreamTracker 2 is mono!
  - Adlib is also not supported.
- You can only use the effects in the table if you want something _somewhat_ decent...

## Effects table

| Effect | Function (in ST2) | Notes (Effect memory **does not exist** in ScreamTracker 2!)                                                                                                                   |
| -----: | :---------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
|    Axx |  Set speed/tempo  | It has a scaling factor alongside setting speed                                                                                                                                |
|    Bxx |  Set next order   | It does not perform an immediate pattern break, so it's best to place it with a Pattern break if you want it to function like other trackers.                                  |
|    Cxx |   Pattern break   |                                                                                                                                                                                |
|    Dxy |   Volume slide    | There's no fine slides, and y will take priority is both x and y are specified.                                                                                                |
|    Exx |  Portamento down  | This effect can underflow, and there are no fine/extra-fine slides.                                                                                                            |
|    Fxx |   Portamento up   | This effect can overflow, ditto for fine/extra-fine slides.                                                                                                                    |
|    Gxx |  Tone portamento  |                                                                                                                                                                                |
|    Hxy |      Vibrato      | The depth is doubled compared to Scream Tracker 3+ and Amiga                                                                                                                   |
|    Ixy |      Tremor       |                                                                                                                                                                                |
|    Jxy |     Arpeggio      | This effect has a rather strange bug where it skips to whatever is specified in y halfway through a row if x was 0, but do not expect this bug to be emulated by every player. |
|    Kxx |      (No-op)      | This can be entered into the editor but it does not do anything.                                                                                                               |
|    Lxx |      (No-op)      | Ditto.                                                                                                                                                                         |
|    Mxx |      (No-op)      | Ditto again.                                                                                                                                                                   |
|    Nxx |      (No-op)      | Ditto yet again.                                                                                                                                                               |
|    Oxx |      (No-op)      | Do you get it now?..                                                                                                                                                           |