all:
	make -C core/re
	make -C core/rem
	
install: all
	make -C core/re PREFIX=@OSS_LIBRE_PREFIX@ install
	make -C core/rem PREFIX=@OSS_LIBREM_PREFIX@ install

