PDFC=pdflatex

SRC=design_document.tex

%.pdf: %.tex
	${PDFC} $<

all: design_document.pdf

design_document.pdf: $(SRC)

clean:
	rm -f design_document.pdf
