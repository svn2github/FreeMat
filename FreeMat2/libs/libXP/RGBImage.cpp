#include "RGBImage.hpp"

#include <png.h>
extern "C" {
#include <jpeglib.h>
}
extern "C" {
#include <tiffio.h>
}

#include <stdio.h>
#include "Exception.hpp"

RGBImage::RGBImage(int awidth, int aheight, byte *adata) {
  width = awidth;
  height = aheight;
  data = adata;
}

RGBImage::RGBImage(const RGBImage &copy) {
  width = copy.width;
  height = copy.height;
  data = copy.data;
}

RGBImage::RGBImage() {
  width = height = 0;
  data = NULL;
}

RGBImage::~RGBImage() {
}

byte* RGBImage::GetPixelData() {
  return data;
}

int RGBImage::GetWidth() {
  return width;
}

int RGBImage::GetHeight() {
  return height;
}

void RGBImage::WritePPM(std::string filename) {
  FILE *fp;
  fp = fopen(filename.c_str(),"wb");
  fprintf(fp,"P6\n");
  fprintf(fp,"%d %d\n",width,height);
  fprintf(fp,"255\n");
  fwrite(data,sizeof(char),width*height*3,fp);
  fclose(fp);
}

void RGBImage::SetPixelDirect(int x, int y, byte valred, byte valgreen, byte valblue) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
    data[3*(y*width+x)] = valred;
    data[3*(y*width+x)+1] = valgreen;
    data[3*(y*width+x)+2] = valblue;
  }
}

void RGBImage::GetPixelDirect(int x, int y, byte &valred, byte &valgreen, byte &valblue) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
    valred = data[3*(y*width+x)];
    valgreen = data[3*(y*width+x)+1];
    valblue = data[3*(y*width+x)+2];
  }
}

void RGBImage::SetPixel(int x, int y, Color col) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
    data[3*(y*width+x)] = col.red;
    data[3*(y*width+x)+1] = col.green;
    data[3*(y*width+x)+2] = col.blue;
  }
}

Color RGBImage::GetPixel(int x, int y) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height)) 
    return Color(data[3*(y*width+x)],data[3*(y*width+x)+1],data[3*(y*width+x)+2]);
  else
    return Color(0,0,0);
}

void RGBImage::SetAllPixels(Color col) {
  int n;
  int i;

  n = width*height;
  for (i=0;i<n;i++) {
    data[3*i] = col.red;
    data[3*i+1] = col.green;
    data[3*i+2] = col.blue;
  }
}

void RGBImage::WritePNG(std::string filename) {
#ifndef HAVE_PNG
  throw FreeMat::Exception("PNG support not available.");
#else
  png_structp png_ptr;
  png_infop info_ptr;
  int number_of_passes;
  FILE *fp = fopen(filename.c_str(), "wb");
  
  if (!fp) 
    throw FreeMat::Exception(std::string("Unable to open file ") + filename + " for writing");
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  if (setjmp(png_jmpbuf(png_ptr)))
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  png_init_io(png_ptr, fp);
  /* write header */
  if (setjmp(png_jmpbuf(png_ptr)))
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  png_set_IHDR(png_ptr, info_ptr, width, height,
	       8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);
  if (setjmp(png_jmpbuf(png_ptr)))
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  png_bytep* row_pointers = (png_bytep*) malloc(sizeof(png_bytep)*height);
  for (int y=0;y<height;y++)
    row_pointers[y] = (png_byte*) (data+3*y*width);
  png_write_image(png_ptr, row_pointers);
  free(row_pointers);
  if (setjmp(png_jmpbuf(png_ptr)))
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  png_write_end(png_ptr, NULL);
#endif
}

void RGBImage::WriteTIFF(std::string filename) {
#ifndef HAVE_TIFFLIB
  throw FreeMat::Exception("TIFF support not available.");
#else
  TIFF *output;
  // Open the output image
  if((output = TIFFOpen(filename.c_str(), "w")) == NULL)
    throw FreeMat::Exception(std::string("Cound not open file") + filename + "for writing");
  // Write the tiff tags to the file
  TIFFSetField(output, TIFFTAG_IMAGEWIDTH, width);
  TIFFSetField(output, TIFFTAG_IMAGELENGTH, height);
  TIFFSetField(output, TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS);
  TIFFSetField(output, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(output, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(output, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, 3);
  // Actually write the image
  if(TIFFWriteEncodedStrip(output, 0, data, width * height * 3) == 0)
    throw FreeMat::Exception(std::string("Could not write image ") + filename);
  TIFFClose(output);
#endif
}

void RGBImage::WriteJPEG(std::string filename) {
#ifndef HAVE_JPEGLIB
  throw FreeMat::Exception("JPEG support not available.");
#else
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE * outfile;               /* target file */
  JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
  int row_stride;               /* physical row width in image buffer */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  if ((outfile = fopen(filename.c_str(), "wb")) == NULL) 
    throw FreeMat::Exception(std::string("Can not open file ") + filename);
  jpeg_stdio_dest(&cinfo, outfile);
  cinfo.image_width = width;      /* image width and height, in pixels */
  cinfo.image_height = height;
  cinfo.input_components = 3;           /* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 90, TRUE /* limit to baseline-JPEG values */);
  jpeg_start_compress(&cinfo, TRUE);
  row_stride = width * 3; /* JSAMPLEs per row in image_buffer */
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = data + cinfo.next_scanline * row_stride;
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);
#endif
}

