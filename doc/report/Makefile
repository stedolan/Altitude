# Weird hax to make sure LaTeX runs enough times to 
# get the table-of-contents right
# blatantly stolen from spec makefile :D


report.pdf: *.tex report.aux report.toc
	pdflatex report.tex

report.aux: *.tex
	pdflatex report.tex

report.toc: *.tex
	pdflatex report.tex
