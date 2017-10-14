PDFC=pdflatex

SRC=charter.tex

%.pdf: %.tex
	${PDFC} $<

all: charter.pdf

charter.pdf: $(SRC)

clean:
	rm -f charter.pdf
