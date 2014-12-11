##======- lib/*/*/Makefile ------------------------------*- Makefile -*-======##
##===----------------------------------------------------------------------===##

LEVEL ?= ../../..

all:
	sage -sh -c "make -f Makefile.llvm LEVEL=$(LEVEL)"
	sage -sh -c "make -C Python -f Makefile.llvm LEVEL=$(LEVEL)/.."

