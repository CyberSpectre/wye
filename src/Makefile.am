
bin_PROGRAMS = wye-service

wye_service_SOURCES = main.C \
			connection.C \
			executable_process.C \
			job.C \
			lambda_process.C \
			lua_process.C \
			manager.C \
			process.C \
			python_process.C \
			worker.C

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(top_srcdir)/http/include -I$(top_srcdir)/process/ 
AM_CXXFLAGS = --std=c++11
AM_LDFLAGS = 

AM_CPPFLAGS += $(LUA_INCLUDE)
AM_LDFLAGS += $(LUA_LDADD)
LIBS += $(LUA_LIB)

