if NEED_BLAS
  BLAS_DIR = libMiniBLAS
endif

if NEED_PNG
  PNG_DIR = lpng125 libz
endif

if NEED_JPEG
  JPEG_DIR = libjpeg6b
endif

if NEED_TIFF
  TIFF_DIR = libtiff361
endif

SUBDIRS = libFreeMat libFFTPack libCore libGraphics libXP \
	 $(BLAS_DIR) libMiniLAPACK libffi libf2c $(JPEG_DIR) \
	$(TIFF_DIR) $(PNG_DIR)
AUTOMAKE_OPTIONS = foreign

