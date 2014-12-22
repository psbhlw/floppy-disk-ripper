
# compiler and linker
CC=g++
CFLAGS=-c -DDEBUG -Wall -O2
LDFLAGS=-static -Xlinker --strip-all
RM=rm -f


BIN=fdrc.exe
OBJS=fdrc.o fmt_adf.o fmt_auto.o fmt_base.o fmt_fdr.o fmt_img.o fmt_mfm.o fmt_raw.o fmt_udi.o mfm.o raw_track.o stats.o trk_analyser.o


all: $(BIN)
	@echo OK.

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(BIN)


fdrc.o: fdrc.cpp
	$(CC) $(CFLAGS) fdrc.cpp

fmt_adf.o: fmt_adf.cpp
	$(CC) $(CFLAGS) fmt_adf.cpp

fmt_fdr.o: fmt_fdr.cpp
	$(CC) $(CFLAGS) fmt_fdr.cpp

fmt_img.o: fmt_img.cpp
	$(CC) $(CFLAGS) fmt_img.cpp

fmt_mfm.o: fmt_mfm.cpp
	$(CC) $(CFLAGS) fmt_mfm.cpp

fmt_raw.o: fmt_raw.cpp
	$(CC) $(CFLAGS) fmt_raw.cpp

fmt_udi.o: fmt_udi.cpp
	$(CC) $(CFLAGS) fmt_udi.cpp

fmt_auto.o: fmt_auto.cpp
	$(CC) $(CFLAGS) fmt_auto.cpp

fmt_base.o: fmt_base.cpp
	$(CC) $(CFLAGS) fmt_base.cpp


mfm.o: mfm.cpp
	$(CC) $(CFLAGS) mfm.cpp


raw_track.o: raw_track.cpp
	$(CC) $(CFLAGS) raw_track.cpp

stats.o: stats.cpp
	$(CC) $(CFLAGS) stats.cpp

trk_analyser.o: trk_analyser.cpp
	$(CC) $(CFLAGS) trk_analyser.cpp


clean:
	$(RM) $(OBJS) $(BIN)

