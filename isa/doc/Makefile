all: manual.pdf

manual.dvi: manual.tex
	latex manual.tex

manual.ps: manual.dvi
	dvips manual.dvi

manual.pdf: manual.ps
	ps2pdf manual.ps

.PHONY: clean
clean:
	rm -fr *.aux *.log *.tex# *.ps *.dvi *.out *.pdf

