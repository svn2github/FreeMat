/*
 * Copyright (c) 2002-2006 Samit Basu
 * Copyright (c) 2006 Thomas Beutlich
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "Array.hpp"
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "Malloc.hpp"
#include "HandleList.hpp"
#include "Interpreter.hpp"
#include "File.hpp"
#include "Serialize.hpp"
#include "IEEEFP.hpp"
#include "Sparse.hpp"
#include "Core.hpp"
#include <QThread>
#include <QImage>
#include <QImageWriter>
#include <QColor>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include "Utils.hpp"
#include "PathSearch.hpp"
#if HAVE_PORTAUDIO19 || HAVE_PORTAUDIO18
#include "portaudio.h"
#endif
#include "Print.hpp"

class FilePtr {
public:
  FILE *fp;
  bool swapflag;
};
  
HandleList<FilePtr*> fileHandles;

static bool init = false;

void InitializeFileSubsystem() {
  if (init) 
    return;
  FilePtr *fptr = new FilePtr();
  fptr->fp = stdin;
  fptr->swapflag = false;
  fileHandles.assignHandle(fptr);
  fptr = new FilePtr();
  fptr->fp = stdout;
  fptr->swapflag = false;
  fileHandles.assignHandle(fptr);
  fptr = new FilePtr();
  fptr->fp = stderr;
  fptr->swapflag = false;
  fileHandles.assignHandle(fptr);
  init = true;
}


const int FRAMES_PER_BUFFER = 64;

class PlayBack {
public:
  char *data;
  int length;
  int channels;
  int framesToGo;
  int ptr;
  int elementSize;
  Array x;
};

#if HAVE_PORTAUDIO19 || HAVE_PORTAUDIO18

#if HAVE_PORTAUDIO19
static int DoPlayBackCallback(const void *, void *output,
			      unsigned long framesPerBuffer,
			      const PaStreamCallbackTimeInfo*,
			      PaStreamCallbackFlags,
			      void *userData) {
#endif

#if HAVE_PORTAUDIO18
static int DoPlayBackCallback(void *, void *output,
			      unsigned long framesPerBuffer,
			      PaTimestamp,
			      void *userData) {
#endif

  PlayBack *pb = (PlayBack*) userData;
  char *out = (char*) output;
  const char *dp = pb->data;
  int framesToCalc;
  int finished = 0;
  int i;
  if( (unsigned long) pb->framesToGo < framesPerBuffer )
    {
      framesToCalc = pb->framesToGo;
      pb->framesToGo = 0;
      finished = 1;
    }
  else
    {
      framesToCalc = framesPerBuffer;
      pb->framesToGo -= framesPerBuffer;
    }
  
  for( i=0; i<framesToCalc; i++ )
    {
      if (pb->channels == 1) {
	for (int j=0;j<pb->elementSize;j++)
	  *out++ = dp[pb->ptr*pb->elementSize + j];
	pb->ptr++;
      } else {
	for (int j=0;j<pb->elementSize;j++) 
	  *out++ = dp[pb->ptr*pb->elementSize + j];
	for (int j=0;j<pb->elementSize;j++)
	  *out++ = dp[(pb->ptr+pb->length)*pb->elementSize + j];
	pb->ptr++;
      }
    }
  /* zero remainder of final buffer */
  for( ; i<(int)framesPerBuffer; i++ )
    {
      if (pb->channels == 1)
	for (int j=0;j<pb->elementSize;j++)
	  *out++ = 0;
      else {
	for (int j=0;j<pb->elementSize;j++) {
	  *out++ = 0; /* left */
	  *out++ = 0; /* right */
	}
      }
    }
  return finished;
}

