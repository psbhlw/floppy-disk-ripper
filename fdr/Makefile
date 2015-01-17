
# tools
MKDIR=mkdir
RMDIR=rm -rf
RM=rm -f

# your favorite asm
ASM=
AFLAGS=
SJASM=sjasmplus.exe
SPGBLD=spgbld.exe

# your favorite C
CC=
CFLAGS=

# your favorite linker
LD=
LDFLAGS=


LIST_DIR=list
OBJFF=FatFs/ff.o FatFs/diskio.o FatFs/diskio_asm.o FatFs/ff_util.o
OBJS=sys/Cstartup.o $(OBJFF)   main.o sdc.o fdd.o   keyb.o ui.o textmod.o fdd_asm.o
RES=fonts/robat.fnt


all: fdr.trd fdr.spg
	@echo OK.

$(LIST_DIR):
	$(MKDIR) $(LIST_DIR)
	$(MKDIR) $(LIST_DIR)/FatFs


sys/Cstartup.o: sys/Cstartup.a80
	$(ASM) $(AFLAGS) sys/Cstartup.a80 -o sys/Cstartup.o


FatFs/diskio_asm.o: FatFs/diskio_asm.a80
	$(ASM) $(AFLAGS) FatFs/diskio_asm.a80 -o FatFs/diskio_asm.o

FatFs/ff.o: FatFs/ff.c
	$(CC) $(CFLAGS) FatFs/ff.c -o FatFs/ff.o

FatFs/diskio.o: FatFs/diskio.c
	$(CC) $(CFLAGS) FatFs/diskio.c -o FatFs/diskio.o

FatFs/ff_util.o: FatFs/ff_util.c
	$(CC) $(CFLAGS) FatFs/ff_util.c -o FatFs/ff_util.o


keyb.o: keyb.a80
	$(ASM) $(AFLAGS) keyb.a80 -o keyb.o

textmod.o: textmod.a80
	$(ASM) $(AFLAGS) textmod.a80 -o textmod.o

fdd_asm.o: fdd_asm.a80
	$(ASM) $(AFLAGS) fdd_asm.a80 -o fdd_asm.o

sdc.o: sdc.c
	$(CC) $(CFLAGS) sdc.c -o sdc.o

fdd.o: fdd.c
	$(CC) $(CFLAGS) fdd.c -o fdd.o

ui.o: ui.c
	$(CC) $(CFLAGS) ui.c -o ui.o

main.o: main.c
	$(CC) $(CFLAGS) main.c -o main.o



main.bin: $(LIST_DIR) $(OBJS)
	$(LD) $(LDFLAGS)  -o main.bin   $(OBJS) $(RES)

fdr.trd: _make_trd.a80 main.bin
	$(SJASM) _make_trd.a80  --nologo

fdr.spg: _make_spg.a80 _make_spg.ini _make_spg.sys main.bin
	$(SJASM) _make_spg.a80  --nologo
	$(SPGBLD) -b _make_spg.ini fdr.spg -c 0


clean:
	$(RM) $(OBJS) main.bin fdr.trd _make_spg.bin fdr.spg
	$(RMDIR) $(LIST_DIR)

