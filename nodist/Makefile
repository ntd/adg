TEX2PDF=latexmk -silent -quiet -rules- -pdf > /dev/null
MP2MPS=mpost -interaction=batchmode > /dev/null

all: overview.pdf

overview.pdf: overview.tex symbols-0.mps ALWAYS_OUTDATED
	@echo "  DOC    overview" ; \
	$(TEX2PDF) overview.tex

symbols-0.mps: symbols.mp
	@echo "  DOC    symbols" ; \
	$(MP2MPS) symbols.mp

clean:
	@echo "  RM     ." ; \
	latexmk -C -silent ; \
	rm -f *.mps symbols.log

# ALWAYS_OUTDATED is a dummy phony target needed to force the build:
# latexmk is the one that will take care of the dependencies.
.PHONY: all clean ALWAYS_OUTDATED