#if HAVE_PORTAUDIO19
static int DoRecordCallback(const void *input, void *,
			    unsigned long framesPerBuffer,
			    const PaStreamCallbackTimeInfo*,
			    PaStreamCallbackFlags,
			    void *userData) {
#endif

#if HAVE_PORTAUDIO18
static int DoRecordCallback(void *input, void *,
			    unsigned long framesPerBuffer,
			    PaTimestamp,
			    void *userData) {
#endif

  PlayBack *pb = (PlayBack*) userData;
  char *in = (char*) input;
  char *dp = pb->data;
  int framesToCalc;
  int finished = 0;
  int i;
  if( (unsigned long) pb->framesToGo < framesPerBuffer )
    {
      framesToCalc = pb->framesToGo;
      pb->framesToGo = 0;
      finished = 1;
    }
  else
    {
      framesToCalc = framesPerBuffer;
      pb->framesToGo -= framesPerBuffer;
    }
  
  for( i=0; i<framesToCalc; i++ )
    {
      if (pb->channels == 1) {
	for (int j=0;j<pb->elementSize;j++)
	  dp[pb->ptr*pb->elementSize + j] = *in++;
	pb->ptr++;
      } else {
	for (int j=0;j<pb->elementSize;j++) 
	  dp[pb->ptr*pb->elementSize + j] = *in++;
	for (int j=0;j<pb->elementSize;j++)
	  dp[(pb->ptr+pb->length)*pb->elementSize + j] = *in++;
	pb->ptr++;
      }
    }
  return finished;
}

static PaError err;
static bool RunningStream = false;
static PaStream *stream;
static PlayBack *pb_obj;

void PAInit() {
  err = Pa_Initialize();
  if (err != paNoError) 
    throw Exception(string("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
}

void PAShutdown() {
#ifdef HAVE_PORTAUDIO19
  while ( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    Pa_Sleep(100);
#endif

#ifdef HAVE_PORTAUDIO18
  while ( ( err = Pa_StreamActive( stream ) ) == 1 )
    Pa_Sleep(100);
#endif

  err = Pa_CloseStream(stream);
  if (err != paNoError) 
    throw Exception(string("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  Pa_Terminate();
  delete pb_obj;
  RunningStream = false;
}

void DoPlayBack(const void *data, int count, int channels, 
		int elementSize, unsigned long SampleFormat,
		int Rate, bool asyncMode, Array x) {
  if (RunningStream) 
    PAShutdown();
  PAInit();
  pb_obj = new PlayBack;
  pb_obj->x = x;
  pb_obj->data = (char *)data;
  pb_obj->length = count;
  pb_obj->channels = channels;
  pb_obj->ptr = 0;
  pb_obj->framesToGo = count;
  pb_obj->elementSize = elementSize;
#ifdef HAVE_PORTAUDIO19
  PaStreamParameters outputParameters;
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputParameters.channelCount = channels;
  outputParameters.sampleFormat = SampleFormat;
  outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;
  err = Pa_OpenStream(&stream,
		      NULL, /* no input */
		      &outputParameters,
		      Rate,
		      FRAMES_PER_BUFFER,
		      paNoFlag,
		      DoPlayBackCallback,
		      pb_obj);
#endif
#ifdef HAVE_PORTAUDIO18
  err = Pa_OpenStream(&stream,
		      paNoDevice, // No input device
		      0,          // No input
		      SampleFormat,
		      NULL,
		      Pa_GetDefaultOutputDeviceID(),
		      channels,
		      SampleFormat,
		      NULL,
		      Rate,
		      FRAMES_PER_BUFFER,
		      0,
		      paClipOff,
		      DoPlayBackCallback,
		      pb_obj);
#endif
  if (err != paNoError) 
    throw Exception(string("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  err = Pa_StartStream(stream);
  if (err != paNoError) 
    throw Exception(string("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  if (!asyncMode)
    PAShutdown();
  else
    RunningStream = true;
}

void DoRecord(void *data, int count, int channels, 
	      int elementSize, unsigned long SampleFormat,
	      int Rate) {
  if (RunningStream) 
    PAShutdown();
  PAInit();
  pb_obj = new PlayBack;
  pb_obj->data = (char *)data;
  pb_obj->length = count;
  pb_obj->channels = channels;
  pb_obj->ptr = 0;
  pb_obj->framesToGo = count;
  pb_obj->elementSize = elementSize;
#if HAVE_PORTAUDIO19
  PaStreamParameters inputParameters;
  inputParameters.device = Pa_GetDefaultInputDevice();
  inputParameters.channelCount = channels;
  inputParameters.sampleFormat = SampleFormat;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;
  err = Pa_OpenStream(&stream,
		      &inputParameters,
		      NULL,
		      Rate,
		      FRAMES_PER_BUFFER,
		      paNoFlag,
		      DoRecordCallback,
		      pb_obj);
#endif
#if HAVE_PORTAUDIO18
  err = Pa_OpenStream(&stream,
		      Pa_GetDefaultInputDeviceID(),
		      channels,
		      SampleFormat,
		      NULL,
		      paNoDevice,
		      0,
		      SampleFormat,
		      NULL,
		      Rate,
		      FRAMES_PER_BUFFER,
		      0,
		      0,
		      DoRecordCallback,
		      pb_obj);
#endif
  if (err != paNoError) 
    throw Exception(string("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  err = Pa_StartStream(stream);
  if (err != paNoError) 
    throw Exception(string("An error occured while using the portaudio stream: ") + Pa_GetErrorText(err));
  PAShutdown();
}
#endif


//!
//@Module WAVPLAY
//@@Section IO
//@@Usage
//Plays a linear PCM set of samples through the audio system.  This
//function is only available if the @|portaudio| library was available
//when FreeMat was built.  The syntax for the command is one of:
//@[
//   wavplay(y)
//   wavplay(y,sampling_rate)
//   wavplay(...,mode)
//@]
//where @|y| is a matrix of audio samples.  If @|y| has two columns, then
//the audio playback is in stereo.  The @|y| input can be of types 
//@|float, double, int32, int16, int8, uint8|.  For @|float| and 
//@|double| types, the sample values in @|y| must be between @|-1| and
//@|1|.  The @|sampling_rate| specifies the rate at which the data is 
//recorded.  If not specified, the @|sampling_rate| defaults to @|11025Hz|.
//Finally, you can specify a playback mode of @|'sync'| which is synchronous
//playback or a playback mode of @|'async'| which is asynchronous playback.
//For @|'sync'| playback, the wavplay function returns when the playback is
//complete.  For @|'async'| playback, the function returns immediately (unless
//a former playback is still issuing).
//!
ArrayVector WavPlayFunction(int nargout, const ArrayVector& argv) {
#if HAVE_PORTAUDIO18 || HAVE_PORTAUDIO19
  if(argv.size() == 0)
    throw Exception("wavplay requires at least one argument (the audio data to playback)");
  Array y(argv[0]);
  int SampleRate = 11025;
  string mode = "SYNC";
  if (argv.size() > 1)
    SampleRate = ArrayToInt32(argv[1]);
  if (argv.size() > 2)
    mode = argv[2].getContentsAsStringUpper();
  // Validate that the data is reasonable
  if ((!y.is2D())  || (!y.isVector() && (y.columns() > 2)) || 
      y.isReferenceType() || y.isComplex())
    throw Exception("wavplay only supports playback of 1 or 2 channel signals, which means a 1 or 2 column matrix");
  if (y.dataClass() == FM_DOUBLE)
    y.promoteType(FM_FLOAT);
  if ((y.dataClass() == FM_INT64) || (y.dataClass() == FM_UINT64))
    throw Exception("wavplay does not support 64 bit data types.");
  if (y.dataClass() == FM_UINT32)
    throw Exception("wavplay does not support unsigned 32-bit data types.");
  if (y.dataClass() == FM_UINT16)
    throw Exception("wavplay does not support unsigned 16-bit data types.");
  int samples;
  int channels;
  if (!y.isVector()) {
    channels = y.columns();
    samples = y.rows();
  } else {
    channels = 1;
    samples = y.getLength();
  }
  if (y.dataClass() == FM_FLOAT)
    DoPlayBack(y.getDataPointer(),samples,channels,sizeof(float),
	       paFloat32,SampleRate,mode != "SYNC",y);
  else if (y.dataClass() == FM_INT32)
    DoPlayBack(y.getDataPointer(),samples,channels,sizeof(int32),
	       paInt32,SampleRate,mode != "SYNC",y);
  else if (y.dataClass() == FM_INT16)
    DoPlayBack(y.getDataPointer(),samples,channels,sizeof(int16),
	       paInt16,SampleRate,mode != "SYNC",y);
  else if (y.dataClass() == FM_INT8)
    DoPlayBack(y.getDataPointer(),samples,channels,sizeof(int8),
	       paInt8,SampleRate,mode != "SYNC",y);
  else if (y.dataClass() == FM_UINT8)
    DoPlayBack(y.getDataPointer(),samples,channels,sizeof(uint8),
	       paUInt8,SampleRate,mode != "SYNC",y);
  else
    throw Exception("wavplay does not support this data types.");
#else
  throw Exception("Audio read/write support not available.  Please build the PortAudio library and rebuild FreeMat to enable this functionality.");
#endif
  return ArrayVector();
}

//!
//@Module WAVRECORD
//@@Section IO
//@@Usage
//Records linear PCM sound from the audio system.  This function is
//only available if the @|portaudio| library was available when FreeMat
//was built.  The syntax for this command is one of:
//@[
//  y = wavrecord(samples,rate)
//  y = wavrecord(...,channels)
//  y = wavrecord(...,'datatype')
//@]
//where @|samples| is the number of samples to record, and @|rate| is the
//sampling rate.  If not specified, the @|rate| defaults to @|11025Hz|.
//If you want to record in stero, specify @|channels = 2|.  Finally, you
//can specify the type of the recorded data (defaults to @|FM_DOUBLE|).
//Valid choices are @|float, double, int32, int16, int8, uint8|.
//!
ArrayVector WavRecordFunction(int nargout, const ArrayVector& argv) {
#if HAVE_PORTAUDIO18 || HAVE_PORTAUDIO19
  if (argv.size() < 1)
    throw Exception("wavrecord requires at least 1 argument (the number of samples to record)");
  int samples = ArrayToInt32(argv[0]);
  int rate = 11025;
  int channels = 1;
  Class datatype = FM_DOUBLE;
  ArrayVector argvCopy(argv);
  if ((argvCopy.size() > 1) && (argvCopy.back().isString())) {
    string typestring = argvCopy.back().getContentsAsStringUpper();
    if ((typestring == "FLOAT") || (typestring == "SINGLE"))
      datatype = FM_FLOAT;
    else if (typestring == "DOUBLE")
      datatype = FM_DOUBLE;
    else if (typestring == "INT32")
      datatype = FM_INT32;
    else if (typestring == "INT16")
      datatype = FM_INT16;
    else if (typestring == "INT8")
      datatype = FM_INT8;
    else if (typestring == "UINT8")
      datatype = FM_UINT8;
    else
      throw Exception("unrecognized data type - expecting one of: double, float, single, int32, int16, int8, uint8");
    argvCopy.pop_back();
  }
  // Check for a channel spec and a sampling rate
  while (argvCopy.size() > 1) {
    int ival = ArrayToInt32(argvCopy.back());
    if (ival > 2)
      rate = ival;
    else
      channels = ival;
    argvCopy.pop_back();
  }
  Class rdatatype(datatype);
  Array retvec;
  if (rdatatype == FM_DOUBLE) rdatatype = FM_FLOAT;
  void *dp = Array::allocateArray(rdatatype,samples*channels);
  switch(rdatatype) {
  default: throw Exception("Illegal data type argument for wavrecord");
  case FM_FLOAT:
    DoRecord(dp,samples,channels,sizeof(float),paFloat32,rate);
    retvec = Array(FM_FLOAT,Dimensions(samples,channels),dp);
    break;
  case FM_INT32:
    DoRecord(dp,samples,channels,sizeof(int32),paInt32,rate);
    retvec = Array(FM_INT32,Dimensions(samples,channels),dp);
    break;
  case FM_INT16:
    DoRecord(dp,samples,channels,sizeof(int16),paInt16,rate);
    retvec = Array(FM_INT16,Dimensions(samples,channels),dp);
    break;
  case FM_INT8:
    DoRecord(dp,samples,channels,sizeof(int8),paInt8,rate);
    retvec = Array(FM_INT8,Dimensions(samples,channels),dp);
    break;
  case FM_UINT8:
    DoRecord(dp,samples,channels,sizeof(uint8),paUInt8,rate);
    retvec = Array(FM_UINT8,Dimensions(samples,channels),dp);
    break;
  }
  retvec.promoteType(datatype);
  return ArrayVector() << retvec;
#else
  throw Exception("Audio read/write support not available.  Please build the PortAudio library and rebuild FreeMat to enable this functionality.");
#endif
  return ArrayVector();
}

//!
//@Module FORMAT Control the Format of Matrix Display
//@@Section IO
//@@Usage
//FreeMat supports several modes for displaying matrices (either through the
//@|disp| function or simply by entering expressions on the command line.  
//There are several options for the format command.  The default mode is equivalent
//to
//@[
//   format short
//@]
//which generally displays matrices with 4 decimals, and scales matrices if the entries
//have magnitudes larger than roughly @|1e2| or smaller than @|1e-2|.   For more 
//information you can use 
//@[
//   format long
//@]
//which displays roughly 7 decimals for @|float| and @|complex| arrays, and 14 decimals
//for @|double| and @|dcomplex|.  You can also use
//@[
//   format short e
//@]
//to get exponential format with 4 decimals.  Matrices are not scaled for exponential 
//formats.  Similarly, you can use
//@[
//   format long e
//@]
//which displays the same decimals as @|format long|, but in exponential format.
//You can also use the @|format| command to retrieve the current format:
//@[
//   s = format
//@]
//where @|s| is a string describing the current format.
//@@Example
//We start with the short format, and two matrices, one of double precision, and the
//other of single precision.
//@<
//format short
//a = randn(4)
//b = float(randn(4))
//@>
//Note that in the short format, these two matrices are displayed with the same format.
//In @|long| format, however, they display differently
//@<
//format long
//a
//b
//@>
//Note also that we we scale the contents of the matrices, FreeMat rescales the entries
//with a scale premultiplier.
//@<
//format short
//a*1e4
//a*1e-4
//b*1e4
//b*1e-4
//@>
//Next, we use the exponential formats:
//@<
//format short e
//a*1e4
//a*1e-4
//b*1e4
//b*1e-4
//@>
//Finally, if we assign the @|format| function to a variable, we can retrieve the 
//current format:
//@<
//format short
//t = format
//@>
//!
ArrayVector FormatFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() > 0) {
    string argtxt;
    for (int i=0;i<arg.size();i++) argtxt += arg[i].getContentsAsStringUpper();
    if (argtxt == "NATIVE") SetPrintFormatMode(format_native);
    else if (argtxt == "SHORT") SetPrintFormatMode(format_short);
    else if (argtxt == "LONG") SetPrintFormatMode(format_long);
    else if (argtxt == "SHORTE") SetPrintFormatMode(format_short_e);
    else if (argtxt == "LONGE") SetPrintFormatMode(format_long_e);
    else throw Exception("unrecognized argument to the format command");
  }
  if (nargout > 0) {
    switch(GetPrintFormatMode()) {
    case format_native:
      return ArrayVector() << Array::stringConstructor("native");
    case format_short:
      return ArrayVector() << Array::stringConstructor("short");
    case format_long:
      return ArrayVector() << Array::stringConstructor("long");
    case format_short_e:
      return ArrayVector() << Array::stringConstructor("short e");
    case format_long_e:
      return ArrayVector() << Array::stringConstructor("long e");
    }
    return ArrayVector() << Array::stringConstructor("unknown?");
  }
  return ArrayVector();
}

//!
//@Module IMREAD Read Image File To Matrix
//@@Section IO
//@@Usage
//Reads the image data from the given file into a matrix.  Note that
//FreeMat's support for @|imread| is not complete.  Only some of the
//formats specified in the MATLAB API are implemented.  The syntax
//for its use is
//@[
//  [A,map,alpha] = imread(filename)
//@]
//where @|filename| is the name of the file to read from.  The returned
//arrays @|A| contain the image data, @|map| contains the colormap information
//(for indexed images), and @|alpha| contains the alphamap (transparency).
//The returned values will depend on the type of the original image.  Generally
//you can read images in the @|jpg,png,xpm,ppm| and some other formats.
//!
static ArrayVector imreadHelperIndexed(QImage img) {
  QVector<QRgb> colorTable(img.colorTable());
  double *ctable_dp = (double*) 
    Array::allocateArray(FM_DOUBLE,colorTable.size()*3);
  int numcol = colorTable.size();
  for (int i=0;i<numcol;i++) {
    QColor c(colorTable[i]);
    ctable_dp[i] = (double) c.redF();
    ctable_dp[i+numcol] = (double) c.greenF();
    ctable_dp[i+2*numcol] = (double) c.blueF();
  }
  Array ctable(FM_DOUBLE,Dimensions(numcol,3),ctable_dp);
  uint8 *img_data_dp = (uint8*) 
    Array::allocateArray(FM_UINT8,img.width()*img.height());
  for (int row=0;row<img.height();row++) {
    uchar *p = img.scanLine(row);
    for (int col=0;col<img.width();col++) 
      img_data_dp[row+col*img.height()] = p[col];
  }
  Array A(FM_UINT8,Dimensions(img.height(),img.width()),img_data_dp);
  QImage alpha(img.alphaChannel());
  uint8 *img_alpha_dp = (uint8*)
    Array::allocateArray(FM_UINT8,img.width()*img.height());
  for (int row=0;row<alpha.height();row++) {
    uchar *p = alpha.scanLine(row);
    for (int col=0;col<alpha.width();col++)
      img_alpha_dp[row+col*img.height()] = p[col];
  }
  Array trans(FM_UINT8,Dimensions(img.height(),img.width()),img_alpha_dp);
  return ArrayVector() << A << ctable << trans;
}

static ArrayVector imreadHelperRGB32(QImage img) {
  if (img.allGray()) {
	  uint8 *img_data_dp = (uint8*) 
		Array::allocateArray(FM_UINT8,img.width()*img.height());
	  int imgcnt = img.height()*img.width();
	  for (int row=0;row<img.height();row++) {
		QRgb *p = (QRgb*) img.scanLine(row);
		for (int col=0;col<img.width();col++) {
		  int ndx = row+col*img.height();
		  img_data_dp[ndx] = qGray(p[col]);
		}
	  }
	  return ArrayVector() << 
		Array(FM_UINT8,Dimensions(img.height(),img.width()),img_data_dp)
				   << Array::emptyConstructor() 
				   << Array::emptyConstructor();
  }
  else {
	  uint8 *img_data_dp = (uint8*) 
		Array::allocateArray(FM_UINT8,img.width()*img.height()*3);
	  int imgcnt = img.height()*img.width();
	  for (int row=0;row<img.height();row++) {
		QRgb *p = (QRgb*) img.scanLine(row);
		for (int col=0;col<img.width();col++) {
		  int ndx = row+col*img.height();
		  img_data_dp[ndx] = qRed(p[col]);
		  img_data_dp[ndx+1*imgcnt] = qGreen(p[col]);
		  img_data_dp[ndx+2*imgcnt] = qBlue(p[col]);
		}
	  }
	  return ArrayVector() << 
		Array(FM_UINT8,Dimensions(img.height(),img.width(),3),img_data_dp)
				   << Array::emptyConstructor() 
				   << Array::emptyConstructor();
  }
}

static ArrayVector imreadHelperARGB32(QImage img) {
   uint8 *img_alpha_dp = (uint8*) 
     Array::allocateArray(FM_UINT8,img.width()*img.height());
  if (img.allGray()) {
	  uint8 *img_data_dp = (uint8*)
		Array::allocateArray(FM_UINT8,img.width()*img.height());
	  int imgcnt = img.height()*img.width();
	  for (int row=0;row<img.height();row++) {
		QRgb *p = (QRgb*) img.scanLine(row);
		for (int col=0;col<img.width();col++) {
		  int ndx = row+col*img.height();
		  img_data_dp[ndx] = qGray(p[col]);
		  img_alpha_dp[ndx] = qAlpha(p[col]);
		}
	  }
	  return ArrayVector() << 
		Array(FM_UINT8,Dimensions(img.height(),img.width()),img_data_dp)
				   << Array::emptyConstructor() 
				   << 
		Array(FM_UINT8,Dimensions(img.height(),img.width()),img_alpha_dp);
  }
  else
  {
	  uint8 *img_data_dp = (uint8*)
		Array::allocateArray(FM_UINT8,img.width()*img.height()*3);
	  int imgcnt = img.height()*img.width();
	  for (int row=0;row<img.height();row++) {
		QRgb *p = (QRgb*) img.scanLine(row);
		for (int col=0;col<img.width();col++) {
		  int ndx = row+col*img.height();
		  img_data_dp[ndx] = qRed(p[col]);
		  img_data_dp[ndx+1*imgcnt] = qGreen(p[col]);
		  img_data_dp[ndx+2*imgcnt] = qBlue(p[col]);
		  img_alpha_dp[ndx] = qAlpha(p[col]);
		}
	  }
	  return ArrayVector() << 
		Array(FM_UINT8,Dimensions(img.height(),img.width(),3),img_data_dp)
				   << Array::emptyConstructor() 
				   << 
		Array(FM_UINT8,Dimensions(img.height(),img.width()),img_alpha_dp);
  }
}

ArrayVector ImReadFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval) {
  PathSearcher psearch(eval->getTotalPath());
  if (arg.size() == 0)
    throw Exception("imread requires a filename to read.");
  string filename(ArrayToString(arg[0]));
  string completename;
  try {
    completename = psearch.ResolvePath(filename);
  } catch (Exception& e) {
    throw Exception("unable to find file " + completename);
  }
  // Construct the QImage object
  QImage img(QString::fromStdString(completename));
  if (img.isNull())
    throw Exception("unable to read file " + completename);
  if (img.format() == QImage::Format_Invalid)
    throw Exception("file " + completename + " is invalid");
  if (img.format() == QImage::Format_Indexed8) return imreadHelperIndexed(img);
  if (img.format() == QImage::Format_RGB32) return imreadHelperRGB32(img);
  if (img.format() == QImage::Format_ARGB32) return imreadHelperARGB32(img);
  throw Exception("unsupported image format - only 8 bit indexed and 24 bit RGB and 32 bit ARGB images are supported");
  return ArrayVector();
}

//!
//@Module IMWRITE Write Matrix to Image File
//@@Section IO
//@@Usage
//Write the image data from the matrix into a given file.  Note that
//FreeMat's support for @|imread| is not complete.  Only some of the
//formats specified in the MATLAB API are implemented.  The syntax
//for its use is
//@[
//  imwrite(filename, A)
//  imwrite(filename, A, map)
//  imwrite(filename, A, map, alpha)
//@]
//where @|filename| is the name of the file to write to.  The input
//arrays @|A| contain the image data, @|map| contains the colormap information
//(for indexed images), and @|alpha| contains the alphamap (transparency).
//The returned values will depend on the type of the original image.  Generally
//you can write images in the @|jpg,png,xpm,ppm| and some other formats.
//!
QImage imwriteHelperIndexed(Array A, Array ctable, Array trans) {
  QImage img(A.columns(), A.rows(), QImage::Format_Indexed8);
  uint8 *img_data_dp = (uint8*) A.getDataPointer();
  uint8 *ctable_dp = (uint8*) ctable.getDataPointer();
  uint8 *img_alpha_dp = (uint8*) trans.getDataPointer();

  for (int row=0;row<img.height();row++) {
    uchar *p = img.scanLine(row);
    for (int col=0;col<img.width();col++) 
      p[col] = img_data_dp[row+col*img.height()];
  }
  
  if (ctable_dp) {
	  QVector<QRgb> colorTable(ctable.getLength()/3);
	  int numcol = colorTable.size();
	  for (int i=0;i<numcol;i++)
		colorTable[i] = qRgb(int(ctable_dp[i]),
		                     int(ctable_dp[i+numcol]),
		                     int(ctable_dp[i+2*numcol]));
	  img.setColorTable(colorTable);
  }
  else {
	  int numrow = 256;
	  QVector<QRgb> colorTable(numrow);
	  for (int i=0;i<numrow;i++)
		colorTable[i] = qRgb(i, i, i);
	  img.setColorTable(colorTable);
  }
  
  if (img_alpha_dp) {
	  QImage alpha(A.columns(), A.rows(), QImage::Format_Indexed8);
	  for (int row=0;row<alpha.height();row++) {
		uchar *p = alpha.scanLine(row);
		for (int col=0;col<alpha.width();col++)
		  p[col] = img_alpha_dp[row+col*img.height()];
	  }
	  img.setAlphaChannel(alpha);
  }
  return img;
}

QImage imwriteHelperRGB32(Array A) {
  uint8 *img_data_dp = (uint8*) A.getDataPointer();
  QImage img(A.columns(), A.rows(), QImage::Format_RGB32);
  int imgcnt = img.height()*img.width();
  for (int row=0;row<img.height();row++) {
	QRgb *p = (QRgb*) img.scanLine(row);
	for (int col=0;col<img.width();col++) {
	  int ndx = row+col*img.height();
	  p[col] = qRgb(img_data_dp[ndx], 
	                img_data_dp[ndx+1*imgcnt], 
	                img_data_dp[ndx+2*imgcnt]);
	}
  }
  return img;
}

QImage imwriteHelperARGB32(Array A, Array trans) {
  QImage img(A.columns(), A.rows(), QImage::Format_ARGB32);
  uint8 *img_data_dp = (uint8*) A.getDataPointer();
  uint8 *img_alpha_dp = (uint8*) trans.getDataPointer();
  int imgcnt = img.height()*img.width();
  for (int row=0;row<img.height();row++) {
	QRgb *p = (QRgb*) img.scanLine(row);
	for (int col=0;col<img.width();col++) {
	  int ndx = row+col*img.height();
	  p[col] = qRgba(img_data_dp[ndx], 
	                 img_data_dp[ndx+1*imgcnt], 
	                 img_data_dp[ndx+2*imgcnt], 
	                 img_alpha_dp[ndx]);
	}
  }
  return img;
}

Array convert2uint8(Array A) {
  if (A.dataClass() ==  FM_UINT8) {
    return A;
  }
  //convert other data types to uint8
  uint8 *img_data_dp1 = (uint8*) 
	Array::allocateArray(FM_UINT8,A.getLength());
  
  if (A.dataClass() ==  FM_LOGICAL) {
    uint8 *img_data_dp = (uint8*) A.getDataPointer();
    int scale = 255;
    for (int i = 0; i< A.getLength(); i++)
      if (img_data_dp[i])
        img_data_dp1[i] = 255;
      else
        img_data_dp1[i] = 0;
  }
  else if (A.dataClass() ==  FM_INT8) {
    int8 *img_data_dp = (int8*) A.getDataPointer();
    for (int i = 0; i< A.getLength(); i++)
      img_data_dp1[i] = uint8(img_data_dp[i]);
  }
  else if (A.dataClass() ==  FM_UINT16) {
    uint16 *img_data_dp = (uint16*) A.getDataPointer();
    for (int i = 0; i< A.getLength(); i++)
      img_data_dp1[i] = uint8(img_data_dp[i]);
  }
  else if (A.dataClass() ==  FM_INT16) {
    int16 *img_data_dp = (int16*) A.getDataPointer();
    for (int i = 0; i< A.getLength(); i++)
      img_data_dp1[i] = uint8(img_data_dp[i]);
  }
  else if (A.dataClass() ==  FM_UINT32) {
    uint32 *img_data_dp = (uint32*) A.getDataPointer();
    for (int i = 0; i< A.getLength(); i++)
      img_data_dp1[i] = uint8(img_data_dp[i]);
  }
  else if (A.dataClass() ==  FM_INT32) {
    int32 *img_data_dp = (int32*) A.getDataPointer();
    for (int i = 0; i< A.getLength(); i++)
      img_data_dp1[i] = uint8(img_data_dp[i]);
  }
  else if (A.dataClass() ==  FM_UINT64) {
    uint64 *img_data_dp = (uint64*) A.getDataPointer();
    for (int i = 0; i< A.getLength(); i++)
      img_data_dp1[i] = uint8(img_data_dp[i]);
  }
  else if (A.dataClass() ==  FM_INT64) {
    int64 *img_data_dp = (int64*) A.getDataPointer();
    for (int i = 0; i< A.getLength(); i++)
      img_data_dp1[i] = uint8(img_data_dp[i]);
  }
  else if (A.dataClass() ==  FM_FLOAT) {
    float *img_data_dp = (float*) A.getDataPointer();
    float scale = 255;
    for (int i = 0; i< A.getLength(); i++)
      img_data_dp1[i] = uint8(scale*img_data_dp[i]);
  }
  else if (A.dataClass() ==  FM_DOUBLE) {
    double *img_data_dp = (double*) A.getDataPointer();
    double scale = 255;
    for (int i = 0; i< A.getLength(); i++)
      img_data_dp1[i] = uint8(scale*img_data_dp[i]);
  }
  else
    throw Exception("invalid data type");
    
  return Array(FM_UINT8,A.dimensions(),img_data_dp1);
}  

ArrayVector ImWriteFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval) {
  PathSearcher psearch(eval->getTotalPath());
  if (arg.size() < 2)
    throw Exception("imwrite requires at least a filename and a matrix");
  string filename(ArrayToString(arg[0]));
  QString FileName = QString::fromStdString(filename);
  QByteArray ImageFormat;
  ImageFormat.append(QFileInfo(FileName).suffix());
  // Construct the QImageWriter object
  QImageWriter imgWriter(FileName,ImageFormat);
  if (!imgWriter.canWrite()) {
     throw Exception("unable to write image file " + filename);
  }

  Array A(arg[1]);
  if (A.dimensions().getLength() == 2) { // choose QImage::Format_Indexed8
    if (arg.size() == 2) { // 8-bit grayscale image
        Array ctable(FM_UINT8, Dimensions(255,1),NULL);
        Array trans(FM_UINT8, A.dimensions(),NULL);
        QImage img = imwriteHelperIndexed(convert2uint8(A), ctable, trans);
	    if (!imgWriter.write(QImage(img)))
	       throw Exception("cannot create image file" + filename);
    }
    else if (arg.size() == 3) { // 8-bit indexed color image
		Array ctable(arg[2]);
		if (ctable.getLength() != 0 && ctable.dimensions().getColumns() != 3)
			throw Exception("color map should be a 3 columns matrix");
		Array trans(FM_UINT8,A.dimensions(),NULL);
		QImage img = imwriteHelperIndexed(convert2uint8(A), convert2uint8(ctable), trans);
		if (!imgWriter.write(img))
			throw Exception("cannot create image file" + filename);
	}
	else if (arg.size() == 4) { // 8-bit indexed color image with alpha channel
		Array ctable(arg[2]);
		if (ctable.getLength() != 0 && ctable.dimensions().getColumns() != 3)
			throw Exception("color map should be a 3 columns matrix");
		Array trans(arg[3]);
		eval->warningMessage("saving alpha/transparent channel will increase file size");
		QImage img = imwriteHelperIndexed(convert2uint8(A), convert2uint8(ctable), convert2uint8(trans));
		if (!imgWriter.write(img))
		  throw Exception("cannot create image file" + filename);
	}
	else
		throw Exception("invalide input number of arguments");
  }
  else if (A.dimensions().getLength() == 3) { // choose QImage::Format_RGB32 or Format_ARGB32
    if (arg.size() == 2) {
        QImage img = imwriteHelperRGB32(convert2uint8(A));
	    if (!imgWriter.write(QImage(img)))
	       throw Exception("cannot create image file" + filename);
	}
	else if (arg.size() == 3) {
		Array trans(arg[2]);
		if (A.rows() == trans.rows() && A.columns() == trans.columns() ) {
		    // the third argument is alpha channel
		    QImage img = imwriteHelperARGB32(convert2uint8(A), convert2uint8(trans));
			if (!imgWriter.write(QImage(img)))
			   throw Exception("cannot create image file" + filename);
		}
		else {
			if (trans.getLength() != 0)
		  		eval->warningMessage("ignore colormap argument");
		    QImage img = imwriteHelperRGB32(convert2uint8(A));
			if (!imgWriter.write(QImage(img)))
			   throw Exception("cannot create image file" + filename);
	    }
	}
	else if (arg.size() == 4) {
		Array ctable(arg[2]);
		if (ctable.getLength() != 0)
	  		eval->warningMessage("ignore colormap argument");
		Array trans(arg[3]);
		if (A.rows() == trans.rows() && A.columns() == trans.columns() ) {
		    // the third argument is alpha/transparent channel
		    QImage img = imwriteHelperARGB32(convert2uint8(A), convert2uint8(trans));
			if (!imgWriter.write(QImage(img)))
			   throw Exception("cannot create image file" + filename);
		}
		else {
			if (trans.getLength() != 0)
		  		eval->warningMessage("ignore invalid transparent argument");
		    QImage img = imwriteHelperRGB32(convert2uint8(A));
			if (!imgWriter.write(QImage(img)))
			   throw Exception("cannot create image file" + filename);
	    }
	}
	else
		throw Exception("invalide input number of arguments");
  }
  else
    throw Exception("invalid matrix dimension");

  return ArrayVector();
}

//!
//@Module SETPRINTLIMIT Set Limit For Printing Of Arrays
//@@Section IO
//@@Usage
//Changes the limit on how many elements of an array are printed
//using either the @|disp| function or using expressions on the
//command line without a semi-colon.  The default is set to 
//one thousand elements.  You can increase or decrease this
//limit by calling
//@[
//  setprintlimit(n)
//@]
//where @|n| is the new limit to use.
//@@Example
//Setting a smaller print limit avoids pages of output when you forget the semicolon on an expression.
//@<
//A = randn(512);
//setprintlimit(10)
//A
//setprintlimit(1000)
//@>
//!
ArrayVector SetPrintLimitFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("setprintlimit requires one, scalar integer argument");
  Array tmp(arg[0]);
  eval->setPrintLimit(tmp.getContentsAsIntegerScalar());
  return ArrayVector();
}

//!
//@Module GETPRINTLIMIT Get Limit For Printing Of Arrays
//@@Section IO
//@@Usage
//Returns the limit on how many elements of an array are printed
//using either the @|disp| function or using expressions on the
//command line without a semi-colon.  The default is set to 
//one thousand elements.  You can increase or decrease this
//limit by calling @|setprintlimit|.  This function is provided
//primarily so that you can temporarily change the output truncation
//and then restore it to the previous value (see the examples).
//@[
//   n=getprintlimit
//@]
//where @|n| is the current limit in use.
//@@Example
//Here is an example of using @|getprintlimit| along with @|setprintlimit| to temporarily change the output behavior of FreeMat.
//@<
//A = randn(100,1);
//n = getprintlimit
//setprintlimit(5);
//A
//setprintlimit(n)
//@>
//!
ArrayVector GetPrintLimitFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  Array tmp(Array::uint32Constructor(eval->getPrintLimit()));
  ArrayVector retval;
  retval.push_back(tmp);
  return retval;
}

void SwapBuffer(char* cp, int count, int elsize) {
  char tmp;
  for (int i=0;i<count;i++)
    for (int j=0;j<elsize/2;j++) {
      tmp = cp[i*elsize+j];
      cp[i*elsize+j] = cp[i*elsize+elsize-1-j];
      cp[i*elsize+elsize-1-j] = tmp;
    }    
}

#define MATCH(x) (prec==x)
void processPrecisionString(string prec, Class &dataClass, int& elementSize, int& swapSize) {
  if (MATCH("uint8") || MATCH("uchar") || MATCH("unsigned char")) {
    dataClass = FM_UINT8;
    elementSize = 1;
    swapSize = 1;
    return;
  }
  if (MATCH("int8") || MATCH("char") || MATCH("integer*1")) {
    dataClass = FM_INT8;
    elementSize = 1;
    swapSize = 1;
    return;
  }
  if (MATCH("uint16") || MATCH("unsigned short")) {
    dataClass = FM_UINT16;
    elementSize = 2;
    swapSize = 2;
    return;
  }
  if (MATCH("int16") || MATCH("short") || MATCH("integer*2")) {
    dataClass = FM_INT16;
    elementSize = 2;
    swapSize = 2;
    return;
  }
  if (MATCH("uint32") || MATCH("unsigned int")) {
    dataClass = FM_UINT32;
    elementSize = 4;
    swapSize = 4;
    return;
  }
  if (MATCH("int32") || MATCH("int") || MATCH("integer*4")) {
    dataClass = FM_INT32;
    elementSize = 4;
    swapSize = 4;
    return;
  }
  if (MATCH("uint64")) {
    dataClass = FM_UINT64;
    elementSize = 8;
    swapSize = 8;
    return;
  }
  if (MATCH("int64") || MATCH("integer*8")) {
    dataClass = FM_INT64;
    elementSize = 8;
    swapSize = 8;
    return;
  }
  if (MATCH("single") || MATCH("float32") || MATCH("float") || MATCH("real*4")) {
    dataClass = FM_FLOAT;
    elementSize = 4;
    swapSize = 4;
    return;
  }
  if (MATCH("double") || MATCH("float64") || MATCH("real*8")) {
    dataClass = FM_DOUBLE;
    elementSize = 8;
    swapSize = 8;
    return;
  }
  if (MATCH("complex") || MATCH("complex*8")) {
    dataClass = FM_COMPLEX;
    elementSize = 8;
    swapSize = 4;
    return;
  }
  if (MATCH("dcomplex") || MATCH("complex*16")) {
    dataClass = FM_DCOMPLEX;
    elementSize = 16;
    swapSize = 8;
    return;
  }
  throw Exception("invalid precision type");
}
#undef MATCH

//!
//@Module FOPEN File Open Function
//@@Section IO
//@@Usage
//Opens a file and returns a handle which can be used for subsequent
//file manipulations.  The general syntax for its use is
//@[
//  fp = fopen(fname,mode,byteorder)
//@]
//Here @|fname| is a string containing the name of the file to be 
//opened.  @|mode| is the mode string for the file open command.
//The first character of the mode string is one of the following:
//\begin{itemize}
//  \item @|'r'|  Open  file  for  reading.  The file pointer is placed at
//          the beginning of the file.  The file can be read from, but
//	  not written to.
//  \item @|'r+'|   Open for reading and writing.  The file pointer is
//          placed at the beginning of the file.  The file can be read
//	  from and written to, but must exist at the outset.
//  \item @|'w'|    Open file for writing.  If the file already exists, it is
//          truncated to zero length.  Otherwise, a new file is
//	  created.  The file pointer is placed at the beginning of
//	  the file.
//  \item @|'w+'|   Open for reading and writing.  The file is created  if  
//          it  does not  exist, otherwise it is truncated to zero
//	  length.  The file pointer placed at the beginning of the file.
//  \item @|'a'|    Open for appending (writing at end of file).  The file  is  
//          created  if it does not exist.  The file pointer is placed at
//	  the end of the file.
//  \item @|'a+'|   Open for reading and appending (writing at end of file).   The
//          file  is created if it does not exist.  The file pointer is
//	  placed at the end of the file.
//\end{itemize}
//On some platforms (e.g. Win32) it is necessary to add a 'b' for 
//binary files to avoid the operating system's 'CR/LF<->CR' translation.
//
//Finally, FreeMat has the ability to read and write files of any
//byte-sex (endian).  The third (optional) input indicates the 
//byte-endianness of the file.  If it is omitted, the native endian-ness
//of the machine running FreeMat is used.  Otherwise, the third
//argument should be one of the following strings:
//\begin{itemize}
//   \item @|'le','ieee-le','little-endian','littleEndian','little'|
//   \item @|'be','ieee-be','big-endian','bigEndian','big'|
//\end{itemize}
//	
//If the file cannot be opened, or the file mode is illegal, then
//an error occurs. Otherwise, a file handle is returned (which is
//an integer).  This file handle can then be used with @|fread|,
//@|fwrite|, or @|fclose| for file access.
//
//Note that three handles are assigned at initialization time:
//\begin{itemize}
//   \item Handle 0 - is assigned to standard input
//   \item Handle 1 - is assigned to standard output
//   \item Handle 2 - is assigned to standard error
//\end{itemize}
//These handles cannot be closed, so that user created file handles start at @|3|.
//
//@@Examples
//Here are some examples of how to use @|fopen|.  First, we create a new 
//file, which we want to be little-endian, regardless of the type of the machine.
//We also use the @|fwrite| function to write some floating point data to
//the file.
//@<
//fp = fopen('test.dat','wb','ieee-le')
//fwrite(fp,float([1.2,4.3,2.1]))
//fclose(fp)
//@>
//Next, we open the file and read the data back
//@<
//fp = fopen('test.dat','rb','ieee-le')
//fread(fp,[1,3],'float')
//fclose(fp)
//@>
//Now, we re-open the file in append mode and add two additional @|float|s to the
//file.
//@<
//fp = fopen('test.dat','a+','le')
//fwrite(fp,float([pi,e]))
//fclose(fp)
//@>
//Finally, we read all 5 @|float| values from the file
//@<
//fp = fopen('test.dat','rb','ieee-le')
//fread(fp,[1,5],'float')
//fclose(fp)
//@>
//!
ArrayVector FopenFunction(int nargout, const ArrayVector& arg) {
  uint32 testEndian = 0xFEEDFACE;
  uint8 *dp;
  bool bigEndian;

  dp = (uint8*) &testEndian;
  bigEndian = (dp[0] == 0xFE);

  if (arg.size() > 3)
    throw Exception("too many arguments to fopen");
  if (arg.size() < 1)
    throw Exception("fopen requires at least one argument (a filename)");
  if (!(arg[0].isString()))
    throw Exception("First argument to fopen must be a filename");
  string fname = arg[0].getContentsAsString();
  string mode = "rb";
  if (arg.size() > 1) {
    if (!arg[1].isString())
      throw Exception("Access mode to fopen must be a string");
    mode = arg[1].getContentsAsString();
  }
  bool swapendian = false;
  if (arg.size() > 2) {
    string swapflag = arg[2].getContentsAsString();
    if (swapflag=="swap") {
      swapendian = true;
    } else if ((swapflag=="le") ||
	       (swapflag=="ieee-le") ||
	       (swapflag=="little-endian") ||
	       (swapflag=="littleEndian") ||
	       (swapflag=="little")) {
      swapendian = bigEndian;
    } else if ((swapflag=="be") ||
	       (swapflag=="ieee-be") ||
	       (swapflag=="big-endian") ||
	       (swapflag=="bigEndian") ||
	       (swapflag=="big")) {
      swapendian = !bigEndian;
    } else if (!arg[2].isEmpty())
      throw Exception("swap flag must be 'swap' or an endian spec ('le','ieee-le','little-endian','littleEndian','little','be','ieee-be','big-endian','bigEndian','big')");
  }
  FILE *fp = fopen(fname.c_str(),mode.c_str());
  if (!fp)
    throw Exception(strerror(errno) + string(" for fopen argument ") + fname);
  FilePtr *fptr = new FilePtr();
  fptr->fp = fp;
  fptr->swapflag = swapendian;
  unsigned int rethan = fileHandles.assignHandle(fptr);
  ArrayVector retval;
  retval.push_back(Array::uint32Constructor(rethan-1));
  return retval;
}

//!
//@Module FCLOSE File Close Function
//@@Section IO
//@@Usage
//Closes a file handle, or all open file handles.  The general syntax
//for its use is either
//@[
//  fclose(handle)
//@]
//or
//@[
//  fclose('all')
//@]
//In the first case a specific file is closed,  In the second, all open
//files are closed.  Note that until a file is closed the file buffers
//are not flushed.  Returns a '0' if the close was successful and a '-1' if
//the close failed for some reason.
//@@Example
//A simple example of a file being opened with @|fopen| and then closed with @|fclose|.
//@<
//fp = fopen('test.dat','wb','ieee-le')
//fclose(fp)
//@>
//!
ArrayVector FcloseFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Fclose must have one argument, either 'all' or a file handle");
  bool closingAll = false;
  int retval = 0;
  if (arg[0].isString()) {
    string allflag = arg[0].getContentsAsString();
    if (allflag == "all") {
      closingAll = true;
      int maxHandle(fileHandles.maxHandle());
      for (int i=3;i<maxHandle;i++) {
	try {
	  FilePtr* fptr = fileHandles.lookupHandle(i+1);
	  fclose(fptr->fp);
	  fileHandles.deleteHandle(i+1);
	  delete fptr;
	} catch (Exception & e) {
	}
      }
    } else
      throw Exception("Fclose must have one argument, either 'all' or a file handle");
  } else {
    Array tmp(arg[0]);
    int handle = tmp.getContentsAsIntegerScalar();
    if (handle <= 2)
      throw Exception("Cannot close handles 0-2, the standard in/out/error file handles");
    FilePtr* fptr = (fileHandles.lookupHandle(handle+1));
    if (fclose(fptr->fp))
      retval = -1;
    fileHandles.deleteHandle(handle+1);
    delete fptr;
  }
  return SingleArrayVector(Array::int32Constructor(retval));
}

//!
//@Module FREAD File Read Function
//@@Section IO
//@@Usage
//Reads a block of binary data from the given file handle into a variable
//of a given shape and precision.  The general use of the function is
//@[
//  A = fread(handle,size,precision)
//@]
//The @|handle| argument must be a valid value returned by the fopen 
//function, and accessable for reading.  The @|size| argument determines
//the number of values read from the file.  The @|size| argument is simply
//a vector indicating the size of the array @|A|.  The @|size| argument
//can also contain a single @|inf| dimension, indicating that FreeMat should
//calculate the size of the array along that dimension so as to read as
//much data as possible from the file (see the examples listed below for
//more details).  The data is stored as columns in the file, not 
//rows.
//    
//Alternately, you can specify two return values to the @|fread| function,
//in which case the second value contains the number of elements read
//@[
//   [A,count] = fread(...)
//@]
//where @|count| is the number of elements in @|A|.
//
//The third argument determines the type of the data.  Legal values for this
//argument are listed below:
//\begin{itemize}
//   \item 'uint8','uchar','unsigned char' for an unsigned, 8-bit integer.
//   \item 'int8','char','integer*1' for a signed, 8-bit integer.
//   \item 'uint16','unsigned short' for an unsigned, 16-bit  integer.
//   \item 'int16','short','integer*2' for a signed, 16-bit integer.
//   \item 'uint32','unsigned int' for an unsigned, 32-bit integer.
//   \item 'int32','int','integer*4' for a signed, 32-bit integer.
//   \item 'single','float32','float','real*4' for a 32-bit floating point.
//   \item 'double','float64','real*8' for a 64-bit floating point.
//   \item 'complex','complex*8' for a 64-bit complex floating point (32 bits for the real and imaginary part).
//   \item 'dcomplex','complex*16' for a 128-bit complex floating point (64 bits for the real and imaginary part).
//\end{itemize}
//@@Example
//First, we create an array of @|512 x 512| Gaussian-distributed @|float| random variables, and then writing them to a file called @|test.dat|.
//@<
//A = float(randn(512));
//fp = fopen('test.dat','wb');
//fwrite(fp,A);
//fclose(fp);
//@>
//Read as many floats as possible into a row vector
//@<
//fp = fopen('test.dat','rb');
//x = fread(fp,[1,inf],'float');
//who x
//@>
//Read the same floats into a 2-D float array.
//@<
//fp = fopen('test.dat','rb');
//x = fread(fp,[512,inf],'float');
//who x
//@>
//!
ArrayVector FreadFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3)
    throw Exception("fread requires three arguments, the file handle, size, and precision");
  Array tmp(arg[0]);
  int handle = tmp.getContentsAsIntegerScalar();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  if (!arg[2].isString())
    throw Exception("second argument to fread must be a precision");
  string prec = arg[2].getContentsAsString();
  // Get the size argument
  Array sze(arg[1]);
  // Promote sze to a float argument
  sze.promoteType(FM_FLOAT);
  // Check for a single infinity
  int dimCount(sze.getLength());
  float *dp = ((float *) sze.getReadWriteDataPointer());
  bool infinityFound = false;
  int elementCount = 1;
  int infiniteDim = 0;
  for (int i=0;i<dimCount;i++) {
    if (IsNaN(dp[i])) throw Exception("nan not allowed in size argument");
    if (IsInfinite(dp[i])) {
      if (infinityFound) throw Exception("only a single inf is allowed in size argument");
      infinityFound = true;
      infiniteDim = i;
    } else {
      if (dp[i] < 0) throw Exception("illegal negative size argument");
      elementCount *= (int) dp[i];
    }
  }
  // Map the precision string to a data class
  Class dataClass;
  int elementSize;
  int swapSize;
  processPrecisionString(prec,dataClass,elementSize,swapSize);
  // If there is an infinity in the dimensions, we have to calculate the
  // appropriate value
  if (infinityFound) {
    long fsize;
    long fcpos;
    fcpos = ftell(fptr->fp);
    fseek(fptr->fp,0L,SEEK_END);
    fsize = ftell(fptr->fp) - fcpos;
    fseek(fptr->fp,fcpos,SEEK_SET);
    dp[infiniteDim] = ceil((double)(fsize/elementSize/elementCount));
    elementCount *= (int) dp[infiniteDim];
  }
  // Next, we allocate space for the result
  void *qp = Malloc(elementCount*elementSize);
  // Read in the requested number of data points...
  int g = fread(qp,elementSize,elementCount,fptr->fp);
  if (g != elementCount) {
    for (int i=0;i<dimCount;i++)
      dp[i] = 1;
    dp[0] = g;
    elementCount = g;
  }
  if (ferror(fptr->fp)) 
    throw Exception(strerror(errno));
  if (fptr->swapflag)
    SwapBuffer((char *)qp,elementCount*elementSize/swapSize,swapSize);
  // Convert dp to a Dimensions
  Dimensions dims(dimCount);
  for (int j=0;j<dimCount;j++)
    dims.set(j,(int) dp[j]);
  if (dimCount == 1)
    dims.set(1,1);
  ArrayVector retval;
  retval.push_back(Array::Array(dataClass,dims,qp));
  if (nargout == 2)
    retval.push_back(Array::uint32Constructor(retval[0].getLength()));
  return retval;
}

//!
//@Module FWRITE File Write Function
//@@Section IO
//@@Usage
//Writes an array to a given file handle as a block of binary (raw) data.
//The general use of the function is
//@[
//  n = fwrite(handle,A)
//@]
//The @|handle| argument must be a valid value returned by the fopen 
//function, and accessable for writing. The array @|A| is written to
//the file a column at a time.  The form of the output data depends
//on (and is inferred from) the precision of the array @|A|.  If the 
//write fails (because we ran out of disk space, etc.) then an error
//is returned.  The output @|n| indicates the number of elements
//successfully written.
//@@Example
//Heres an example of writing an array of @|512 x 512| Gaussian-distributed @|float| random variables, and then writing them to a file called @|test.dat|.
//@<
//A = float(randn(512));
//fp = fopen('test.dat','wb');
//fwrite(fp,A);
//fclose(fp);
//@>
//!
ArrayVector FwriteFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("fwrite requires two arguments, the file handle, and the variable to be written");
  Array tmp(arg[0]);
  int handle = tmp.getContentsAsIntegerScalar();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  if (arg[1].isReferenceType())
    throw Exception("cannot write reference data types with fwrite");
  Array toWrite(arg[1]);
  unsigned int written;
  unsigned int count(toWrite.getLength());
  unsigned int elsize(toWrite.getElementSize());
  if (!fptr->swapflag || (elsize == 1)) {
    const void *dp=(toWrite.getDataPointer());
    written = fwrite(dp,elsize,count,fptr->fp);      
  } else {
    void *dp=(toWrite.getReadWriteDataPointer());
    SwapBuffer((char*) dp, count, elsize);
    written = fwrite(dp,elsize,count,fptr->fp);
  }
  ArrayVector retval;
  retval.push_back(Array::uint32Constructor(written));
  return retval;    
}

//!
//@Module FFLUSH Force File Flush
//@@Section IO
//@@Usage
//Flushes any pending output to a given file.  The general use of
//this function is
//@[
//   fflush(handle)
//@]
//where @|handle| is an active file handle (as returned by @|fopen|).
//!
 ArrayVector FflushFunction(int nargout, const ArrayVector& arg) {
   if (arg.size() != 1)
     throw Exception("fflush requires an argument, the file handle.");
   int handle = ArrayToInt32(arg[0]);
   FilePtr *fptr = (fileHandles.lookupHandle(handle+1));
   fflush(fptr->fp);
   return ArrayVector();
 }

//!
//@Module FTELL File Position Function
//@@Section IO
//@@Usage
//Returns the current file position for a valid file handle.
//The general use of this function is
//@[
//  n = ftell(handle)
//@]
//The @|handle| argument must be a valid and active file handle.  The
//return is the offset into the file relative to the start of the
//file (in bytes).
//@@Example
//Here is an example of using @|ftell| to determine the current file 
//position.  We read 512 4-byte floats, which results in the file 
//pointer being at position 512*4 = 2048.
//@<
//fp = fopen('test.dat','wb');
//fwrite(fp,randn(512,1));
//fclose(fp);
//fp = fopen('test.dat','rb');
//x = fread(fp,[512,1],'float');
//ftell(fp)
//@>
//!
ArrayVector FtellFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("ftell requires an argument, the file handle.");
  Array tmp(arg[0]);
  int handle = tmp.getContentsAsIntegerScalar();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  unsigned int fpos;
  fpos = ftell(fptr->fp);
  ArrayVector retval;
  retval.push_back(Array::uint32Constructor(fpos));
  return retval;
}

//!
//@Module FEOF End Of File Function
//@@Section IO
//@@Usage
//Check to see if we are at the end of the file.  The usage is
//@[
//  b = feof(handle)
//@]
//The @|handle| argument must be a valid and active file handle.  The
//return is true (logical 1) if the current position is at the end of
//the file, and false (logical 0) otherwise.  Note that simply reading
//to the end of a file will not cause @|feof| to return @|true|.  
//You must read past the end of the file (which will cause an error 
//anyway).  See the example for more details.
//@@Example
//Here, we read to the end of the file to demonstrate how @|feof| works.
//At first pass, we force a read of the contents of the file by specifying
//@|inf| for the dimension of the array to read.  We then test the
//end of file, and somewhat counter-intuitively, the answer is @|false|.
//We then attempt to read past the end of the file, which causes an
//error.  An @|feof| test now returns the expected value of @|true|.
//@<1
//fp = fopen('test.dat','rb');
//x = fread(fp,[512,inf],'float');
//feof(fp)
//x = fread(fp,[1,1],'float');
//feof(fp)
//@>
//!
ArrayVector FeofFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("feof requires an argument, the file handle.");
  Array tmp(arg[0]);
  int handle = tmp.getContentsAsIntegerScalar();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  int ateof;
  ateof = feof(fptr->fp);
  ArrayVector retval;
  retval.push_back(Array::logicalConstructor(ateof));
  return retval;
}
  