void WritePNGFile(std::string filename, byte *data, int width, int height) {
  png_structp png_ptr;
  png_infop info_ptr;
  int number_of_passes;
  FILE *fp = fopen(filename.c_str(), "wb");
  
  if (!fp) 
    throw FreeMat::Exception(std::string("Unable to open file ") + filename + " for writing");
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  if (setjmp(png_jmpbuf(png_ptr)))
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  png_init_io(png_ptr, fp);
  /* write header */
  if (setjmp(png_jmpbuf(png_ptr)))
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  png_set_IHDR(png_ptr, info_ptr, width, height,
	       8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
	       PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);
  if (setjmp(png_jmpbuf(png_ptr)))
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  png_bytep* row_pointers = (png_bytep*) malloc(sizeof(png_bytep)*height);
  for (int y=0;y<height;y++)
    row_pointers[y] = (png_byte*) (data+3*y*width);
  png_write_image(png_ptr, row_pointers);
  free(row_pointers);
  if (setjmp(png_jmpbuf(png_ptr)))
    throw FreeMat::Exception(std::string("Internal error in PNG write codes..."));
  png_write_end(png_ptr, NULL);
  fclose(fp);
}

void WriteJPEGFile(std::string filename, byte *data, int width, int height) {
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE * outfile;               /* target file */
  JSAMPROW row_pointer[1];      /* pointer to JSAMPLE row[s] */
  int row_stride;               /* physical row width in image buffer */
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  if ((outfile = fopen(filename.c_str(), "wb")) == NULL) 
    throw FreeMat::Exception(std::string("Can not open file ") + filename);
  jpeg_stdio_dest(&cinfo, outfile);
  cinfo.image_width = width;      /* image width and height, in pixels */
  cinfo.image_height = height;
  cinfo.input_components = 3;           /* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, 90, TRUE /* limit to baseline-JPEG values */);
  jpeg_start_compress(&cinfo, TRUE);
  row_stride = width * 3; /* JSAMPLEs per row in image_buffer */
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = data + cinfo.next_scanline * row_stride;
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);
}

void WriteTIFFFile(std::string filename, byte *data, int width, int height) {
  TIFF *output;
  // Open the output image
  if((output = TIFFOpen(filename.c_str(), "w")) == NULL)
    throw FreeMat::Exception(std::string("Cound not open file") + filename + "for writing");
  // Write the tiff tags to the file
  TIFFSetField(output, TIFFTAG_IMAGEWIDTH, width);
  TIFFSetField(output, TIFFTAG_IMAGELENGTH, height);
  TIFFSetField(output, TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS);
  TIFFSetField(output, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(output, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(output, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(output, TIFFTAG_SAMPLESPERPIXEL, 3);
  // Actually write the image
  if(TIFFWriteEncodedStrip(output, 0, data, width * height * 3) == 0)
    throw FreeMat::Exception(std::string("Could not write image ") + filename);
  TIFFClose(output);
}

void WriteEPSFile(std::string filename, byte *data, int width, int height) {
  int linelen;
  int outcount;
  int remaining;
  int n;
  FILE *fp = fopen(filename.c_str(),"w");
  if (!fp)
    throw FreeMat::Exception(std::string("Error: unable to open file ") + filename + "for writing");
  // Emit standard header stuff
  fprintf(fp,"%%!PS-Adobe-3.0 EPSF-3.0\n");
  fprintf(fp,"%%%%Creator: FreeMat\n");
  fprintf(fp,"%%%%Title: %s\n",filename.c_str());
  fprintf(fp,"%%%%DocumentData: Clean7Bit\n");
  fprintf(fp,"%%%%Origin 0 0\n");
  fprintf(fp,"%%%%BoundingBox: 0 0 %d %d\n",width,height);
  fprintf(fp,"%%%%LanguageLevel: 2\n");
  fprintf(fp,"%%%%Pages: 1\n");
  fprintf(fp,"%%%%Page: 1 1\n");
  fprintf(fp,"/picstr %d string def\n",3*width);
  fprintf(fp,"gsave\n");
  fprintf(fp,"0 0 translate\n");
  fprintf(fp,"%d %d scale\n",width,height);
  fprintf(fp,"%d %d 8 [%d 0 0 -%d 0 %d]\n",width,height,width,height,height);
  fprintf(fp,"{currentfile picstr readhexstring pop} \n");
  fprintf(fp,"false 3 colorimage\n");
  outcount = 0;
  remaining = width*height;
  while (remaining>0) {
    linelen = 10;
    if (linelen>remaining)
      linelen = remaining;
    remaining -= linelen;
    for (n=outcount;n<outcount+linelen;n++) 
      fprintf(fp,"%02x%02x%02x",data[3*n],data[3*n+1],data[3*n+2]);
    outcount += linelen;
    fprintf(fp,"\n");
  }
  fprintf(fp,"grestore\n");  
  fprintf(fp,"%%%%EOF\n");
  fclose(fp);  
}

int GetShiftFromMask(int mask) {
  if (mask == 0) return 0;
  int shift = 0;
  while (!(mask & 1)) {
    shift++;
    mask >>= 1;
  }
  return shift;
}
