/*
 * Color reducer for pseudocolor displays.  Reduces an RGB image
 * to a paletted one (for displaying on colormapped displays).
 * Cannot handle more than 16 bit colormaps.
 */
#ifndef __Reducer_hpp__
#define __Reducer_hpp__

int ColorReduce(unsigned char *source_data, int width, int height,
		int color_count, unsigned short *output_colors, 
		unsigned short *output_data);

#endif
