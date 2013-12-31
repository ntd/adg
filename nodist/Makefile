TEX2PDF=latexmk -silent -quiet -rules- -pdf > /dev/null
MP2MPS=mpost -interaction=batchmode > /dev/null
CONVERT=convert -flatten -density 150 -quality 90

all: overview.pdf desktop.png web.png

overview.pdf: overview.tex symbols-0.mps ALWAYS_OUTDATED
	@echo "  DOC    overview.pdf" ; \
	$(TEX2PDF) overview.tex

symbols-0.mps: symbols.mp
	@echo "  DOC    symbols" ; \
	$(MP2MPS) symbols.mp

desktop.png: overview.pdf
	@echo "  DOC    desktop.png" ; \
	$(CONVERT) overview.pdf[0] desktop.png

web.png: overview.pdf
	@echo "  DOC    web.png" ; \
	$(CONVERT) overview.pdf[1] web.png

clean:
	@echo "  RM     ." ; \
	latexmk -C -silent ; \
	rm -f *.mps symbols.log desktop.png web.png

# ALWAYS_OUTDATED is a dummy phony target needed to force the build:
# latexmk is the one that will take care of the dependencies.
.PHONY: all clean ALWAYS_OUTDATED
