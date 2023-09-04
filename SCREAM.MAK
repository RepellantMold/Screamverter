PROJ	=SCREAM
DEBUG	=0
CC	=qcl
AS	=qcl
CFLAGS_G	= /AL /W1 /Za 
CFLAGS_D	= /Zi /Zr /Od 
CFLAGS_R	= /O /Ot /Gs /Os /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_R)
AFLAGS_G	= /Cx /W1 /P2 
AFLAGS_D	= /Zi /l /Sa /Sd 
AFLAGS_R	= /DNDEBUG 
AFLAGS	=$(AFLAGS_G) $(AFLAGS_R)
LFLAGS_G	= /CP:0xfff /NOE /SE:0x80 /ST:0x800 
LFLAGS_D	= /CO 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_R)
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
	qlink $(LFLAGS) @$(PROJ).crf

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)

