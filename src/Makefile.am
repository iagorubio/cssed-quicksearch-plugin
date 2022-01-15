## Process this file with automake to produce Makefile.in
plugindir=$(prefix)/lib/cssed/plugins
plugin_PROGRAMS=quicksearch.so

#the core pplugin files
SRCS=\
quicksearch.c 

if ISDARWIN
quicksearch_so_SOURCES = $(SRCS)
quicksearch_so_LDFLAGS =  -bundle -flat_namespace -undefined suppress
quicksearch_so_LDADD = @PACKAGE_LIBS@
quicksearch_so_CFLAGS = -fno-common
else
quicksearch_so_SOURCES= $(SRCS)
quicksearch_so_LDFLAGS = -shared -fPICS
quicksearch_so_LDADD = @PACKAGE_LIBS@
endif

INCLUDES=\
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/share/locale"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)/quicksearch-plugin"\" \
	-DPACKAGE_LIB_DIR=\""$(prefix)/lib/cssed/"\" \
	-I$(top_srcdir) -I$(top_srcdir)/src \
	@PACKAGE_CFLAGS@


