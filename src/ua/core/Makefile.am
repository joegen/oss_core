EXTRA_DIST = \
  re \
  rem

all:
	make -C re
	make -C rem
	
install: all
	make -C re PREFIX=$(DESTDIR)@OSS_LIBRE_PREFIX@ install
	make -C rem PREFIX=$(DESTDIR)@OSS_LIBREM_PREFIX@ install

