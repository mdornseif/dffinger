# $Id: Makefile,v 1.3 2000/04/26 09:37:20 drt Exp $

CFLAGS = -g -Wall

DOWNLOADER = "wget"

all: djblib dffingerd dffingerd-data dffingerd-conf

dffingerd: dffingerd.o
	$(CC) $(CFLAGS) -o dffingerd dffingerd.o djb.a

dffingerd-data: dffingerd-data.o
	$(CC) $(CFLAGS) -o dffingerd-data dffingerd-data.o djb.a

dffingerd-conf: dffingerd-conf.o
	$(CC) $(CFLAGS) -o dffingerd-conf dffingerd-conf.o djb.a

djblib: 
	if [ ! -d djb ]; then \
		$(DOWNLOADER) http://cr.yp.to/dnscache/dnscache-1.00.tar.gz; \
		tar xzvf dnscache-1.00.tar.gz; \
		cd dnscache-1.00; \
		make; \
		cd ..; \
		mv dnscache-1.00 djb; \
		rm dnscache-1.00.tar.gz; \
	fi;
	make djb.a

# Library with all the nice Functions of djb
djb.a: djb/env.o djb/error.o djb/error_str.o \
djb/iopause.o djb/open_read.o djb/tai_add.o djb/tai_now.o djb/tai_pack.o \
djb/tai_uint.o djb/tai_unpack.o djb/taia_approx.o djb/taia_frac.o \
djb/taia_less.o djb/taia_now.o djb/taia_pack.o djb/taia_sub.o djb/taia_tai.o \
djb/taia_uint.o djb/taia_add.o djb/tai_sub.o djb/timeoutwrite.o \
djb/strerr_sys.o djb/strerr_die.o djb/byte_copy.o \
djb/alloc.o djb/alloc_re.o djb/stralloc_eady.o djb/stralloc_opyb.o \
djb/stralloc_opys.o djb/stralloc_pend.o djb/stralloc_catb.o djb/stralloc_copy.o \
djb/stralloc_cats.o djb/stralloc_cat.o djb/stralloc_num.o djb/getln.o djb/getln2.o \
djb/buffer.o djb/buffer_get.o djb/buffer_put.o djb/buffer_copy.o djb/buffer_2.o \
djb/buffer_1.o djb/strerr_sys.o djb/strerr_die.o \
djb/uint32_pack.o djb/uint32_unpack.o djb/uint16_pack.o djb/uint16_unpack.o \
djb/byte_copy.o djb/byte_cr.o djb/byte_chr.o djb/byte_diff.o djb/byte_zero.o djb/str_len.o \
djb/str_diff.o djb/str_start.o djb/str_rchr.o djb/str_chr.o djb/case_diffb.o \
djb/case_lowerb.o djb/fmt_ulong.o djb/scan_ulong.o djb/ip4_scan.o djb/ip4_fmt.o \
djb/case_diffs.o \
djb/dns_dfd.o djb/dns_domain.o djb/dns_dtda.o djb/dns_packet.o djb/dns_random.o \
djb/dns_sortip.o djb/dns_transmit.o djb/dns_resolve.o djb/dns_rcip.o djb/dns_ip.o \
djb/dns_ipq.o djb/dns_rcrw.o djb/dns_mx.o djb/dns_txt.o djb/dns_nd.o djb/dns_name.o \
djb/tai_add.o djb/tai_now.o djb/tai_pack.o djb/tai_uint.o djb/tai_unpack.o \
djb/taia_approx.o djb/taia_frac.o djb/taia_less.o djb/taia_now.o djb/taia_pack.o \
djb/taia_sub.o djb/taia_tai.o djb/taia_uint.o djb/taia_add.o djb/tai_sub.o \
djb/makelib djb/error.o djb/error_str.o djb/open_read.o djb/open_trunc.o djb/seek_set.o \
djb/ndelay_on.o djb/ndelay_off.o djb/socket_accept.o djb/socket_bind.o djb/socket_conn.o \
djb/socket_listen.o djb/socket_recv.o djb/socket_send.o djb/socket_tcp.o djb/socket_udp.o \
djb/cdb.o djb/cdb_hash.o djb/cdb_make.o \
djb/droproot.o djb/prot.o \
djb/generic-conf.o djb/auto_home.o djb/timeoutread.o
	ar cr $@ $^
	ranlib $@       

install:
	install -m 755 -s dffingerd /usr/local/bin
	install -m 755 -s dffingerd-conf /usr/local/bin
	install -m 755 -s dffingerd-data /usr/local/bin

clean:
	rm -f dffingerd dffingerd-data dffingerd-conf

distclean:
	-rm -f core dffingerd dffingerd-data dffingerd-conf *~ *.o *.a
	-rm -Rf djb dnscache*

