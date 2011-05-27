C_RAPPORT = rapport.tex
C_SOUTENANCE = soutenance.tex

all : rapport soutenance

rapport :
	pdflatex -shell-escape $(C_RAPPORT) && pdflatex $(C_RAPPORT) && pdflatex $(C_RAPPORT)

soutenance :
	pdflatex -shell-escape $(C_SOUTENANCE) && pdflatex $(C_SOUTENANCE) && pdflatex $(C_SOUTENANCE)

