# $Id: Makefile,v 1.7 2000/05/04 07:54:08 drt Exp $

CFLAGS = -g -Wall -Idnscache

DOWNLOADER = "wget"

all: dnscache.a dffingerd dffingerd-data dffingerd-conf

dffingerd: dffingerd.o
	$(CC) $(CFLAGS) -o dffingerd dffingerd.o dnscache.a

dffingerd-data: dffingerd-data.o
	$(CC) $(CFLAGS) -o dffingerd-data dffingerd-data.o dnscache.a

dffingerd-conf: dffingerd-conf.o
	$(CC) $(CFLAGS) -o dffingerd-conf dffingerd-conf.o dnscache.a

dnscache.a:
	if [ ! -d dnscache ]; then \
		$(DOWNLOADER) http://cr.yp.to/dnscache/dnscache-1.00.tar.gz; \
		tar xzvf dnscache-1.00.tar.gz; rm dnscache-1.00.tar.gz; \
		mv dnscache-1.00 dnscache; \
	fi;    
	cd dnscache; \
	make; \
	grep -l ^main *.c | perl -npe 's/^(.*).c/\1.o/;' | xargs rm -f; \
	ar cr ../dnscache.a *.o;
	ranlib $@       

install:
	install -m 755 -s dffingerd /usr/local/bin
	install -m 755 -s dffingerd-conf /usr/local/bin
	install -m 755 -s dffingerd-data /usr/local/bin
	install -m 644 dffingerd.8 /usr/local/man/man8
	install -m 644 dffingerd-data.8 /usr/local/man/man8
	install -m 644 dffingerd-conf.8 /usr/local/man/man8

clean:
	rm -f dffingerd dffingerd-data dffingerd-conf

distclean:
	-rm -f core dffingerd dffingerd-data dffingerd-conf *~ *.o *.a
	-rm -Rf dnscache*

