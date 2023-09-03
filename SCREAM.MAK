PROJ	=SCREAM
DEBUG	=1
CC	=qcl
AS	=qcl
CFLAGS_G	= /AT /W1 /Za 
CFLAGS_D	= /Zi /Zr /Od 
CFLAGS_R	= /O /Ol /Gs /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_D)
AFLAGS_G	= /Cx /W1 /P2 
AFLAGS_D	= /Zi /l /Sa /Sd 
AFLAGS_R	= /DNDEBUG 
AFLAGS	=$(AFLAGS_G) $(AFLAGS_D)
LFLAGS_G	= /CP:0xfff /NOE /SE:0x80 /ST:0x800 
LFLAGS_D	= /CO 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_D)
RUNFLAGS	=test.s3m test.stm
OBJS_EXT = 	
LIBS_EXT = 	

.asm.obj: ; $(AS) $(AFLAGS) -c $*.asm

all:	$(PROJ).EXE

main.obj:	main.c $(H)

$(PROJ).EXE:	main.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
main.obj +
$(OBJS_EXT)
$(PROJ).EXE

$(LIBS_EXT);
<<
	ilink -a -e "qlink $(LFLAGS) @$(PROJ).crf" $(PROJ)

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)