//!
//@Module FSEEK Seek File To A Given Position
//@@Section IO
//@@Usage
//Moves the file pointer associated with the given file handle to 
//the specified offset (in bytes).  The usage is
//@[
//  fseek(handle,offset,style)
//@]
//The @|handle| argument must be a value and active file handle.  The
//@|offset| parameter indicates the desired seek offset (how much the
//file pointer is moved in bytes).  The @|style| parameter determines
//how the offset is treated.  Three values for the @|style| parameter
//are understood:
//\begin{itemize}
//\item string @|'bof'| or the value -1, which indicate the seek is relative
//to the beginning of the file.  This is equivalent to @|SEEK_SET| in
//ANSI C.
//\item string @|'cof'| or the value 0, which indicates the seek is relative
//to the current position of the file.  This is equivalent to 
//@|SEEK_CUR| in ANSI C.
//\item string @|'eof'| or the value 1, which indicates the seek is relative
//to the end of the file.  This is equivalent to @|SEEK_END| in ANSI
//C.
//\end{itemize}
//The offset can be positive or negative.
//@@Example
//The first example reads a file and then ``rewinds'' the file pointer by seeking to the beginning.
//The next example seeks forward by 2048 bytes from the files current position, and then reads a line of 512 floats.
//@<
//% First we create the file
//fp = fopen('test.dat','wb');
//fwrite(fp,float(rand(4096,1)));
//fclose(fp);
//% Now we open it
//fp = fopen('test.dat','rb');
//% Read the whole thing
//x = fread(fp,[1,inf],'float');
//% Rewind to the beginning
//fseek(fp,0,'bof');
//% Read part of the file
//y = fread(fp,[1,1024],'float');
//who x y
//% Seek 2048 bytes into the file
//fseek(fp,2048,'cof');
//% Read 512 floats from the file
//x = fread(fp,[512,1],'float');
//% Close the file
//fclose(fp);
//@>
//!
ArrayVector FseekFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("fseek requires three arguments, the file handle, the offset, and style");
  Array tmp1(arg[0]);
  int handle = tmp1.getContentsAsIntegerScalar();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  Array tmp2(arg[1]);
  int offset = tmp2.getContentsAsIntegerScalar();
  Array tmp3(arg[2]);
  int style;
  if (tmp3.isString()) {
    string styleflag = arg[2].getContentsAsString();
    if (styleflag=="bof" || styleflag=="BOF")
      style = -1;
    else if (styleflag=="cof" || styleflag=="COF")
      style = 0;
    else if (styleflag=="eof" || styleflag=="EOF")
      style = 1;
    else
      throw Exception("unrecognized style format for fseek");
  } else {
    style = tmp3.getContentsAsIntegerScalar();
    if ((style != -1) && (style != 0) && (style != 1))
      throw Exception("unrecognized style format for fseek");	
  }
  switch (style) {
  case -1:
    if (fseek(fptr->fp,offset,SEEK_SET))
      throw Exception(strerror(errno));
    break;
  case 0:
    if (fseek(fptr->fp,offset,SEEK_CUR))
      throw Exception(strerror(errno));
    break;
  case 1:
    if (fseek(fptr->fp,offset,SEEK_END))
      throw Exception(strerror(errno));
    break;
  }
  return ArrayVector();
}

