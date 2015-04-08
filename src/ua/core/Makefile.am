all:
	make -C re
	make -C rem
	
install: all
	make -C re PREFIX=@OSS_LIBRE_PREFIX@ install
	make -C rem PREFIX=@OSS_LIBREM_PREFIX@ install

