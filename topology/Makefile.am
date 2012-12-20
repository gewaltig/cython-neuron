# Automake file for Topology module
#
# Headers from this directory are not to be installed upon
# make install. They are therefore included in _SOURCES.

# BEGIN: do not change these lines ---------------------------------------
defs= @SLI_THREAD_DEFS@ $defs
MAKEFLAGS= @MAKE_FLAGS@
# END  : do not change these lines ---------------------------------------

SUBDIRS= examples
EXTRA_DIST= testsuite sli doc pynest

noinst_LTLIBRARIES= libtopologymodule.la

# Add one line for each h-cpp-file pair containing code for your
# module. Remember the \ at the end of each but the last line!
libtopologymodule_la_SOURCES= \
			topologymodule.h \
			topologymodule.cpp \
			topology_names.h \
			topology_names.cpp \
			connection_creator.h \
			connection_creator_impl.h \
			connection_creator.cpp \
			generic_factory.h \
			position.h \
			layer.h \
			layer_impl.h \
			layer.cpp \
			free_layer.h \
			grid_layer.h \
			mask.h \
			mask_impl.h \
			grid_mask.h \
			ntree.h \
			ntree_impl.h \
			vose.h \
			vose.cpp \
			parameter.h \
			parameter.cpp \
			selector.h \
			selector.cpp

# do not change anything below this line ------------------------------

libtopologymodule_la_CXXFLAGS= @AM_CXXFLAGS@

AM_CPPFLAGS= -I$(top_srcdir)/libnestutil\
	  -I$(top_srcdir)/librandom\
	  -I$(top_srcdir)/sli\
	  -I$(top_srcdir)/nestkernel\
	  @INCLTDL@ \
	  @GSL_CFLAGS@ @MUSIC_INCLUDE@ @MPI_INCLUDE@

nobase_pkgdata_DATA=\
  sli/topology-interface.sli

install-data-hook:
	mkdir -p $(DESTDIR)@PKGDOCDIR@/topology
	rm -rf $(DESTDIR)@PKGDOCDIR@/topology/testsuite
	cp -r @PKGSRCDIR@/$(subdir)/testsuite $(DESTDIR)@PKGDOCDIR@/topology
	rm -rf `find $(DESTDIR)@PKGDOCDIR@/topology -name .svn`

install-pynest-component:
	@PYTHON_EXEC@ setup.py install --prefix=$(DESTDIR)@PYNEST_PREFIX@