int flagChar(char c) {
  return ((c == '#') ||  (c == '0') || (c == '-') ||  
	  (c == ' ') ||  (c == '+'));
}
  
int convspec(char c) {
  return ((c == 'd') || (c == 'i') || (c == 'o') || 
	  (c == 'u') || (c == 'x') || (c == 'X') ||
	  (c == 'e') || (c == 'E') || (c == 'f') || 
	  (c == 'F') || (c == 'g') || (c == 'G') ||
	  (c == 'a') || (c == 'A') || (c == 'c') || 
	  (c == 's'));
}
    
char* validateFormatSpec(char* cp) {
  if (*cp == '%') return cp+1;
  while ((*cp) && flagChar(*cp)) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  while ((*cp) && (*cp == '.')) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  if ((*cp) && convspec(*cp)) 
    return cp+1;
  else
    return 0;
}

char* validateScanFormatSpec(char* cp) {
  if (*cp == '%') return cp+1;
  while ((*cp) && flagChar(*cp)) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  while ((*cp) && (*cp == '.')) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  if ((*cp) && (convspec(*cp) || (*cp == 'h') || (*cp == 'l')))
    return cp+1;
  else
    return 0;
}
    
bool isEscape(char *dp) {
  return ((dp[0] == '\\') &&
	  ((dp[1] == 'n') ||
	   (dp[1] == 't') ||
	   (dp[1] == 'r') ||
	   (dp[1] == '\\')));
}
  
