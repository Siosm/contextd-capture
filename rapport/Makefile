C_RAPPORT = rapport.tex
C_SOUTENANCE = soutenance.tex

all : clean rapport soutenance

clean :
	rm *~ *.dvi

rapport :
	pdflatex $(C_RAPPORT) && pdflatex $(C_RAPPORT) && pdflatex $(C_RAPPORT)

soutenance :
	pdflatex $(C_SOUTENANCE) && pdflatex $(C_SOUTENANCE) && pdflatex $(C_SOUTENANCE)

