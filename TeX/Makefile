# Tools

PDFLATEX=pdflatex
BIBTEX=bibtex
PDFTK=pdftk
RM=rm

# Output file
PDF=main.pdf
TEXOUT=texout.log
BIBOUT=bibout.log

# Input paths
SRC=src
INC=$(TEX)/inc

# Output paths
OUT=../../build

# Input files
# no .tex allowed in MAINTEX!
MAINTEX=main
BIBFILE=$(SRC)/bib/main.bib
PREAMBLE=preamble-std.tex
STYLES=$(SRC)/styles/GostBase.clo $(SRC)/styles/G7-32.sty $(SRC)/styles/G7-32.cls $(SRC)/styles/G2-105.sty
PARTS_TEX = $(wildcard $(TEX)/[0-9][0-9]-*.tex)

# Target
all: $(PDF)

# Make options
.PHONY: all clean

$(PDF): $(SRC)/tex/$(MAINTEX).tex
	cd $(SRC)/tex && echo 'Compiling of TeX files...' && $(PDFLATEX) $(MAINTEX) -output-directory=$(OUT) && cd .. && cp bib/main.bib ../build && cp styles/gost780u.bst ../build && cd ../build && echo 'Compiling of BiB files...' && $(BIBTEX) $(MAINTEX) > $(BIBOUT) && cd ../src/tex && echo 'Compiling of TeX files...' && $(PDFLATEX) $(MAINTEX) -output-directory=$(OUT)  && echo 'Compiling of TeX files...' && $(PDFLATEX) $(MAINTEX) -output-directory=$(OUT) && echo 'Done'

clean:
	cd build && rm *