!IF $(BUILD_ZLIB)
SUBDIRS = zlib libmatio test
!ELSE
SUBDIRS = libmatio test
!ENDIF

all:
	for %d in ($(SUBDIRS)) do nmake sub THIS=%d
clean:
	for %d in ($(SUBDIRS)) do nmake clean-sub THIS=%d
install:
	for %d in ($(SUBDIRS)) do nmake install-sub THIS=%d

sub:
	cd $(THIS)
	nmake
	cd ..

install-sub:
	cd $(THIS)
	nmake -e install

clean-sub:
	cd $(THIS)
	nmake -e clean
