## Process this file with automake to produce Makefile.in

SUBDIRS = src po

EXTRA_DIST = \
	autogen.sh \
	FAQ \
	BUGS \
	doc

     

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/doc; \
	cp -r $(srcdir)/doc $(DESTDIR)$(pkgdatadir); \
	cp FAQ $(DESTDIR)$(pkgdatadir)/doc; \
	cp AUTHORS $(DESTDIR)$(pkgdatadir)/doc; \
	cp ChangeLog $(DESTDIR)$(pkgdatadir)/doc; \
	cp README $(DESTDIR)$(pkgdatadir)/doc; \
	cp NEWS $(DESTDIR)$(pkgdatadir)/doc; \
	cp COPYING $(DESTDIR)$(pkgdatadir)/doc; \
	cp TODO $(DESTDIR)$(pkgdatadir)/doc; \
	cp BUGS $(DESTDIR)$(pkgdatadir)/doc; \
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi
	


uninstall:
	rm -f -r -d $(DESTDIR)$(pkgdatadir); 