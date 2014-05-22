all: proj1.pdf
proj1.dvi: proj1.tex
	latex proj1.tex

proj1.ps: proj1.dvi
	dvips proj1.dvi

proj1.pdf: proj1.ps
	ps2pdf proj1.ps

.PHONY: clean
clean:
	rm -fr *.aux *.log *.tex# *.ps *.dvi
