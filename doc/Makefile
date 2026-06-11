#
#  Documentation Makefile
#
.SUFFIXES: .texi .ps .dvi .txt

.texi.dvi:
	./texi2dvi $<

.texi.ps: 
	dvips -f $< > $@

.dvi.txt:
	/home/olyg3/Cmd/dvi2tty $< > $@

all: r.dvi r.txt HTML/Manual/Top.html rules.tar.gz

HTML/Manual/Top.html: r.texi
	cd HTML/Manual;/bin/rm -f *.html;../../texi2www ../../r.texi

rules.tar.gz: r.texi
	tar cf rules.tar HTML/Manual; /bin/rm -f rules.tar.gz; gzip rules.tar

install: r.txt HTML/Manual/Top.html rules.tar.gz
	mv r.txt /u/ht/pbm/oly/tag/Doc
	mv r.dvi /u/ht/pbm/oly/tag/Doc
	mv rules.tar.gz /u/ht/pbm/oly/tag/Doc
	chmod go+r  /u/ht/pbm/oly/tag/Doc/*
	(cd HTML/Manual;cp *.html /u/ht/pbm/oly/tag/Manual)
	(cd /u/ht/pbm/oly/tag/Manual; fixlinks *5b*.html)
	chmod -R a+r /u/ht/pbm/oly/tag/Manual

ztar:
	tar czf ../tagdoc.tar.Z r.texi Makefile