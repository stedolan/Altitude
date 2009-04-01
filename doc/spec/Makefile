# Weird hax to make sure LaTeX runs enough times to 
# get the table-of-contents right


spec.pdf: *.tex spec.aux spec.toc
	pdflatex spec.tex

spec.aux: *.tex
	pdflatex spec.tex

spec.toc: *.tex
	pdflatex spec.tex