void convertEscapeSequences(char *dst, char* src) {
  char *sp;
  char *dp;
  sp = src;
  dp = dst;
  while (*sp) {
    // Is this an escape sequence?
    if (isEscape(sp)) {
      switch (sp[1]) {
      case '\\':
	*(dp++) = '\\';
	break;
      case 'n':
	*(dp++) = '\n';
	break;
      case 't':
	*(dp++) = '\t';
	break;
      case 'r':
	*(dp++) = '\r';
	break;
      }
      sp += 2;
    } else
      *(dp++) = *(sp++);
  }
  // Null terminate
  *dp = 0;
}

  
//Common routine used by sprintf,printf,fprintf.  They all
//take the same inputs, and output either to a string, the
//console or a file.  For output to a console or a file, 
//we want escape-translation on.  For output to a string, we
//want escape-translation off.  So this common routine prints
//the contents to a string, which is then processed by each 
//subroutine.
char* xprintfFunction(int nargout, const ArrayVector& arg) {
  Array format(arg[0]);
  string frmt = format.getContentsAsString();
  char *buff = strdup(frmt.c_str());
  // Search for the start of a format subspec
  char *dp = buff;
  char *np;
  char sv;
  // Buffer to hold each sprintf result
#define BUFSIZE 65536
  char nbuff[BUFSIZE];
  // Buffer to hold the output
  char *op;
  op = (char*) malloc(sizeof(char));
  *op = 0;
  int nextArg = 1;
  // Scan the string
  while (*dp) {
    np = dp;
    while ((*dp) && (*dp != '%')) dp++;
    // Print out the formatless segment
    sv = *dp;
    *dp = 0;
    snprintf(nbuff,BUFSIZE,np);
    op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
    strcat(op,nbuff);
    *dp = sv;
    // Process the format spec
    if (*dp) {
      np = validateFormatSpec(dp+1);
      if (!np)
	throw Exception("erroneous format specification " + std::string(dp));
      else {
	if (*(np-1) == '%') {
	  snprintf(nbuff,BUFSIZE,"%%");
	  op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
	  strcat(op,nbuff);
	  dp+=2;
	} else {
	  sv = *np;
	  *np = 0;
	  if (arg.size() <= nextArg)
	    throw Exception("not enough arguments to satisfy format specification");
	  Array nextVal(arg[nextArg++]);
	  if ((*(np-1) != 's') && (nextVal.isEmpty())) {
	    op = (char*) realloc(op,strlen(op)+strlen("[]")+1);
	    strcat(op,"[]");
	  } else {
	    switch (*(np-1)) {
	    case 'd':
	    case 'i':
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X':
	    case 'c':
	      nextVal.promoteType(FM_INT32);
	      snprintf(nbuff,BUFSIZE,dp,*((int32*)nextVal.getDataPointer()));
	      op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
	      strcat(op,nbuff);
	      break;
	    case 'e':
	    case 'E':
	    case 'f':
	    case 'F':
	    case 'g':
	    case 'G':
	      nextVal.promoteType(FM_DOUBLE);
	      snprintf(nbuff,BUFSIZE,dp,*((double*)nextVal.getDataPointer()));
	      op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
	      strcat(op,nbuff);
	      break;
	    case 's':
	      snprintf(nbuff,BUFSIZE,dp,nextVal.getContentsAsString().c_str());
	      op = (char*) realloc(op,strlen(op)+strlen(nbuff)+1);
	      strcat(op,nbuff);
	    }
	  }
	  *np = sv;
	  dp = np;
	}
      }
    }
  }
  free(buff);
  return op;
}

//!
//@Module NUM2STR Convert Numbers To Strings
//@@Section ARRAY
//@@Usage
//Converts an array into its string representation.  The general syntax
//for this function is
//@[
//   s = num2str(X)
//@]
//where @|s| is a string (or string matrix) and @|X| is an array.  By
//default, the @|num2str| function uses 4 digits of precision and an 
//exponent if required.  If you want more digits of precision, you can 
//specify the precition via the form
//@[
//   s = num2str(X, precision)
//@]
//where @|precision| is the number of digits to include in the string
//representation.  For more control over the format of the output, you 
//can also specify a format specifier (see @|printf| for more details).
//@[
//   s = num2str(X, format)
//@]
//where @|format| is the specifier string.
//!
template <class T>
Array Num2StrHelperReal(const T*dp, Dimensions Xdims, const char *formatspec) {
  int rows(Xdims.getRows());
  int cols(Xdims.getColumns());
  StringVector row_string;
  if (Xdims.getLength() == 2)
    Xdims.set(3,1);
  Dimensions Wdims(Xdims.getLength());
  int offset = 0;
  while (Wdims.inside(Xdims)) {
    for (int i=0;i<rows;i++) {
      string colbuffer;
      for (int j=0;j<cols;j++) {
	char elbuffer[1024];
	sprintf(elbuffer,formatspec,dp[i+j*rows+offset]);
	char elbuffer2[1024];
	convertEscapeSequences(elbuffer2,elbuffer);
	colbuffer += string(elbuffer2) + " ";
      }
      row_string.push_back(colbuffer);
    }
    offset += rows*cols;
    Wdims.incrementModulo(Xdims,2);
  }
  // Next we compute the length of the largest string
  int maxlen = 0;
  for (int n=0;n<(int)row_string.size();n++)
    if ((int)row_string[n].size() > maxlen)
      maxlen = row_string[n].size();
  // Next we allocate a character array large enough to
  // hold the string array.
  char *sp = (char*) Array::allocateArray(FM_STRING,maxlen*row_string.size());
  // Now we copy 
  int slices = row_string.size() / rows;
  for (int i=0;i<slices;i++)
    for (int j=0;j<rows;j++) {
      string line(row_string[j+i*rows]);
      for (int k=0;k<(int)line.size();k++)
	sp[j+i*rows*maxlen+k*rows] = line[k];
    }
  Dimensions odims(Xdims);
  odims.set(1,maxlen);
  odims.simplify();
  return Array(FM_STRING,odims,sp);
}

template <class T>
Array Num2StrHelperComplex(const T*dp, Dimensions Xdims, const char *formatspec) {
  int rows(Xdims.getRows());
  int cols(Xdims.getColumns());
  StringVector row_string;
  if (Xdims.getLength() == 2)
    Xdims.set(3,1);
  Dimensions Wdims(Xdims.getLength());
  int offset = 0;
  while (Wdims.inside(Xdims)) {
    for (int i=0;i<rows;i++) {
      string colbuffer;
      for (int j=0;j<cols;j++) {
	char elbuffer[1024];
	char elbuffer2[1024];
	sprintf(elbuffer,formatspec,dp[2*(i+j*rows+offset)]);
	convertEscapeSequences(elbuffer2,elbuffer);
	colbuffer += string(elbuffer2) + " ";
	sprintf(elbuffer,formatspec,dp[2*(i+j*rows+offset)+1]);
	convertEscapeSequences(elbuffer2,elbuffer);
	if (dp[2*(i+j*rows+offset)+1]>=0) 
	  colbuffer += "+";
	colbuffer += string(elbuffer2) + "i ";
      }
      row_string.push_back(colbuffer);
    }
    offset += rows*cols;
    Wdims.incrementModulo(Xdims,2);
  }
  // Next we compute the length of the largest string
  int maxlen = 0;
  for (int n=0;n<(int)row_string.size();n++)
    if ((int)row_string[n].size() > maxlen)
      maxlen = row_string[n].size();
  // Next we allocate a character array large enough to
  // hold the string array.
  char *sp = (char*) Array::allocateArray(FM_STRING,maxlen*row_string.size());
  // Now we copy 
  int slices = row_string.size() / rows;
  for (int i=0;i<slices;i++)
    for (int j=0;j<rows;j++) {
      string line(row_string[j+i*rows]);
      for (int k=0;k<(int)line.size();k++)
	sp[j+i*rows*maxlen+k*rows] = line[k];
    }
  Dimensions odims(Xdims);
  odims.set(1,maxlen);
  odims.simplify();
  return Array(FM_STRING,odims,sp);
}


