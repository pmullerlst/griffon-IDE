## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = griffon

griffon_SOURCES = \
	main.c \
	interface.c interface.h \
	callbacks.c callbacks.h \
	griffon_text_document.c griffon_text_document.h \
	rox_strings.c rox_strings.h \
	griffon_funx.c griffon_funx.h \
	griffon_defs.c griffon_defs.h \
	griffon_config.c griffon_config.h \
	griffon_hl.c griffon_hl.h \
	griffon_fr.c griffon_fr.h \
	griffon_options.c griffon_options.h \
	griffon_gtk_utils.c griffon_gtk_utils.h \
	griffon_enc.c griffon_enc.h \
	help_def.c help_def.h \
	minido.c \
	griffon_doc_gen.c \
	griffon_proj.c griffon_proj.h

griffon_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

