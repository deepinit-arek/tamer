noinst_PROGRAMS = t01 t02 t03 t04 t05 t06 t07 t08 t09 t10 t11 t12 t13 t14

t01_SOURCES = t01.tcc
t02_SOURCES = t02.tt
t03_SOURCES = t03.tcc
t04_SOURCES = t04.tt
t05_SOURCES = t05.tt
t06_SOURCES = t06.tcc
t07_SOURCES = t07.tcc
t08_SOURCES = t08.tcc
t09_SOURCES = t09.tcc
t10_SOURCES = t10.tcc
t11_SOURCES = t11.tcc
t12_SOURCES = t12.tcc
t13_SOURCES = t13.tcc
t14_SOURCES = t14.tcc

DRIVER_LIBS = @DRIVER_LIBS@
MALLOC_LIBS = @MALLOC_LIBS@
LDADD = ../tamer/libtamer.la $(DRIVER_LIBS) $(MALLOC_LIBS)

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir)
DEFS = -DTAMER_DEBUG

TAMER = ../compiler/tamer
.tt.cc:
	$(TAMER) -g -o $@ -c $<  || (rm $@ && false)
.tcc.cc:
	$(TAMER) -g -o $@ -c $<  || (rm $@ && false)

t01.cc: $(srcdir)/t01.tcc $(TAMER)
t02.cc: $(srcdir)/t02.tt $(TAMER)
t03.cc: $(srcdir)/t03.tcc $(TAMER)
t04.cc: $(srcdir)/t04.tt $(TAMER)
t05.cc: $(srcdir)/t05.tt $(TAMER)
t06.cc: $(srcdir)/t06.tcc $(TAMER)
t07.cc: $(srcdir)/t07.tcc $(TAMER)
t08.cc: $(srcdir)/t08.tcc $(TAMER)
t09.cc: $(srcdir)/t09.tcc $(TAMER)
t10.cc: $(srcdir)/t10.tcc $(TAMER)
t11.cc: $(srcdir)/t11.tcc $(TAMER)
t12.cc: $(srcdir)/t12.tcc $(TAMER)
t13.cc: $(srcdir)/t13.tcc $(TAMER)
t14.cc: $(srcdir)/t14.tcc $(TAMER)

TAMED_CXXFILES = t01.cc t02.cc t03.cc t04.cc t05.cc t06.cc t07.cc t08.cc \
	t09.cc t10.cc t11.cc t12.cc t13.cc t14.cc
CLEANFILES = $(TAMED_CXXFILES)
.PRECIOUS: $(TAMED_CXXFILES)