ArrayVector Num2StrFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("num2str function requires at least one argument");
  Array X(arg[0]);
  if (X.isReferenceType())
    throw Exception("num2str function requires a numeric input");
  char formatspec[1024];
  if (X.isIntegerClass())
    sprintf(formatspec,"%%d");
  else
    sprintf(formatspec,"%%11.4g");
  if (arg.size() > 1) {
    Array format(arg[1]);
    if (format.isString())
      strcpy(formatspec,ArrayToString(format).c_str());
  }
  switch (X.dataClass()) 
    {
    default: throw Exception("illegal type argument to num2str");
    case FM_UINT8:
      return ArrayVector() << Num2StrHelperReal((const uint8*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_INT8:
      return ArrayVector() << Num2StrHelperReal((const int8*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_UINT16:
      return ArrayVector() << Num2StrHelperReal((const uint16*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_INT16:
      return ArrayVector() << Num2StrHelperReal((const int16*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_UINT32:
      return ArrayVector() << Num2StrHelperReal((const uint32*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_INT32:
      return ArrayVector() << Num2StrHelperReal((const int32*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_UINT64:
      return ArrayVector() << Num2StrHelperReal((const uint64*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_INT64:
      return ArrayVector() << Num2StrHelperReal((const int64*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_FLOAT:
      return ArrayVector() << Num2StrHelperReal((const float*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_DOUBLE:
      return ArrayVector() << Num2StrHelperReal((const double*) X.getDataPointer(),
						X.dimensions(),formatspec);
    case FM_COMPLEX:
      return ArrayVector() << Num2StrHelperComplex((const float*) X.getDataPointer(),
						   X.dimensions(),formatspec);
    case FM_DCOMPLEX:
      return ArrayVector() << Num2StrHelperComplex((const double*) X.getDataPointer(),
						   X.dimensions(),formatspec);
    case FM_STRING:
      throw Exception("argument to num2str must be numeric type");
    }
  return ArrayVector();
}

//!
//@Module SPRINTF Formated String Output Function (C-Style)
//@@Section IO
//@@Usage
//Prints values to a string.  The general syntax for its use is
//@[
//  y = sprintf(format,a1,a2,...).
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the output.  The values of the variables
//@|a_i| are substituted into the output as required.  It is
//an error if there are not enough variables to satisfy the format
//string.  Note that this @|sprintf| command is not vectorized!  Each
//variable must be a scalar.  The returned value @|y| contains the
//string that would normally have been printed. For
//more details on the format string, see @|printf|.  
//@@Examples
//Here is an example of a loop that generates a sequence of files based on
//a template name, and stores them in a cell array.
//@<
//l = {}; for i = 1:5; s = sprintf('file_%d.dat',i); l(i) = {s}; end;
//l
//@>
//!
ArrayVector SprintfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("sprintf requires at least one (string) argument");
  Array format(arg[0]);
  if (!format.isString())
    throw Exception("sprintf format argument must be a string");
  char *op = xprintfFunction(nargout,arg);
  char *buff = (char*) malloc(strlen(op)+1);
  convertEscapeSequences(buff,op);
  Array outString(Array::stringConstructor(buff));
  free(op);
  free(buff);
  return SingleArrayVector(outString);
}
  
//!
//@Module PRINTF Formated Output Function (C-Style)
//@@Section IO
//@@Usage
//Prints values to the output.  The general syntax for its use is
//@[
//  printf(format,a1,a2,...)
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the output.  The values of the variables
//@|a_i| are substituted into the output as required.  It is
//an error if there are not enough variables to satisfy the format
//string.  Note that this @|printf| command is not vectorized!  Each
//variable must be a scalar.
//
//It is important to point out that the @|printf| function does not
//add a newline (or carriage return) to the output by default.  That
//can lead to some confusing behavior if you do not know what to expect.
//For example, the command @|printf('Hello')| does not appear to
//produce any output.  In fact, it does produce the text, but it then
//gets overwritten by the prompt.  To see the text, you need 
//@|printf('Hello\n')|.  This seems odd, but allows you to assemble a
//line using multiple @|printf| commands, including the @|'\n'| when
//you are done with the line.  You can also use the @|'\r'| character
//as an explicit carriage return (with no line feed).  This allows you
//to write to the same line many times (to show a progress string, for
//example).
//
//@@Format of the format string:
//
//The  format  string  is a character string, beginning and ending in its
//initial shift state, if any.  The format string is composed of zero  or
//more   directives:  ordinary  characters  (not  %),  which  are  copied
//unchanged to the output stream; and conversion specifications, each  of
//which results in fetching zero or more subsequent arguments.  Each 
//conversion specification is introduced by the character %, and ends with a
//conversion  specifier.  In between there may be (in this order) zero or
//more flags, an optional minimum field width, and an optional precision.
//
//The  arguments must correspond properly (after type promotion) with the
//conversion specifier, and are used in the order given.
//
//@@The flag characters:
//The character @|%| is followed by zero or more of the following flags:
//\begin{itemize}
//  \item @|\#|   The  value  should be converted to an ``alternate form''.  For @|o| conversions, the first character of the output  string  is  made  zero (by prefixing a @|0| if it was not zero already).  For @|x| and @|X| conversions, a nonzero result has the string @|'0x'| (or @|'0X'| for  @|X|  conversions) prepended to it.  For @|a, A, e, E, f, F, g,| and @|G|  conversions, the result will always  contain  a  decimal  point,  even  if  no digits follow it (normally, a decimal point appears  in the results of those conversions only if  a  digit  follows).  For @|g| and @|G| conversions, trailing zeros are not removed from the  result as they would otherwise be.  For other  conversions,  the  result is undefined.
//  \item @|0|   The value should be zero padded.  For @|d, i, o, u, x, X, a, A, e, E, f, F, g,| and @|G| conversions, the converted value is padded  on the  left  with  zeros rather than blanks.  If the @|0| and @|-| flags  both appear, the @|0| flag is ignored.  If  a  precision  is  given  with  a numeric conversion @|(d, i, o, u, x, and X)|, the @|0| flag is  ignored.  For other conversions, the behavior is undefined.
//  \item @|-|   The converted value is to be left adjusted on the  field  boundary.  (The default is right justification.) Except for @|n| conversions, the converted value is padded on the right  with  blanks, rather than on the left with blanks or zeros.  A @|-| overrides a @|0| if both are given.
//  \item @|' '| (a space) A blank should be left before a  positive  number  (or empty string) produced by a signed conversion.
//  \item @|+| A  sign  (@|+| or @|-|) always be placed before a number produced by a signed conversion.  By default a sign is used only for  negative numbers. A @|+| overrides a space if both are used.
//\end{itemize}
//@@The field width:
//An  optional decimal digit string (with nonzero first digit) specifying a 
//minimum field width.  If the converted  value  has  fewer  characters than 
//the  field  width,  it will be padded with spaces on the left (or right, 
//if the left-adjustment flag has been given).  A  negative  field width is 
//taken as a @|'-'| flag followed by a positive field  width. In no case does 
//a non-existent or small field width cause truncation of a field; if the 
//result of a conversion is wider than the  field  width, the field is 
//expanded to contain the conversion result.
//
//@@The precision:
//
//An  optional  precision,  in the form of a period (@|'.'|)  followed by an optional decimal digit string.  If the precision is given as just @|'.'|, or the precision is negative, the precision is  taken  to  be zero.   This  gives the minimum number of digits to appear for @|d, i, o, u, x|, and @|X| conversions, the number of digits to appear after the radix character  for  @|a, A, e, E, f|, and @|F| conversions, the maximum number of significant digits for @|g| and @|G| conversions, or the  maximum  number  of  characters to be printed from a string for s conversions.
//
//@@The conversion specifier:
//
//A character that specifies the type of conversion to be  applied.   The
//conversion specifiers and their meanings are:
//\begin{itemize}
//\item @|d,i|   The  int  argument is converted to signed decimal notation.  The  precision, if any, gives the minimum number of digits that  must   appear;  if  the  converted  value  requires fewer digits, it is    padded on the left with zeros. The default precision is @|1|.  When @|0|  is printed with an explicit precision @|0|, the output is empty.
//\item @|o,u,x,X|   The unsigned int argument is converted to  unsigned  octal  (@|o|),  unsigned  decimal  (@|u|),  or unsigned hexadecimal (@|x| and @|X|) notation.  The letters @|abcdef| are used for @|x| conversions;  the  letters @|ABCDEF| are used for @|X| conversions.  The precision, if any,  gives the minimum number of digits that must appear; if the converted  value  requires  fewer  digits, it is padded on the left  with zeros. The default precision is @|1|.  When @|0| is printed  with  an explicit precision @|0|, the output is empty.
//\item @|e,E|    The  double  argument  is  rounded  and  converted  in the style  @|[-]d.ddde dd| where there is one digit before  the  decimal-point  character and the number of digits after it is equal to the precision; if the precision is missing, it is taken as  @|6|;  if  the    precision  is  zero,  no  decimal-point character appears.  An @|E|  conversion uses the letter @|E| (rather than @|e|)  to  introduce  the  exponent.   The exponent always contains at least two digits; if  the value is zero, the exponent is @|00|.
//\item @|f,F|   The double argument is rounded and converted to decimal notation  in  the  style  @|[-]ddd.ddd|, where the number of digits after the decimal-point character is equal to the precision specification.  If  the precision is missing, it is taken as @|6|; if the precision  is explicitly zero, no decimal-point character  appears.   If  a   decimal point appears, at least one digit appears before it.
//\item @|g,G|   The double argument is converted in style @|f| or @|e| (or @|F| or @|E|  for  @|G|  conversions).  The precision specifies the number of significant digits.  If the precision is missing, @|6| digits  are  given;  if  the  precision is zero, it is treated as @|1|.  Style e is used   if the exponent from its conversion is less than @|-4|  or  greater than or equal to the precision.  Trailing zeros are removed from  the fractional part of the result; a decimal point appears  only  if it is followed by at least one digit.
//\item @|c| The int argument is  converted  to  an  unsigned  char, and  the resulting character is written.
//\item @|s| The string argument is printed.
//\item @|%|   A @|'%'| is written. No argument is converted. The complete conversion specification is @|'%%'|.
//\end{itemize}
//@@Example
//Here are some examples of the use of @|printf| with various arguments.  First we print out an integer and double value.
//@<
//printf('intvalue is %d, floatvalue is %f\n',3,1.53);
//@>
//Next, we print out a string value.
//@<
//printf('string value is %s\n','hello');
//@>
//Now, we print out an integer using 12 digits, zeros up front.
//@<
//printf('integer padded is %012d\n',32);
//@>
//Print out a double precision value with a sign, a total of 18 characters (zero prepended if necessary), a decimal point, and 12 digit precision.
//@<
//printf('float value is %+018.12f\n',pi);
//@>
//!
ArrayVector PrintfFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("printf requires at least one (string) argument");
  Array format(arg[0]);
  if (!format.isString())
    throw Exception("printf format argument must be a string");
  char *op = xprintfFunction(nargout,arg);
  char *buff = (char*) malloc(strlen(op)+1);
  convertEscapeSequences(buff,op);
  eval->outputMessage(buff);
  free(buff);
  free(op);
  return ArrayVector();
}

//!
//@Module FGETLINE Read a String from a File
//@@Section IO
//@@Usage
//Reads a string from a file.  The general syntax for its use
//is
//@[
//  s = fgetline(handle)
//@]
//This function reads characters from the file @|handle| into
//a @|string| array @|s| until it encounters the end of the file
//or a newline.  The newline, if any, is retained in the output
//string.  If the file is at its end, (i.e., that @|feof| would
//return true on this handle), @|fgetline| returns an empty
//string.
//@@Example
//First we write a couple of strings to a test file.
//@<
//fp = fopen('testtext','w');
//fprintf(fp,'String 1\n');
//fprintf(fp,'String 2\n');
//fclose(fp);
//@>
//Next, we read then back.
//@<
//fp = fopen('testtext','r')
//fgetline(fp)
//fgetline(fp)
//fclose(fp);
//@>
//!
ArrayVector FgetlineFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("fgetline takes one argument, the file handle");
  Array tmp(arg[0]);
  int handle = tmp.getContentsAsIntegerScalar();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  char buffer[65535];
  fgets(buffer,sizeof(buffer),fptr->fp);
  if (feof(fptr->fp))
    return SingleArrayVector(Array::emptyConstructor());
  return SingleArrayVector(Array::stringConstructor(buffer));
}

//!
//@Module STR2NUM Convert a String to a Number
//@@Section IO
//@@Usage
//Converts a string to a number.  The general syntax for its use
//is
//@[
//  x = str2num(string)
//@]
//Here @|string| is the data string, which contains the data to 
//be converted into a number.  The output is in double precision,
//and must be typecasted to the appropriate type based on what
//you need.
//!
ArrayVector Str2NumFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("str2num takes a single argument, the string to convert into a number");
  Array data(arg[0]);
  if (!data.isString())
    throw Exception("the first argument to str2num must be a string");
  return ArrayVector() << Array::doubleConstructor(ArrayToDouble(data));
}

// How does sscanf work...
// a whitespace matches any number/type of whitespace
// a non-whitespace must be matched exactly
// a %d, %ld, etc.  reads a number... 
// a %s matches a sequence of characters that does
// not contain whitespaces...
// a mismatch - 

//!
//@Module SSCANF Formated String Input Function (C-Style)
//@@Section IO
//@@Usage
//Reads values from a string.  The general syntax for its use is
//@[
//  [a1,...,an] = sscanf(text,format)
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the input.  Each value that is parsed
//from the @|text| occupies one output slot.  See @|printf|
//for a description of the format.
//!

class AutoFileCloser {
  FILE *fp;
public:
  AutoFileCloser(FILE *g) {fp = g;}
  ~AutoFileCloser() {fclose(fp);}
};

ArrayVector SscanfFunction(int nargout, const ArrayVector& arg) {
  if ((arg.size() != 2) || (!arg[0].isString()) || (!arg[1].isString()))
    throw Exception("sscanf takes two arguments, the text and the format string");
  Array text(arg[0]);
  Array format(arg[1]);
  string txt = text.getContentsAsString();
  FILE *fp = tmpfile();
  AutoFileCloser afc(fp);
  if (!fp)
    throw Exception("sscanf was unable to open a temp file (and so it won't work)");
  fprintf(fp,"%s",txt.c_str());
  rewind(fp);
  if (feof(fp))
    return SingleArrayVector(Array::emptyConstructor());
  string frmt = format.getContentsAsString();
  char *buff = strdup(frmt.c_str());
  // Search for the start of a format subspec
  char *dp = buff;
  char *np;
  char sv;
  bool shortarg;
  bool doublearg;
  // Scan the string
  ArrayVector values;
  while (*dp) {
    np = dp;
    while ((*dp) && (*dp != '%')) dp++;
    // Print out the formatless segment
    sv = *dp;
    *dp = 0;
    fscanf(fp,np);
    if (feof(fp))
      values.push_back(Array::emptyConstructor());
    *dp = sv;
    // Process the format spec
    if (*dp) {
      np = validateScanFormatSpec(dp+1);
      if (!np)
	throw Exception("erroneous format specification " + std::string(dp));
      else {
	if (*(np-1) == '%') {
	  fscanf(fp,"%%");
	  dp+=2;
	} else {
	  shortarg = false;
	  doublearg = false;
	  if (*(np-1) == 'h') {
	    shortarg = true;
	    np++;
	  } else if (*(np-1) == 'l') {
	    doublearg = true;
	    np++;
	  } 
	  sv = *np;
	  *np = 0;
	  switch (*(np-1)) {
	  case 'd':
	  case 'i':
	    if (shortarg) {
	      short sdumint;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&sdumint);
		values.push_back(Array::int16Constructor(sdumint));
	      }
	    } else {
	      int sdumint;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&sdumint);
		values.push_back(Array::int32Constructor(sdumint));
	      }
	    }
	    break;
	  case 'o':
	  case 'u':
	  case 'x':
	  case 'X':
	  case 'c':
	    if (shortarg) {
	      int sdumint;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&sdumint);
		values.push_back(Array::int32Constructor(sdumint));
	      }
	    } else {
	      unsigned int dumint;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&dumint);
		values.push_back(Array::uint32Constructor(dumint));
	      }
	    }
	    break;
	  case 'e':
	  case 'E':
	  case 'f':
	  case 'F':
	  case 'g':
	  case 'G':
	    if (doublearg) {
	      double dumfloat;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&dumfloat);
		values.push_back(Array::doubleConstructor(dumfloat));
	      }
	    } else {
	      float dumfloat;
	      if (feof(fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fp,dp,&dumfloat);
		values.push_back(Array::floatConstructor(dumfloat));
	      }
	    }
	    break;
	  case 's':
	    char stbuff[4096];
	    if (feof(fp))
	      values.push_back(Array::emptyConstructor());
	    else {
	      fscanf(fp,dp,stbuff);
	      values.push_back(Array::stringConstructor(stbuff));
	    }
	    break;
	  default:
	    throw Exception("unsupported fscanf configuration");
	  }
	  *np = sv;
	  dp = np;
	}
      }
    }
  }
  free(buff);
  return values;
}

//!
//@Module FSCANF Formatted File Input Function (C-Style)
//@@Section IO
//@@Usage
//Reads values from a file.  The general syntax for its use is
//@[
//  [a1,...,an] = fscanf(handle,format)
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the input.  Each value that is parsed from
//the file described by @|handle| occupies one output slot.
//See @|printf| for a description of the format.  Note that if
//the file is at the end-of-file, the fscanf will return 
//!
ArrayVector FscanfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("fscanf takes two arguments, the file handle and the format string");
  Array tmp(arg[0]);
  int handle = tmp.getContentsAsIntegerScalar();
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  Array format(arg[1]);
  if (!format.isString())
    throw Exception("fscanf format argument must be a string");
  if (feof(fptr->fp))
    return SingleArrayVector(Array::emptyConstructor());
  string frmt = format.getContentsAsString();
  char *buff = strdup(frmt.c_str());
  // Search for the start of a format subspec
  char *dp = buff;
  char *np;
  char sv;
  bool shortarg;
  bool doublearg;
  // Scan the string
  ArrayVector values;
  while (*dp) {
    np = dp;
    while ((*dp) && (*dp != '%')) dp++;
    // Print out the formatless segment
    sv = *dp;
    *dp = 0;
    fscanf(fptr->fp,np);
    if (feof(fptr->fp))
      values.push_back(Array::emptyConstructor());
    *dp = sv;
    // Process the format spec
    if (*dp) {
      np = validateScanFormatSpec(dp+1);
      if (!np)
	throw Exception("erroneous format specification " + std::string(dp));
      else {
	if (*(np-1) == '%') {
	  fscanf(fptr->fp,"%%");
	  dp+=2;
	} else {
	  shortarg = false;
	  doublearg = false;
	  if (*(np-1) == 'h') {
	    shortarg = true;
	    np++;
	  } else if (*(np-1) == 'l') {
	    doublearg = true;
	    np++;
	  } 
	  sv = *np;
	  *np = 0;
	  switch (*(np-1)) {
	  case 'd':
	  case 'i':
	    if (shortarg) {
	      short sdumint;
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fptr->fp,dp,&sdumint);
		values.push_back(Array::int16Constructor(sdumint));
	      }
	    } else {
	      int sdumint;
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fptr->fp,dp,&sdumint);
		values.push_back(Array::int32Constructor(sdumint));
	      }
	    }
	    break;
	  case 'o':
	  case 'u':
	  case 'x':
	  case 'X':
	  case 'c':
	    if (shortarg) {
	      int sdumint;
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fptr->fp,dp,&sdumint);
		values.push_back(Array::int32Constructor(sdumint));
	      }
	    } else {
	      unsigned int dumint;
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fptr->fp,dp,&dumint);
		values.push_back(Array::uint32Constructor(dumint));
	      }
	    }
	    break;
	  case 'e':
	  case 'E':
	  case 'f':
	  case 'F':
	  case 'g':
	  case 'G':
	    if (doublearg) {
	      double dumfloat;
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fptr->fp,dp,&dumfloat);
		values.push_back(Array::doubleConstructor(dumfloat));
	      }
	    } else {
	      float dumfloat;
	      if (feof(fptr->fp))
		values.push_back(Array::emptyConstructor());
	      else {
		fscanf(fptr->fp,dp,&dumfloat);
		values.push_back(Array::floatConstructor(dumfloat));
	      }
	    }
	    break;
	  case 's':
	    char stbuff[4096];
	    if (feof(fptr->fp))
	      values.push_back(Array::emptyConstructor());
	    else {
	      fscanf(fptr->fp,dp,stbuff);
	      values.push_back(Array::stringConstructor(stbuff));
	    }
	    break;
	  default:
	    throw Exception("unsupported fscanf configuration");
	  }
	  *np = sv;
	  dp = np;
	}
      }
    }
  }
  free(buff);
  return values;
}

