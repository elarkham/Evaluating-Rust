PDFC=pdflatex

%.pdf: %.tex
	${PDFC} $<

all: design_document.pdf writing_plan.pdf

design_document.pdf: design_document.tex

writing_plan.pdf: writing_plan.tex

clean:
	rm -f design_document.pdf writing_plan.pdf
