#ifndef MATIO_PRIVATE_H
#define MATIO_PRIVATE_H

#include "matio.h"

#ifndef EXTERN
#   ifdef __cplusplus
#       define EXTERN extern "C"
#   else
#       define EXTERN extern
#   endif
#endif

/*   endian.c     */
EXTERN double       doubleSwap(double  *a);
EXTERN float         floatSwap(float   *a);
EXTERN mat_int32_t   int32Swap(mat_int32_t  *a);
EXTERN mat_uint32_t uint32Swap(mat_uint32_t *a);
EXTERN mat_int16_t   int16Swap(mat_int16_t  *a);
EXTERN mat_uint16_t uint16Swap(mat_uint16_t *a);

/* read_data.c */
EXTERN int ReadDoubleData(mat_t *mat,double  *data,int data_type,int len);
EXTERN int ReadSingleData(mat_t *mat,float   *data,int data_type,int len);
EXTERN int ReadInt32Data (mat_t *mat,mat_int32_t *data,int data_type,int len);
EXTERN int ReadUInt32Data(mat_t *mat,mat_uint32_t *data,int data_type,int len);
EXTERN int ReadInt16Data (mat_t *mat,mat_int16_t *data,int data_type,int len);
EXTERN int ReadUInt16Data(mat_t *mat,mat_uint16_t *data,int data_type,int len);
EXTERN int ReadInt8Data  (mat_t *mat,mat_int8_t  *data,int data_type,int len);
EXTERN int ReadUInt8Data (mat_t *mat,mat_uint8_t  *data,int data_type,int len);
EXTERN int ReadCharData  (mat_t *mat,char  *data,int data_type,int len);
EXTERN int ReadDataSlab2(mat_t *mat,void *data,int class_type,int data_type,
               int *dims,int *start,int *stride,int *edge);
EXTERN int ReadDataSlabN(mat_t *mat,void *data,int class_type,int data_type,
               int rank,int *dims,int *start,int *stride,int *edge);
#if defined(HAVE_ZLIB)
EXTERN int ReadCompressedDoubleData(mat_t *mat,z_stream *z,double  *data,
               int data_type,int len);
EXTERN int ReadCompressedSingleData(mat_t *mat,z_stream *z,float   *data,
               int data_type,int len);
EXTERN int ReadCompressedInt32Data(mat_t *mat,z_stream *z,mat_int32_t *data,
               int data_type,int len);
EXTERN int ReadCompressedUInt32Data(mat_t *mat,z_stream *z,mat_uint32_t *data,
               int data_type,int len);
EXTERN int ReadCompressedInt16Data(mat_t *mat,z_stream *z,mat_int16_t *data,
               int data_type,int len);
EXTERN int ReadCompressedUInt16Data(mat_t *mat,z_stream *z,mat_uint16_t *data,
               int data_type,int len);
EXTERN int ReadCompressedInt8Data(mat_t *mat,z_stream *z,mat_int8_t  *data,
               int data_type,int len);
EXTERN int ReadCompressedUInt8Data(mat_t *mat,z_stream *z,mat_uint8_t  *data,
               int data_type,int len);
EXTERN int ReadCompressedCharData(mat_t *mat,z_stream *z,char *data,
               int data_type,int len);
EXTERN int ReadCompressedDataSlab2(mat_t *mat,z_stream *z,void *data,
               int class_type,int data_type,int *dims,int *start,int *stride,
               int *edge);
EXTERN int ReadCompressedDataSlabN(mat_t *mat,z_stream *z,void *data,
               int class_type,int data_type,int rank,int *dims,int *start,
               int *stride,int *edge);
#endif

#endif
