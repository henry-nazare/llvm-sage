##======- lib/*/*/Makefile ------------------------------*- Makefile -*-======##
##===----------------------------------------------------------------------===##

LEVEL ?= ../../..

SAGE_VERSION = 6.4.1
SAGE_INSTALLED_VERSION = $(shell sage --version)

ifneq ($(findstring Sage Version $(SAGE_VERSION),$(SAGE_INSTALLED_VERSION)),)
  ifeq ($(shell sage -c "print is_package_installed('qepcad')"),True)
    $(error Please install the QEPCAD package for SAGE)
  endif

bin/sage:
	ln -s $(shell which sage) $@

else
  SAGE_ARCH = $(shell uname -m)

  OS=$(shell uname -s)
  ifeq ($(OS), Linux)
    ifeq ($(SAGE_ARCH), x86_64)
      SAGE_BITS = 64
    else ifeq ($(SAGE_ARCH), i386)
      SAGE_BITS = 32
    else
      $(error Unknown architecture: $(SAGE_ARCH))
    endif

    SAGE_OS = Linux-Ubuntu_8.04_$(SAGE_ARCH)
    SAGE_PACKAGE = sage-$(SAGE_VERSION)-$(SAGE_ARCH)-$(SAGE_OS).tar.gz
    SAGE_LINK = http://linorg.usp.br/sage/linux/$(SAGE_BITS)bit/$(SAGE_PACKAGE)

sage: $(SAGE_PACKAGE)
	mkdir -p sage
	tar -xvf $(SAGE_PACKAGE) -C sage --strip-components=1
	yes | ./sage/sage -i qepcad

  else ifeq ($(OS), Darwin)
    ifneq ($(SAGE_ARCH), x86_64)
      $(error SAGE does not provide 32-bit packages for OSX)
    endif

    SAGE_OS = Darwin-OSX_10.7_$(SAGE_ARCH)
    SAGE_PACKAGE = sage-$(SAGE_VERSION)-$(SAGE_ARCH)-$(SAGE_OS).dmg
    SAGE_LINK = http://linorg.usp.br/sage/osx/intel/$(SAGE_PACKAGE)

sage: $(SAGE_PACKAGE)
	hdiutil mount -mountpoint /Volumes/sage $(SAGE_PACKAGE)
	cp -r /Volumes/sage/sage .
	yes | ./sage/sage -i qepcad

  else
    $(error Unknown operating system: $(OS))
  endif

bin/sage:
	ln -s ./sage/sage bin/

$(SAGE_PACKAGE):
	wget $(SAGE_LINK)

endif

all: bin/sage
	./sage/sage -sh -c "make -C Python LEVEL=$(LEVEL)/.."
	./sage/sage -sh -c "make -f Makefile.llvm LEVEL=$(LEVEL)"

