all: dokumentace.pdf

%.eps: %.jpg
	convert -geometry 600x500 $^ $@
%.eps: %.png
	convert -geometry 600x500 $^ $@
dokumentace.dvi: dokumentace.tex
	latex dokumentace.tex

dokumentace.ps: dokumentace.dvi
	dvips dokumentace.dvi

dokumentace.pdf: dokumentace.ps
	ps2pdf dokumentace.ps

.PHONY: clean
clean:
	rm -fr *.aux *.log *.tex# *.ps *.dvi
