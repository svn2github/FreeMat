all:
	latex2html --split 5 manual
	cd manual && zip manual.zip *.html *.png
	cd manual && zip manual.zip *.html *.png

tex:
	latex manual.tex
	latex manual.tex

pdf:
	pdflatex manual.tex

clean:
	rm -f *.aux

distclean:
