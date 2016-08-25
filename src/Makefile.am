
bin_PROGRAMS = wye-service

wye_service_SOURCES = manager.C process.C service.C python.C worker.C \
	manager.h process.h worker.h ../process ../http

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(top_srcdir)/http/include -I$(top_srcdir)/process/
AM_CXXFLAGS = --std=c++11
AM_LDFLAGS = 
