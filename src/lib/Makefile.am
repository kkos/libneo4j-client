lib_LTLIBRARIES = libneo4j-client.la

include_HEADERS = neo4j-client.h
libneo4j_client_la_SOURCES = \
	error_handling.c \
	buffering_iostream.c \
	buffering_iostream.h \
	chunking_iostream.c \
	chunking_iostream.h \
	client_config.c \
	client_config.h \
	connection.c \
	connection.h \
	deserialization.c \
	deserialization.h \
	dotdir.c \
	init.c \
	iostream.c \
	iostream.h \
	job.h \
	logging.c \
	logging.h \
	memory.c \
	memory.h \
	messages.c \
	messages.h \
	metadata.c \
	metadata.h \
	network.c \
	network.h \
	print.c \
	print.h \
	posix_iostream.c \
	posix_iostream.h \
	render.c \
	render.h \
	render_plan.c \
	render_results.c \
	result_stream.c \
	result_stream.h \
	ring_buffer.c \
	ring_buffer.h \
	serialization.c \
	serialization.h \
	session.c \
	session.h \
	thread.c \
	thread.h \
	tofu.c \
	tofu.h \
	uri.c \
	uri.h \
	util.c \
	util.h \
	values.c \
	values.h

if WITH_TLS
if HAVE_OPENSSL
libneo4j_client_la_SOURCES += \
	openssl.c \
	openssl.h \
	openssl_iostream.c \
	openssl_iostream.h
endif
endif

libneo4j_client_la_CFLAGS = $(PTHREAD_CFLAGS) $(OPENSSL_CFLAGS) -Wpedantic -Wvla
libneo4j_client_la_LDFLAGS = -version-info 7:2:0
libneo4j_client_la_LIBADD = $(PTHREAD_LIBS) $(OPENSSL_LIBS) $(LIBM)
