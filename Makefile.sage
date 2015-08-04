##======- Makefile.sage ---------------------------------*- Makefile -*-======##
##===----------------------------------------------------------------------===##

SAGE_VERSION = 6.8
SAGE_INSTALLED_VERSION = $(shell sage --version)

QEPCAD_PACKAGE_URL = http://cuda.dcc.ufmg.br/sage/qepcad-1.50.spkg

ifneq ($(findstring Sage Version $(SAGE_VERSION),$(SAGE_INSTALLED_VERSION)),)
  ifeq ($(shell sage -c "print is_package_installed('qepcad')"),False)
    $(error Please install the QEPCAD package for SAGE)
  endif

./bin/sage: sage
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

    SAGE_OS = Ubuntu_12.04_LTS
    SAGE_PACKAGE = $(SAGE_OS)_sage-$(SAGE_VERSION)-$(SAGE_ARCH)-Linux.tar.lrz
    SAGE_LINK = http://sagemath.c3sl.ufpr.br/linux/64bit/$(SAGE_PACKAGE)

sage: $(SAGE_PACKAGE)
	lrzuntar $(SAGE_PACKAGE)
	mv sage-$(SAGE_VERSION)-$(SAGE_ARCH)-Linux sage
	yes | ./sage/sage -i $(QEPCAD_PACKAGE_URL)
	patch sage/local/lib/python2.7/site-packages/sage/interfaces/qepcad.py \
	    qepcad_py.diff

  else ifeq ($(OS), Darwin)
    ifneq ($(SAGE_ARCH), x86_64)
      $(error SAGE does not provide 32-bit packages for OSX)
    endif

    # Override the SAGE version while OS X packages aren't available for 6.8.
    SAGE_VERSION = 6.7
    SAGE_OS = Darwin-OSX_10.7_$(SAGE_ARCH)
    SAGE_PACKAGE = sage-$(SAGE_VERSION)-$(SAGE_ARCH)-$(SAGE_OS).dmg
    SAGE_LINK = http://linorg.usp.br/sage/osx/intel/$(SAGE_PACKAGE)

sage: $(SAGE_PACKAGE)
	hdiutil mount -mountpoint /Volumes/sage $(SAGE_PACKAGE)
	cp -r /Volumes/sage/sage .
	yes | ./sage/sage -i $(QEPCAD_PACKAGE_URL)
	patch sage/local/lib/python2.7/site-packages/sage/interfaces/qepcad.py \
	    qepcad_py.diff

  else
    $(error Unknown operating system: $(OS))
  endif

./bin/sage: sage
	ln -s $(shell pwd)/sage/sage $@
	touch $(shell pwd)/sage/sage

$(SAGE_PACKAGE):
	wget $(SAGE_LINK)

endif

