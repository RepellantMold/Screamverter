# Screamverter

Scream Tracker 3 module to Scream Tracker 2 module converter written in ANSI C.

> [!NOTE]
> This project is currently halted, but I'm open to anyone completing it via a fork.
> In its current state, it can only convert the main song/sample headers.
> This branch (v2) is what I had stored on the repository on Codeberg last month.
> All the others are from this repository.

# License

[0MIT/MIT-0/MIT No Attribution](LICENSE), it can be relicensed under users discretion under any other license. (WTFPL, GPL, BSD, standard MIT, MPL, etc.)
There's also no warranty, so I can't be held responsible for any damage from use of the program.

# Compiling

It can be built with CMake, or compiled manually (which shouldn't be too difficult to do.)
I mainly tested this under a virtual DOS environment, with Microsoft Quick C.

# How to use

> [!IMPORTANT]
> These are required to have a decent conversion
> 
> - 4 channels only, no more, no less!
> - Sample sizes cannot exceed 65535, and no 16-bit samples
> - No panning, mono.
>   - Adlib is also not supported.
> - Only use the effects listed in the effects table listed below
> 
> | Effect | Function (in ST2) | Notes (Effect memory **does not exist** in Scream Tracker 2!)                                                                                                                  |
> | -----: | :---------------: | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
> |    Axx |  Set speed/tempo  | It has a scaling factor alongside setting speed                                                                                                                                |
> |    Bxx |  Set next order   | It does not perform an immediate pattern break, so it's best to place it with a Pattern break if you want it to function like other trackers.                                  |
> |    Cxx |   Pattern break   | You cannot choose which row to jump to, Scream Tracker 2 ignores whatever value is stored.                                                                                     |
> |    Dxy |   Volume slide    | There's no fine slides, and y will take priority is both x and y are specified.                                                                                                |
> |    Exx |  Portamento down  | This effect can underflow, and there are no fine/extra-fine slides.                                                                                                            |
> |    Fxx |   Portamento up   | This effect can overflow, ditto for fine/extra-fine slides.                                                                                                                    |
> |    Gxx |  Tone portamento  |                                                                                                                                                                                |
> |    Hxy |      Vibrato      | The depth is doubled compared to other trackers.                                                                                                                               |
> |    Ixy |      Tremor       |                                                                                                                                                                                |
> |    Jxy |     Arpeggio      | This effect has a rather strange bug where it skips to whatever is specified in y halfway through a row if x was 0, but do not expect this bug to be emulated by every player! |
> |    Kxx |      (No-op)      | This can be entered into the editor but it does not do anything.                                                                                                               |
> |    Lxx |      (No-op)      | Ditto.                                                                                                                                                                         |
> |    Mxx |      (No-op)      | Ditto again.                                                                                                                                                                   |
> |    Nxx |      (No-op)      | Ditto yet again.                                                                                                                                                               |
> |    Oxx |      (No-op)      | Do you get it now?..                                                                                                                                                           |

# Acknowledgements

I left in my own attempts at documenting the formats (as unfinished and crappily organized they are...)

## The sources used for my S3M documentation

[Scream Tracker 3's TECH.DOC](https://ftp.modland.com/pub/documents/format_documentation/Scream%20Tracker%20v3.20%20(.s3m).txt )
[DOS Game Modding Wiki](https://moddingwiki.shikadi.net/wiki/S3M_Format)
[Firelight's S3M Player tutorial](https://ftp.modland.com/pub/documents/format_documentation/FireLight%20S3M%20Player%20Tutorial.txt)
[OpenMPT Wiki](https://wiki.openmpt.org/Development:_Formats/S3M)
[Multimedia Wiki](https://wiki.multimedia.cx/index.php?title=Scream_Tracker_3_Module)
[Effect reference, taken from the Scream Tracker 3 manual](https://ftp.modland.com/pub/documents/format_documentation/Scream%20Tracker%20v3.20%20effects%20(.s3m).txt)
[Saga Musix's blog post](https://sagamusix.de/en/blog/2021/08/21/s3m-format-shenanigans/)

## The sources used for my STM documentation

[Scream Tracker 2's TECH.DOC](https://raw.githubusercontent.com/libxmp/libxmp/master/docs/formats/stm.txt)
[File Format List 2.0 by Max Maischein](https://ftp.modland.com/pub/documents/format_documentation/Scream%20Tracker%20v1.0%20(.stm).txt)