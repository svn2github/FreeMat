top_builddir = ..
top_srcdir = ..\..
srcdir = ..\..\test

CFLAGS  = -I$(top_builddir) $(ZLIB_CFLAGS) $(CFLAGS)
FCFLAGS = -I$(top_builddir) $(FCFLAGS)
!IF $(BUILD_ZLIB)
ZLIB_LIBS = $(top_builddir)\zlib\zlib.lib
!ENDIF

.SUFFIXES : .c .obj .exe

.c.obj:
	$(CC) -o $@ $(CFLAGS) $<

test_mat_SOURCES = test_mat.c
test_mat_LDADD   = $(top_builddir)\libmatio\libmatio.lib $(ZLIB_LIBS)

test_mat.exe:
	for %f in ($(test_mat_SOURCES)) do $(CC) -c $(CFLAGS) $(srcdir)\%f
	$(LD) -out:$@ *.obj $(test_mat_LDADD)

install:
	if not exist $(EXEC_PREFIX)\bin mkdir $(EXEC_PREFIX)\bin
	copy *.exe $(EXEC_PREFIX)\bin

clean:
	del *.obj
	del *.exe
