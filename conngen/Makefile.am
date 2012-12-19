# Automake file for ConnectionGenerator module
#
# Headers from this directory are not to be installed upon
# make install. They are therefore included in _SOURCES.

# BEGIN: do not change these lines ---------------------------------------
defs= @SLI_THREAD_DEFS@ $defs
MAKEFLAGS= @MAKE_FLAGS@
# END  : do not change these lines ---------------------------------------

noinst_LTLIBRARIES= libconngenmodule.la

# Add one line for each h-cpp-file pair containing code for your
# module. Remember the \ at the end of each but the last line!
libconngenmodule_la_SOURCES= \
		conngenmodule.h \
		conngenmodule.cpp \
		cg_connect.h \
		cg_connect.cpp

# do not change anything below this line ------------------------------

libconngenmodule_la_CXXFLAGS= @AM_CXXFLAGS@

AM_CPPFLAGS= -I$(top_srcdir)/libnestutil\
	  -I$(top_srcdir)/librandom\
	  -I$(top_srcdir)/sli\
	  -I$(top_srcdir)/nestkernel\
	  @INCLTDL@ \
	  @GSL_CFLAGS@ @MUSIC_INCLUDE@ @MPI_INCLUDE@

nobase_pkgdata_DATA=\
  sli/conngen-interface.sli

HELPDIRS="@PKGSRCDIR@/$(subdir):@PKGSRCDIR@/$(subdir)/sli"

install-slidoc:
	mkdir -p $(DESTDIR)@PKGDOCDIR@/help
	$(DESTDIR)$(exec_prefix)/bin/sli --userargs=$(HELPDIRS) $(top_srcdir)/lib/sli/install-help.sli 2>&1 > @INSTALL_HELP_LOG@

install-data-hook:
	mkdir -p $(DESTDIR)@PKGDOCDIR@/conngen
	rm -rf `find $(DESTDIR)@PKGDOCDIR@/conngen -name .svn`

EXTRA_DIST= sli
