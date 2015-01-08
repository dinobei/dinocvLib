OMPILER_PREFIX=
CROSS=
CC=$(CROSS)gcc
ELF2FLT=$(CROSS)elf2flt
STRIP=$(CROSS)strip

CFLAGS += -O2 -Dlinux -D__linux__ -Dunix -D__unix__ -DEMBED -std=c++11
#LDFLAGS += -r -Xlinker
#LOCAL_CFLAGS += -std=c++11
TRG = main
ALLOBJS = $(TRG).o\
../../source/dinocore/bmpanal.o\
../../source/dinocore/imageio.o\
../../source/dinocore/imgconv.o\
../../source/dinocore/imgcore.o\
../../source/dinocore/imgdraw.o\
../../source/dinocore/ppmanal.o\
../../source/dinocore/yuv422anal.o\
../../source/dinods/ds_sort.o\
../../source/dinods/linkedlist.o\
../../source/dinods/queue.o\
../../source/dinomm/memmgr.o\
../../source/dinoproc/binarization.o\
../../source/dinoproc/cannyedge.o\
../../source/dinoproc/chaincode.o\
../../source/dinoproc/convexhull.o\
../../source/dinoproc/dp.o\
../../source/dinoproc/feature.o\
../../source/dinoproc/filter.o\
../../source/dinoproc/histogram.o\
../../source/dinoproc/labeling.o\
../../source/dinoproc/morphology.o\
../../source/dinoproc/resize.o\
../../source/dinoproc/thinning.o\
Classifier.o\
dinotime.o\
list.o\
well512.o
ALLSRCS = $(ALLOBJS:.o=.cpp)

LIBRARY_PATH = /usr/local/lib

all:$(TRG) 
$(TRG) : $(ALLOBJS)
	$(CC) $(LDFLAGS) $(ALLOBJS) -o $@ -L$(LIBRARY_PATH) -ldinocv -lm

%o : %c
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	rm -f *.o *.elf *.bflt $(TRG)

