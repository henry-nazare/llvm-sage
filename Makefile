##======- lib/*/*/Makefile ------------------------------*- Makefile -*-======##
##===----------------------------------------------------------------------===##

LEVEL ?= ../../..

SAGE_BITS = 64
SAGE_ARCH = x86_64
SAGE_OS = Linux-Ubuntu_8.04_x86_64
SAGE_VERSION = 6.4.1
SAGE_PACKAGE = sage-$(SAGE_VERSION)-$(SAGE_ARCH)-$(SAGE_OS).tar.gz
SAGE_LINK = http://linorg.usp.br/sage/linux/$(SAGE_BITS)bit/$(SAGE_PACKAGE)

all: sage
	./sage/sage -sh -c "make -C Python LEVEL=$(LEVEL)/.."
	./sage/sage -sh -c "make -f Makefile.llvm LEVEL=$(LEVEL)"

sage.tar.gz:
	wget $(SAGE_LINK) -O sage.tar.gz

sage: sage.tar.gz
	mkdir -p sage
	tar -xvf sage.tar.gz -C sage --strip-components=1
	yes | ./sage/sage -i qepcad

