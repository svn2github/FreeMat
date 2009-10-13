INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY( zlib
adler32.c
compress.c
crc32.c
gzio.c
uncompr.c
deflate.c
trees.c
zutil.c
inflate.c
infback.c
inftrees.c
inffast.c)
