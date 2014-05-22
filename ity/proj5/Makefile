all: proj5.pdf

#%.eps: %.png
#	convert image.png image.pdf
#	pdftops -eps image.pdf

proj5.dvi: proj5.tex
	latex proj5.tex

proj5.ps: proj5.dvi
	dvips proj5.dvi

proj5.pdf: proj5.ps
	ps2pdf proj5.ps

.PHONY: clean
clean:
	rm -f *.aux*.log *.dvi *.out *.pdf
