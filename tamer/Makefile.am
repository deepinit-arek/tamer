pkgconfigdir = ${libdir}/pkgconfig
pkgconfig_DATA = tamer.pc

lib_LTLIBRARIES = libtamer.la
libtamer_la_SOURCES = \
	adapter.hh \
	bufferedio.hh bufferedio.tt \
	driver.hh \
	dbase.cc \
	dinternal.hh dinternal.cc \
	dlibev.cc \
	dlibevent.cc \
	dtamer.cc \
	dsignal.cc \
	event.hh \
	fd.hh fd.tt \
	dns.hh dns.tt \
	lock.hh lock.tt \
	ref.hh \
	rendezvous.hh \
	tamer.hh \
	xadapter.hh xadapter.cc \
	xbase.hh xbase.cc \
	xdriver.hh \
	xevent.hh
pkginclude_HEADERS = \
	adapter.hh \
	autoconf.h \
	bufferedio.hh \
	driver.hh \
	event.hh \
	fd.hh \
	dns.hh \
	lock.hh \
	ref.hh \
	rendezvous.hh \
	tamer.hh \
	xadapter.hh \
	xbase.hh \
	xdriver.hh
EXTRA_libtamer_la_SOURCES = \
	fdhmsg.hh fdhmsg.cc \
	fdh.hh fdh.tt
libtamer_la_LIBADD = \
	$(TAMER_FDHELPER_OBJS)

bin_PROGRAMS = $(TAMER_FDHELPER_PROGRAM)
EXTRA_PROGRAMS = tamerfdh

tamerfdh_SOURCES = fdhelp.cc

EXTRA_DIST = autoconf.h.in \
	tamer.pc.in

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir)
TAMER = ../compiler/tamer

.tt.cc:
	$(TAMER) -g -o $@ -c $< || (rm $@ && false)

fd.cc: $(TAMER) fd.tt
fdh.cc: $(TAMER) fdh.tt
dns.cc: $(TAMER) dns.tt
lock.cc: $(TAMER) lock.tt
bufferedio.cc: $(TAMER) bufferedio.tt

clean-local:
	-rm -f lock.cc fd.cc fdh.cc dns.cc bufferedio.cc
