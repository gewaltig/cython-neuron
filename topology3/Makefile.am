# Automake file for Topology3 module
#
# Headers from this directory are not to be installed upon
# make install. They are therefore included in _SOURCES.

# BEGIN: do not change these lines ---------------------------------------
defs= @SLI_THREAD_DEFS@ $defs
MAKEFLAGS= @MAKE_FLAGS@
# END  : do not change these lines ---------------------------------------

#SUBDIRS= doc examples

noinst_LTLIBRARIES= libtopology3module.la

# Add one line for each h-cpp-file pair containing code for your
# module. Remember the \ at the end of each but the last line!
libtopology3module_la_SOURCES= \
			topology3module.h \
			topology3module.cpp \
			topology_names.h \
			topology_names.cpp \
			connection_creator.h \
			connection_creator_impl.h \
			connection_creator.cpp \
			generic_factory.h \
			position.h \
			layer.h \
			layer.cpp \
			free_layer.h \
			mask.h \
			mask.cpp \
			ntree.h \
			ntree_impl.h \
			vose.h \
			vose.cpp \
			parameter.h \
			parameter.cpp

# do not change anything below this line ------------------------------

libtopology3module_la_CXXFLAGS= @AM_CXXFLAGS@

AM_CPPFLAGS= -I$(top_srcdir)/libnestutil\
	  -I$(top_srcdir)/librandom\
	  -I$(top_srcdir)/sli\
	  -I$(top_srcdir)/nestkernel\
	  @INCLTDL@ \
	  @GSL_CFLAGS@ @MUSIC_INCLUDE@ @MPI_INCLUDE@

nobase_pkgdata_DATA=\
  sli/topology-interface.sli

#HELPDIRS="@PKGSRCDIR@/$(subdir):@PKGSRCDIR@/$(subdir)/sli:@PKGSRCDIR@/$(subdir)/testsuite/manualtests"

install-slidoc:
	mkdir -p $(DESTDIR)@PKGDOCDIR@/help
#	$(DESTDIR)$(exec_prefix)/bin/sli --userargs=$(HELPDIRS) $(top_srcdir)/lib/sli/install-help.sli 2>&1 > @INSTALL_HELP_LOG@

install-data-hook:
	mkdir -p $(DESTDIR)@PKGDOCDIR@/topology3
#	rm -rf $(DESTDIR)@PKGDOCDIR@/topology3/testsuite
#	cp -r @PKGSRCDIR@/$(subdir)/testsuite $(DESTDIR)@PKGDOCDIR@/topology3
#	rm -rf `find $(DESTDIR)@PKGDOCDIR@/topology3 -name .svn`

install-pynest-component:
	@PYTHON_EXEC@ setup.py install --prefix=$(DESTDIR)@PYNEST_PREFIX@

#EXTRA_DIST= testsuite sli doc pynest examples
