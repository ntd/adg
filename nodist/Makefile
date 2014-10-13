# Silent rules. By default the build is silent:
# use "make V=1" to run in verbose mode.
V=0

DOC=$(DOC_$(V))
DOC_0=@echo "  DOC   " $@;
DOC_1=

RM=$(RM_$(V))
RM_0=@echo "  RM    " $@;
RM_1=

TEX2PDF=$(TEX2PDF_$(V))
TEX2PDF_0=$(TEX2PDF_1) -silent -quiet > /dev/null
TEX2PDF_1=latexmk -rules- -lualatex -pdf

MP2MPS=$(MP2MPS_$(V))
MP2MPS_0=$(MP2MPS_1) > /dev/null
MP2MPS_1=mpost -interaction=batchmode

CLEAN=$(CLEAN_$(V))
CLEAN_0=$(CLEAN_1) -silent
CLEAN_1=latexmk -C

CONVERT=convert -flatten -density 150 -quality 90


all: overview.pdf offset.pdf desktop.png web.png

overview.pdf: overview.tex symbols-0.mps ALWAYS_OUTDATED
	$(DOC)$(TEX2PDF) overview.tex

offset.pdf: offset.tex ALWAYS_OUTDATED
	$(DOC)$(TEX2PDF) offset.tex

symbols-0.mps: symbols.mp
	$(DOC)$(MP2MPS) symbols.mp

desktop.png: overview.pdf
	$(DOC)$(CONVERT) overview.pdf[0] desktop.png

web.png: overview.pdf
	$(DOC)$(CONVERT) overview.pdf[1] web.png

clean:
	$(RM)$(CLEAN) ; \
	rm -f *.mps symbols.log desktop.png web.png


# ALWAYS_OUTDATED is a dummy phony target needed to force the build:
# latexmk is the one that will take care of the dependencies.
.PHONY: all clean ALWAYS_OUTDATED
