all: lib

OBJ = SRC/sgetv0.o SRC/slaqrb.o SRC/sstqrb.o SRC/ssortc.o SRC/ssortr.o \
	SRC/sstatn.o SRC/sstats.o SRC/snaitr.o SRC/snapps.o SRC/snaup2.o \
	SRC/snaupd.o SRC/snconv.o SRC/sneigh.o SRC/sngets.o SRC/ssaitr.o \
	SRC/ssapps.o SRC/ssaup2.o SRC/ssaupd.o SRC/ssconv.o SRC/sseigt.o \
	SRC/ssgets.o SRC/sneupd.o SRC/sseupd.o SRC/ssesrt.o SRC/dgetv0.o \
	SRC/dlaqrb.o SRC/dstqrb.o SRC/dsortc.o SRC/dsortr.o SRC/dstatn.o \
	SRC/dstats.o SRC/dnaitr.o SRC/dnapps.o SRC/dnaup2.o SRC/dnaupd.o \
	SRC/dnconv.o SRC/dneigh.o SRC/dngets.o SRC/dsaitr.o SRC/dsapps.o \
	SRC/dsaup2.o SRC/dsaupd.o SRC/dsconv.o SRC/dseigt.o SRC/dsgets.o \
	SRC/dneupd.o SRC/dseupd.o SRC/dsesrt.o SRC/cnaitr.o SRC/cnapps.o \
	SRC/cnaup2.o SRC/cnaupd.o SRC/cneigh.o SRC/cneupd.o SRC/cngets.o \
	SRC/cgetv0.o SRC/csortc.o SRC/cstatn.o SRC/znaitr.o SRC/znapps.o \
	SRC/znaup2.o SRC/znaupd.o SRC/zneigh.o SRC/zneupd.o SRC/zngets.o \
	SRC/zgetv0.o SRC/zsortc.o SRC/zstatn.o UTIL/icnteq.o UTIL/icopy.o \
	UTIL/iset.o UTIL/iswap.o UTIL/ivout.o UTIL/second.o UTIL/svout.o \
	UTIL/smout.o UTIL/dvout.o UTIL/dmout.o UTIL/cvout.o UTIL/cmout.o \
	UTIL/zvout.o UTIL/zmout.o

lib: $(OBJ)
	$(AR) $(ARFLAGS) libarpack.a $(OBJ)
	ranlib libarpack.a

clean:
	$(RM) $(OBJ) libarpack.a
