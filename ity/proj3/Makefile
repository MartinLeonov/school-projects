all: proj3.pdf
proj3.dvi: proj3.tex
	latex proj3.tex

proj3.ps: proj3.dvi
	dvips proj3.dvi

proj3.pdf: proj3.ps
	ps2pdf proj3.ps

.PHONY: clean
clean:
	rm -fr *.aux *.log *.tex# *.ps *.dvi *.out