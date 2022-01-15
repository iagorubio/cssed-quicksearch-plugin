## Process this file with automake to produce Makefile.in

SUBDIRS = src po

EXTRA_DIST = \
	autogen.sh 

dist-hook:
	if test -d include; then \
          mkdir $(distdir)/include; \
          for header in include/*; do \
            if test -f $$header; then \
              cp -pL $$header $(distdir)/include; \
            fi \
          done \
        fi	
