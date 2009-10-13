# automake file for libvolpack examples, process with automake
# Andy Loening 2001.09.29

LDADD = -lvolpack -lm
INCLUDES = -I../src
AM_LDFLAGS = -L../src

noinst_PROGRAMS = \
	makevolume \
	makeoctree \
	classifyvolume \
	rendervolume \
	scalevolume

scalevolume_SOURCES = \
	scalevolume.c \
	denfile.c 


MOSTLYCLEANFILES = \
	brainsmall.rv \
	brainsmall.oct \
	brainsmall.cv \
	brainsmall*ppm

DISTCLEANFILES = *~
