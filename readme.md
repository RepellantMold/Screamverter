# Screamverter

Scream Tracker 3 module to Scream Tracker 2.21+ module converter written in ANSI C.

> [!NOTE]
> This project is currently halted (due to me being frustrated with the pattern formats), there's probably all kinds of dumb mistakes (e.g. off-by-one) in this code too.
> 
> In its current state, it can only convert the main song/sample headers.
> I'm open to anyone completing it via a fork (credits for the original work are appreciated but not requried!)
> 
> This branch (v2) is what I had stored on a repository on Codeberg last month.
> 
> All the others are from this Github repository.

# License

[0MIT/MIT-0/MIT No Attribution](LICENSE), it can be relicensed under users discretion under any other license. (WTFPL, GPL, BSD, standard MIT, MPL, etc.)
There's also no warranty, so I can't be held responsible for any damage from use of the program.

# Compiling

It can be built with CMake, or compiled manually (which shouldn't be too difficult to do, e.g. with clang: `clang -std=c89 main.c`)

I mainly tested this under a virtual DOS environment, with Microsoft Quick C. (I used `qcl /AT /G0 /Gs main.c`)

# How to use

Call the executable with an input and output file inside your terminal. (`[executable] input.s3m output.s3m`)

> [!IMPORTANT]
> These are required to have a decent conversion
> 
> # General
> 
> - 4 channels only, no more, no less
> - No panning, mono
> - Adlib is not supported
> - No more than 63 patterns and 128 orders
> 
> # Samples
> 
> - Sample cannot exceed a length of 65535
> - Any sample with a volume of 0 will "clear out" the sample when saved in Scream Tracker 2
> - no 16-bit samples
> 
> # Patterns
> 
> - Only use the effects A to J (caveats shown in the note below)

> [!NOTE]
> Not all effects are compatible, here's a table showing all the effects that are available
> 
> | Effect | Function (in ST2) | Notes (Effect memory **does not exist** in Scream Tracker 2, it'll get treated as a no-op!)                                                                                    |
> | -----: | :---------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
> |    Axy |  Set speed/tempo  | `y` is a scaling factor alongside setting speed that's currently not accounted for, so it's best to just leave the tempo at 125. `x` is basically ticks per row.               |
> |    Bxx |  Set next order   | It does not perform an immediate pattern break, so it's best to place it with a Pattern break if you want it to function like other trackers.                                  |
> |    Cxx |   Pattern break   | You cannot choose which row to jump to, Scream Tracker 2 ignores whatever value is stored.                                                                                     |
> |    Dxy |   Volume slide    | There's no fine slides, `y` will take priority if both `x` and `y` are specified which is backwards from how it's usually handled.                                             |
> |    Exx |  Portamento down  | This effect can underflow, there are no fine/extra-fine slides.                                                                                                                |
> |    Fxx |   Portamento up   | This effect can overflow, ditto for fine/extra-fine slides.                                                                                                                    |
> |    Gxx |  Tone portamento  | It does not reset the volume to the sample's default sample volume unlike most other trackers.                                                                                 |
> |    Hxy |      Vibrato      | The depth is doubled compared to other trackers.                                                                                                                               |
> |    Ixy |      Tremor       | a value of 0 is very fast, like Tremor in early Scream Tracker 3.                                                                                                              |
> |    Jxy |     Arpeggio      | This effect has a rather strange bug where it skips to whatever is specified in y halfway through a row if x was 0, but do not expect this bug to be emulated by every player! |
> |    Kxx |      (No-op)      | This can be entered into the editor but it does not do anything. It also bugs out Scream Tracker 3 when importing!                                                             |
> |    Lxx |      (No-op)      | Ditto.                                                                                                                                                                         |
> |    Mxx |      (No-op)      | Ditto again.                                                                                                                                                                   |
> |    Nxx |      (No-op)      | Ditto yet again.                                                                                                                                                               |
> |    Oxx |      (No-op)      | Do you get it now?..                                                                                                                                                           |

# Acknowledgements

I left in my own attempts at documenting the formats (as unfinished and crappily organized they are...)

## The sources used for my S3M documentation

1. [Scream Tracker 3's TECH.DOC](https://ftp.modland.com/pub/documents/format_documentation/Scream%20Tracker%20v3.20%20(.s3m).txt )
3. [DOS Game Modding Wiki](https://moddingwiki.shikadi.net/wiki/S3M_Format)
3. [Firelight's S3M Player tutorial](https://ftp.modland.com/pub/documents/format_documentation/FireLight%20S3M%20Player%20Tutorial.txt)
7. [OpenMPT Wiki](https://wiki.openmpt.org/Development:_Formats/S3M)
4. [Multimedia Wiki](https://wiki.multimedia.cx/index.php?title=Scream_Tracker_3_Module)
2. [Effect reference, taken from the Scream Tracker 3 manual](https://ftp.modland.com/pub/documents/format_documentation/Scream%20Tracker%20v3.20%20effects%20(.s3m).txt)
0. [Saga Musix's blog post](https://sagamusix.de/en/blog/2021/08/21/s3m-format-shenanigans/)

## The sources used for my STM documentation

6. [Scream Tracker 2's TECH.DOC](https://raw.githubusercontent.com/libxmp/libxmp/master/docs/formats/stm.txt)
9. [File Format List 2.0 by Max Maischein](https://ftp.modland.com/pub/documents/format_documentation/Scream%20Tracker%20v1.0%20(.stm).txt)
