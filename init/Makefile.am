
if HAVE_SYSTEMD
systemdsystemunit_DATA=wye.service
endif

EXTRA_DIST=wye.service.in

CLEANFILES = wye.service

do_substitution = sed \
	-e 's,[@]bindir[@],$(bindir),g' \
	-e 's,[@]sysconfdir[@],$(sysconfdir),g' \
	-e 's,[@]PACKAGE[@],$(PACKAGE),g'

wye.service: wye.service.in Makefile
	$(do_substitution) < $(srcdir)/wye.service.in > $@