//!
//@Module FPRINTF Formated File Output Function (C-Style)
//@@Section IO
//@@Usage
//Prints values to a file.  The general syntax for its use is
//@[
//  fprintf(fp,format,a1,a2,...).
//@]
//Here @|format| is the format string, which is a string that
//controls the format of the output.  The values of the variables
//@|ai| are substituted into the output as required.  It is
//an error if there are not enough variables to satisfy the format
//string.  Note that this @|fprintf| command is not vectorized!  Each
//variable must be a scalar.  The value @|fp| is the file handle.  For
//more details on the format string, see @|printf|.  Note also 
//that @|fprintf| to the file handle @|1| is effectively equivalent to @|printf|.
//@@Examples
//A number of examples are present in the Examples section of the @|printf| command.
//!
 ArrayVector FprintfFunction(int nargout, const ArrayVector& arg, 
			     Interpreter* eval) {
  if (arg.size() < 2)
    throw Exception("fprintf requires at least two arguments, the file handle and theformat string");
  Array tmp(arg[0]);
  int handle = tmp.getContentsAsIntegerScalar();
  if (handle == 1) {
    ArrayVector argCopy(arg);
    argCopy.pop_front();
    return PrintfFunction(nargout,argCopy,eval);
  }
  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));
  Array format(arg[1]);
  if (!format.isString())
    throw Exception("fprintf format argument must be a string");
  ArrayVector argcopy(arg);
  argcopy.pop_front();
  char *op = xprintfFunction(nargout,argcopy);
  char *buff = (char*) malloc(strlen(op)+1);
  convertEscapeSequences(buff,op);
  fprintf(fptr->fp,"%s",buff);
  free(buff);
  free(op);
  return ArrayVector();
}

  
ArrayVector SaveNativeFunction(string filename, StringVector names, Interpreter* eval) {
  File ofile(filename,"wb");
  Serialize output(&ofile);
  output.handshakeServer();
  Context *cntxt = eval->getContext();
  for (int i=0;i<names.size();i++) {
    ArrayReference toWrite;
    char flags;
    if (!(names[i].compare("ans") == 0)) {
      toWrite = cntxt->lookupVariable(names[i]);
      if (!toWrite.valid())
	throw Exception(std::string("unable to find variable ")+
			names[i]+" to save to file "+filename);
      flags = 'n';
      if (cntxt->isVariableGlobal(names[i]))
	flags = 'g';
      if (cntxt->isVariablePersistent(names[i]))	
	flags = 'p';
      output.putString(names[i].c_str());
      output.putByte(flags);
      output.putArray(*toWrite);
    }
  }
  output.putString("__eof");
  return ArrayVector();
}
  
ArrayVector SaveASCIIFunction(string filename, StringVector names, bool tabsMode,
			      bool doubleMode, Interpreter* eval) {
  FILE *fp = fopen(filename.c_str(),"w");
  if (!fp) throw Exception("unable to open file " + filename + " for writing.");
  Context *cntxt = eval->getContext();
  for (int i=0;i<names.size();i++) {
    if (!(names[i] == "ans")) {
      ArrayReference toWrite = cntxt->lookupVariable(names[i]);
      if (!toWrite.valid())
	throw Exception("unable to find variable " + names[i] + 
			" to save to file "+filename);
      if (toWrite->isReferenceType()) {
	eval->warningMessage("variable " + names[i] + " is not numeric - cannot write it to an ASCII file");
	continue;
      }
      if (!toWrite->is2D()) {
	eval->warningMessage("variable " + names[i] + " is not 2D - cannot write it to an ASCII file");
	continue;
      }
      if (toWrite->isComplex()) 
	eval->warningMessage("variable " + names[i] + " is complex valued - only real part will be written to ASCII file");
      Array A(*toWrite);
      A.promoteType(FM_DOUBLE);
      int rows = A.rows();
      int cols = A.columns();
      double *dp = (double*) A.getDataPointer();
      for (int i=0;i<rows;i++) {
	for (int j=0;j<cols;j++) {
	  if (doubleMode)
	    fprintf(fp,"%.15e",dp[j*rows+i]);
	  else
	    fprintf(fp,"%.7e",dp[j*rows+i]);
	  if (tabsMode && (j < (cols-1)))
	    fprintf(fp,"\t");
	  else
	    fprintf(fp," ");
	}
	fprintf(fp,"\n");
      }
    }
  }
  fclose(fp);
  return ArrayVector();
}

//!
//@Module SAVE Save Variables To A File
//@@Section IO
//@@Usage
//Saves a set of variables to a file in a machine independent format.
//There are two formats for the function call.  The first is the explicit
//form, in which a list of variables are provided to write to the file:
//@[
//  save filename a1 a2 ...
//@]
//In the second form,
//@[
//  save filename
//@]
//all variables in the current context are written to the file.  The 
//format of the file is a simple binary encoding (raw) of the data
//with enough information to restore the variables with the @|load|
//command.  The endianness of the machine is encoded in the file, and
//the resulting file should be portable between machines of similar
//types (in particular, machines that support IEEE floating point 
//representation).
//
//You can also specify both the filename as a string, in which case
//you also have to specify the names of the variables to save.  In
//particular
//@[
//   save('filename','a1','a2')
//@]
//will save variables @|a1| and @|a2| to the file.
//
//Starting with version 2.0, FreeMat can also read and write MAT
//files (the file format used by MATLAB) thanks to substantial 
//work by Thomas Beutlich.  Support for MAT files in version 2.1
//has improved over previous versions.  In particular, classes
//should be saved properly, as well as a broader range of sparse
//matrices.  Compression is supported for both reading and writing
//to MAT files.  MAT file support is still in the alpha stages, so 
//please be cautious with using it to store critical 
//data.  The file format is triggered
//by the extension.  To save files with a MAT format, simply
//use a filename with a ".mat" ending.
//
//The @|save| function also supports ASCII output.  This is a very limited
//form of the save command - it can only save numeric arrays that are
//2-dimensional.  This form of the @|save| command is triggered using
//@[
//   save -ascii filename var1 var 2
//@]
//although where @|-ascii| appears on the command line is arbitrary (provided
//it comes after the @|save| command, of course).  Be default, the @|save|
//command uses an 8-digit exponential format notation to save the values to
//the file.  You can specify that you want 16-digits using the
//@[
//   save -ascii -double filename var1 var2
//@]
//form of the command.  Also, by default, @|save| uses spaces as the 
//delimiters between the entries in the matrix.  If you want tabs instead,
//you can use
//@[
//   save -ascii -tabs filename var1 var2
//@]
//(you can also use both the @|-tabs| and @|-double| flags simultaneously).
//
//Finally, you can specify that @|save| should only save variables that
//match a particular regular expression.  Any of the above forms can be
//combined with the @|-regexp| flag:
//@[
//   save filename -regexp pattern1 pattern2
//@]
//in which case variables that match any of the patterns will be saved.
//@@Example
//Here is a simple example of @|save|/@|load|.  First, we save some 
//variables to a file.
//@< 
//D = {1,5,'hello'};
//s = 'test string';
//x = randn(512,1);
//z = zeros(512);
//who
//save loadsave.dat
//@>
//Next, we clear the variables, and then load them back from the file.
//@<
//clear D s x z
//who
//load loadsave.dat
//who
//@>
//@@Tests
//@{ test_save1.m
//% Test the save and load capability with cell arrays (bug 1581481)
//function test_val = test_save1
//   a{1} = 'bert';
//   save tmp.mat a
//   b = a;
//   load tmp.mat
//   test_val = strcomp(a{1},b{1});
//@}
//!
ArrayVector SaveFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  ArrayVector argCopy;
  if (arg.size() == 0) return ArrayVector();
  bool asciiMode = false;
  bool tabsMode = false;
  bool doubleMode = false;
  bool matMode = false;
  bool regexpMode = false;
  for (int i=0;i<arg.size();i++) {
    if (arg[i].isString()) {
      if (arg[i].getContentsAsStringUpper() == "-MAT")
	matMode = true;
      else if (arg[i].getContentsAsStringUpper() == "-ASCII")
	asciiMode = true;
      else if (arg[i].getContentsAsStringUpper() == "-REGEXP")
	regexpMode = true;
      else if (arg[i].getContentsAsStringUpper() == "-DOUBLE")
	doubleMode = true;
      else if (arg[i].getContentsAsStringUpper() == "-TABS")
	tabsMode = true;
      else
	argCopy << arg[i];
    } else
      argCopy << arg[i];
  }
  if (argCopy.size() < 1) throw Exception("save requires a filename argument");
  string fname(ArrayToString(argCopy[0]));
  if (!asciiMode && !matMode) {
    int len = fname.size();
    if ((len >= 4) && (fname[len-4] == '.') &&
	((fname[len-3] == 'M') || (fname[len-3] == 'm')) &&
	((fname[len-2] == 'A') || (fname[len-2] == 'a')) &&
	((fname[len-1] == 'T') || (fname[len-1] == 't'))) 
      matMode = true;
    if ((len >= 4) && (fname[len-4] == '.') &&
	((fname[len-3] == 'T') || (fname[len-3] == 't')) &&
	((fname[len-2] == 'X') || (fname[len-2] == 'x')) &&
	((fname[len-1] == 'T') || (fname[len-1] == 't'))) 
      matMode = true;    
  }
  StringVector names;
  for (int i=1;i<argCopy.size();i++) {
    if (!arg[i].isString())
      throw Exception("unexpected non-string argument to save command");
    names << ArrayToString(argCopy[i]);
  }
  Context *cntxt = eval->getContext();
  StringVector toSave;
  if (regexpMode || (names.size() == 0)) {
    StringVector allNames = cntxt->listAllVariables();
    for (int i=0;i<(int)allNames.size();i++)
      if ((names.size() == 0) || contains(names,allNames[i],regexpMode))
	toSave << allNames[i];
  } else 
    toSave = names;
  if (matMode)
    return MatSaveFunction(fname,toSave,eval);
  else if (asciiMode)
    return SaveASCIIFunction(fname,toSave,tabsMode,doubleMode,eval);
  else
    return SaveNativeFunction(fname,toSave,eval);
}

//!
//@Module DLMREAD Read ASCII-delimited File
//@@Section IO
//@@Usage
//Loads a matrix from an ASCII-formatted text file with a delimiter
//between the entries.  This function is similar to the @|load -ascii|
//command, except that it can handle complex data, and it allows you
//to specify the delimiter.  Also, you can read only a subset of the
//data from the file.  The general syntax for the @|dlmread| function
//is
//@[
//    y = dlmread(filename)
//@]
//where @|filename| is a string containing the name of the file to read.
//In this form, FreeMat will guess at the type of the delimiter in the 
//file.  The guess is made by examining the input for common delimiter
//characters, which are @|,;:| or a whitespace (e.g., tab).  The text
//in the file is preprocessed to replace these characters with whitespace
//and the file is then read in using a whitespace for the delimiter.
//
//If you know the delimiter in the file, you can specify it using
//this form of the function:
//@[
//    y = dlmread(filename, delimiter)
//@]
//where @|delimiter| is a string containing the delimiter.  If @|delimiter|
//is the empty string, then the delimiter is guessed from the file.
//
//You can also read only a portion of the file by specifying a start row
//and start column:
//@[
//    y = dlmread(filename, delimiter, startrow, startcol)
//@]
//where @|startrow| and @|startcol| are zero-based.  You can also specify
//the data to read using a range argument:
//@[
//    y = dlmread(filename, delimiter, range)
//@]
//where @|range| is either a vector @|[startrow,startcol,stoprow,stopcol]|
//or is specified in spreadsheet notation as @|B4..ZA5|.
//
//Note that complex numbers can be present in the file if they are encoded
//without whitespaces inside the number, and use either @|i| or @|j| as 
//the indicator.  Note also that when the delimiter is given, each incidence
//of the delimiter counts as a separator.  Multiple separators generate
//zeros in the matrix.
//!
static int ParseNumber(QString tx) {
  int lookahead = 0;
  int len = 0;
  if ((tx[len] == '+') || (tx[len] == '-'))
    len++;
  lookahead = len;
  while (tx[len].isDigit()) len++;
  lookahead = len;
  if (tx[lookahead] == '.') {
    lookahead++;
    len = 0;
    while (tx[len+lookahead].isDigit()) len++;
    lookahead+=len;
  }
  if ((tx[lookahead] == 'E') || (tx[lookahead] == 'e')) {
    lookahead++;
    if ((tx[lookahead] == '+') || (tx[lookahead] == '-')) {
      lookahead++;
    }
    len = 0;
    while (tx[len+lookahead].isDigit()) len++;
    lookahead+=len;
  }
  return lookahead;
}

