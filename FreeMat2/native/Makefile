all: GrayscaleImage.o RGBImage.o Color.o RGBImageGC.o Rect2D.o Point2D.o BitmapFont.o PostScriptGC.o testmain.o
	g++ -o testGC -g GrayscaleImage.o RGBImage.o Color.o RGBImageGC.o Rect2D.o Point2D.o BitmapFont.o PostScriptGC.o  testmain.o

clean:
	rm *.o
	rm testGC