static void ParseComplexValue(QString tx, double &real, double &imag) {
  int lnum = ParseNumber(tx);
  int rnum = ParseNumber(tx.mid(lnum));
  QString num1 = tx.left(lnum);
  QString num2 = tx.mid(lnum,rnum);
  if (num1.isEmpty() && num2.isEmpty()) {
    real = 0; imag = 1;
    return;
  }
  if (num1 == "+") num1 = "+1";
  if (num2 == "+") num2 = "+1";
  if (num1 == "-") num1 = "-1";
  if (num2 == "-") num2 = "-1";
  if (num2.isEmpty()) {
    real = 0;
    imag = num1.toDouble();
  } else {
    real = num1.toDouble();
    imag = num2.toDouble();
  }
}

int DecodeSpreadsheetColumn(QString tx) {
  tx.toUpper();
  QByteArray txb(tx.toLatin1());
  for (int i=0;i<txb.count();i++) 
    txb[i] = txb[i] - 'A';
  int ret = 0;
  for (int i=0;i<txb.count();i++) 
    ret += (int) (txb.at(i)*pow(26.0,txb.count()-1-i));
  return ret;
}

void DecodeSpreadsheetRange(QString tx, int &startrow, int &startcol,
			    int &stoprow, int &stopcol) {
  QString colstart;
  QString rowstart;
  QString colstop;
  QString rowstop;
  while (tx.at(0).isLetter()) {
    colstart += tx.left(1);
    tx = tx.mid(1);
  }
  while (tx.at(0).isDigit()) {
    rowstart += tx.left(1);
    tx = tx.mid(1);
  }
  tx = tx.mid(1);
  tx = tx.mid(1);
  while (tx.at(0).isLetter()) {
    colstop += tx.left(1);
    tx = tx.mid(1);
  }
  while (tx.at(0).isDigit()) {
    rowstop += tx.left(1);
    tx = tx.mid(1);
  }
  startrow = rowstart.toInt()-1;
  stoprow = rowstop.toInt()-1;
  startcol = DecodeSpreadsheetColumn(colstart);
  stopcol = DecodeSpreadsheetColumn(colstop);
}

ArrayVector DlmReadFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0) 
    throw Exception("dlmread expects a filename");
  string filename(ArrayToString(arg[0]));
  QFile ifile(QString::fromStdString(filename));
  if (!ifile.open(QFile::ReadOnly))
    throw Exception("filename " + filename + " could not be opened");
  bool no_delimiter = true;
  string delimiter;
  if (arg.size() >= 2) {
    delimiter = ArrayToString(arg[1]);
    no_delimiter = (delimiter.size() == 0);
  }
  int col_count = 0;
  int row_count = 0;
  QList<QList<double> > data_real;
  QList<QList<double> > data_imag;
  QTextStream str(&ifile);
  while (!str.atEnd()) {
    QString whole_line = str.readLine(0);
    QStringList line_pieces(whole_line.split("\r"));
    for (int i=0;i<line_pieces.size();i++) {
      QString line = line_pieces[i];
      QStringList elements;
      if (no_delimiter) {
	if (line.contains(QRegExp("[,;:]")))
	  elements = line.split(QRegExp("[,;:]"));
	else {
	  line = line.simplified();
	  elements = line.split(' ');
	}
      } else {
	elements = line.split(QString::fromStdString(delimiter)[0]);
      }
      QList<double> row_data_real;
      QList<double> row_data_imag;
      row_count++;
      for (int i=0;i<elements.size();i++) {
	QString element(elements[i]);
	element.replace(" ","");
	if (element.contains('i') || element.contains('I') ||
	    element.contains('j') || element.contains('J')) {
	  double real, imag;
	  ParseComplexValue(element,real,imag);
	  row_data_real << real;
	  row_data_imag << imag;
	} else {
	  row_data_real << element.toDouble();
	  row_data_imag << 0;
	}
      }
      col_count = qMax(col_count,elements.size());
      data_real << row_data_real;
      data_imag << row_data_imag;
    }
  }
  int startrow = 0;
  int startcol = 0;
  int stoprow = row_count-1;
  int stopcol = col_count-1;
  if (arg.size() == 4) {
    startrow = ArrayToInt32(arg[2]);
    startcol = ArrayToInt32(arg[3]);
  } else if (arg.size() == 3) {
    if (arg[2].isVector() && (arg[2].getLength() == 4)) {
      Array range(arg[2]);
      range.promoteType(FM_INT32);
      const int32*dp = (const int32*) range.getDataPointer();
      startrow = dp[0];
      startcol = dp[1];
      stoprow = dp[2];
      stopcol = dp[3];
    } else if (arg[2].isString()) 
      DecodeSpreadsheetRange(QString::fromStdString(ArrayToString(arg[2])),
			     startrow,startcol,stoprow,stopcol);
    else
      throw Exception("Unable to decode the range arguments to the dlmread function");
  }
  startrow = qMax(0,qMin(row_count-1,startrow));
  startcol = qMax(0,qMin(col_count-1,startcol));
  stoprow = qMax(0,qMin(row_count-1,stoprow));
  stopcol = qMax(0,qMin(col_count-1,stopcol));
  int numrows = stoprow-startrow+1;
  int numcols = stopcol-startcol+1;
  bool anyNonzeroImaginary = false;
  for (int i=startrow;i<=stoprow;i++) 
    for (int j=0;j<=qMin(data_real[i].size()-1,stopcol);j++) 
      if (data_imag[i][j] != 0) anyNonzeroImaginary = true;
  Array A;
  if (!anyNonzeroImaginary) {
    A = Array::doubleMatrixConstructor(numrows,numcols);
    double *dp = (double*) A.getReadWriteDataPointer();
    for (int i=startrow;i<=stoprow;i++)
      for (int j=startcol;j<=stopcol;j++)
	if (j <= (data_real[i].size()-1))
	  dp[i-startrow+(j-startcol)*numrows] = data_real[i][j];
  } else {
    A = Array::dcomplexMatrixConstructor(numrows,numcols);
    double *dp = (double*) A.getReadWriteDataPointer();
    for (int i=startrow;i<=stoprow;i++)
      for (int j=startcol;j<=stopcol;j++)
	if (j <= (data_real[i].size()-1)) {
	  dp[2*(i-startrow+(j-startcol)*numrows)] = data_real[i][j];
	  dp[2*(i-startrow+(j-startcol)*numrows)+1] = data_imag[i][j];
	}
  }
  return ArrayVector() << A;
}

ArrayVector LoadASCIIFunction(int nargout, string filename, Interpreter* eval) {
  // Hmmm...
  QFile ifile(QString::fromStdString(filename));
  if (!ifile.open(QFile::ReadOnly))
    throw Exception("filename " + filename + " could not be opened");
  QTextStream str(&ifile);
  int i=0;
  int col_count = 0;
  int row_count = 0;
  QList<double> data;
  bool evenData = true;
  while (!str.atEnd() && evenData) {
    QString line = str.readLine(0);
    line = line.simplified();
    QStringList elements(line.split(' '));
    if (row_count == 0) 
      col_count = elements.size();
    else if (elements.size() != col_count)
      evenData = false;
    if (evenData) {
      row_count++;
      for (i=0;i<elements.size();i++) 
	data << elements[i].toDouble();
    }
  }
  if (!evenData)
    eval->warningMessage("data in ASCII file does not have a uniform number of entries per row");
  // Now construct the matrix
  Array A;
  if ((row_count > 0) && (col_count > 0)) {
    A = Array::doubleMatrixConstructor(row_count,col_count);
    double *dp = (double *) A.getDataPointer();
    for (int r=0;r<row_count;r++) 
      for (int c=0;c<col_count;c++) 
	dp[r+c*row_count] = data.at(r*col_count+c);
  }
  if (nargout == 1)
    return ArrayVector() << A;
  else {
    QFileInfo fi(QString::fromStdString(filename));
    eval->getContext()->insertVariable(fi.baseName().toStdString(),A);
  }
  return ArrayVector();
}

ArrayVector LoadNativeFunction(int nargout, string filename,
			       StringVector names, bool regexpmode, Interpreter* eval) {
  File ofile(filename,"rb");
  Serialize input(&ofile);
  input.handshakeClient();
  string arrayName = input.getString();
  StringVector fieldnames;
  ArrayVector fieldvalues;
  while (arrayName != "__eof") {
    Array toRead;
    char flag;
    flag = input.getByte();
    input.getArray(toRead);
    if ((names.size() == 0) || 
	(contains(names,arrayName,regexpmode) && (nargout == 0))) {
      switch (flag) {
      case 'n':
	break;
      case 'g':
	eval->getContext()->addGlobalVariable(arrayName);
	break;
      case 'p':
	eval->getContext()->addPersistentVariable(arrayName);
	break;
      default:
	throw Exception(std::string("unrecognized variable flag ") + flag + 
			std::string(" on variable ") + arrayName);
      }
      eval->getContext()->insertVariable(arrayName,toRead);
    } else {
      fieldnames << arrayName;
      fieldvalues << toRead;
    }
    arrayName = input.getString();
  }
  if (nargout == 0)
    return ArrayVector();
  else
    return ArrayVector() <<
      Array::structConstructor(fieldnames,fieldvalues);
}

//!
//@Module LOAD Load Variables From A File
//@@Section IO
//@@Usage
//Loads a set of variables from a file in a machine independent format.
//The @|load| function takes one argument:
//@[
//  load filename,
//@]
//or alternately,
//@[
//  load('filename')
//@]
//This command is the companion to @|save|.  It loads the contents of the
//file generated by @|save| back into the current context.  Global and 
//persistent variables are also loaded and flagged appropriately.  By
//default, FreeMat assumes that files that end in a @|.mat| or @|.MAT|
//extension are MATLAB-formatted files.  Also, FreeMat assumes that 
//files that end in @|.txt| or @|.TXT| are ASCII files. 
//For other filenames, FreeMat first tries to open the file as a 
//FreeMat binary format file (as created by the @|save| function).  
//If the file fails to open as a FreeMat binary file, then FreeMat 
//attempts to read it as an ASCII file.  
//
//You can force FreeMat to assume a particular format for the file
//by using alternate forms of the @|load| command.  In particular,
//@[
//  load -ascii filename
//@]
//will load the data in file @|filename| as an ASCII file (space delimited
//numeric text) loaded into a single variable in the current workspace
//with the name @|filename| (without the extension).
//
//For MATLAB-formatted data files, you can use
//@[
//  load -mat filename
//@]
//which forces FreeMat to assume that @|filename| is a MAT-file, regardless
//of the extension on the filename.
//
//You can also specify which variables to load from a file (not from 
//an ASCII file - only single 2-D variables can be successfully saved and
//retrieved from ASCII files) using the additional syntaxes of the @|load|
//command.  In particular, you can specify a set of variables to load by name
//@[
//  load filename Var_1 Var_2 Var_3 ...
//@]
//where @|Var_n| is the name of a variable to load from the file.  
//Alternately, you can use the regular expression syntax
//@[
//  load filename -regexp expr_1 expr_2 expr_3 ...
//@]
//where @|expr_n| is a regular expression (roughly as expected by @|regexp|).
//Note that a simpler regular expression mechanism is used for this syntax
//than the full mechanism used by the @|regexp| command.
//
//Finally, you can use @|load| to create a variable containing the 
//contents of the file, instead of automatically inserting the variables
//into the curent workspace.  For this form of @|load| you must use the
//function syntax, and capture the output:
//@[
//  V = load('arg1','arg2',...)
//@]
//which returns a structure @|V| with one field for each variable
//retrieved from the file.  For ASCII files, @|V| is a double precision
//matrix.
//
//@@Example
//Here is a simple example of @|save|/@|load|.  First, we save some variables to a file.
//@<
//D = {1,5,'hello'};
//s = 'test string';
//x = randn(512,1);
//z = zeros(512);
//who
//save loadsave.dat
//@>
//Next, we clear the variables, and then load them back from the file.
//@<
//clear D s x z
//who
//load loadsave.dat
//who
//@>
//!
ArrayVector LoadFunction(int nargout, const ArrayVector& arg, 
			 Interpreter* eval) {
  // Process the arguments to extract the "-mat", "-ascii" and "-regexp" 
  // flags.
  ArrayVector argCopy;
  if (arg.size() == 0) return ArrayVector();
  bool asciiMode = false;
  bool matMode = false;
  bool regexpMode = false;
  for (int i=0;i<arg.size();i++) {
    if (arg[i].isString()) {
      if (arg[i].getContentsAsStringUpper() == "-MAT")
	matMode = true;
      else if (arg[i].getContentsAsStringUpper() == "-ASCII")
	asciiMode = true;
      else if (arg[i].getContentsAsStringUpper() == "-REGEXP")
	regexpMode = true;
      else
	argCopy << arg[i];
    } else
      argCopy << arg[i];
  }
  if (argCopy.size() < 1)  throw Exception("load requries a filename argument");
  string fname(ArrayToString(argCopy[0]));
  // If one of the filemode flags has not been specified, try to 
  // guess if it is an ASCII file or a MAT file
  if (!matMode && !asciiMode) {
    int len = fname.size();
    if ((len >= 4) && (fname[len-4] == '.') &&
	((fname[len-3] == 'M') || (fname[len-3] == 'm')) &&
	((fname[len-2] == 'A') || (fname[len-2] == 'a')) &&
	((fname[len-1] == 'T') || (fname[len-1] == 't'))) {
      matMode = true;
    } else  if ((len >= 4) && (fname[len-4] == '.') &&
		((fname[len-3] == 'T') || (fname[len-3] == 't')) &&
		((fname[len-2] == 'X') || (fname[len-2] == 'x')) &&
		((fname[len-1] == 'T') || (fname[len-1] == 't'))) {
      asciiMode = true;
    } else {
      // Could be an ASCII file - try to open it native
      try {
	File of(fname,"rb");
	Serialize input(&of);
	input.handshakeClient();
      } catch(Exception& e) {
	asciiMode = true;
      }
    }
  }
  StringVector names;
  for (int i=1;i<argCopy.size();i++) {
    if (!arg[i].isString())
      throw Exception("unexpected non-string argument to load command");
    names << ArrayToString(argCopy[i]);
  }
  // Read the data file using the appropriate handler
  try {
    if (matMode)
      return MatLoadFunction(nargout,fname,names,regexpMode,eval);
    else if (asciiMode)
      return LoadASCIIFunction(nargout,fname,eval);
    else
      return LoadNativeFunction(nargout,fname,names,regexpMode,eval);
  } catch (Exception& e) {
    throw Exception("unable to read data from file " + fname + " - it may be corrupt, or FreeMat may not understand the format.  See help load for more information.  The specific error was: " + e.getMessageCopy());
  }
  return ArrayVector();
}
