

AC_DEFUN([SFAC_LIBRE_CONFIGURE],
[
	PROJECT_NAME=libre
	PROJECT_DIR=src/ua/core/re
	CHECKSUM_FILE=.${PREOJECT_NAME}.checksum
	CURRENT_DIR=`pwd`
	PROJECT_BUILDDIR=${CURRENT_DIR}/${PROJECT_DIR}
	PROJECT_INSTALLDIR=${prefix}/opt/ossapp/${PROJECT_NAME}
	OSS_LIBRE_PREFIX=$PROJECT_INSTALLDIR

	AC_SUBST(OSS_LIBRE_PREFIX)

	cd ${srcdir}
	SRC_DIR=`pwd`
	PROJECT_SRCDIR=${SRC_DIR}/${PROJECT_DIR}


	cd ${CURRENT_DIR}
	checksum_value_new=`find ${PROJECT_SRCDIR} -type f -name "*.*" -exec md5sum {} + | awk '{print $1}' | sort | md5sum`

	if [test -f ./${CHECKSUM_FILE}]; then
	    checksum_value_old=`cat ./${CHECKSUM_FILE}`
	fi

	if [[ "x${checksum_value_new}" != "x${checksum_value_old}" ]]; then
		if [[ "x${PROJECT_SRCDIR}" != "x${PROJECT_BUILDDIR}" ]]; then
			rm -rf ${PROJECT_BUILDDIR}
			mkdir -p ${PROJECT_BUILDDIR}
			cp -rpP ${PROJECT_SRCDIR}/* ${PROJECT_BUILDDIR}
		fi
	fi

	cd ${CURRENT_DIR}
	rm -f ./${CHECKSUM_FILE}
	echo "$checksum_value_new" > ./${CHECKSUM_FILE}

        cd ${CURRENT_DIR}
])


AC_DEFUN([SFAC_LIBREM_CONFIGURE],
[
	PROJECT_NAME=librem
	PROJECT_DIR=src/ua/core/rem
	CHECKSUM_FILE=.${PREOJECT_NAME}.checksum
	CURRENT_DIR=`pwd`
	PROJECT_BUILDDIR=${CURRENT_DIR}/${PROJECT_DIR}
	PROJECT_INSTALLDIR=${prefix}/opt/ossapp/${PROJECT_NAME}
	OSS_LIBREM_PREFIX=$PROJECT_INSTALLDIR

	AC_SUBST(OSS_LIBREM_PREFIX)

	cd ${srcdir}
	SRC_DIR=`pwd`
	PROJECT_SRCDIR=${SRC_DIR}/${PROJECT_DIR}


	cd ${CURRENT_DIR}
	checksum_value_new=`find ${PROJECT_SRCDIR} -type f -name "*.*" -exec md5sum {} + | awk '{print $1}' | sort | md5sum`

	if [test -f ./${CHECKSUM_FILE}]; then
	    checksum_value_old=`cat ./${CHECKSUM_FILE}`
	fi

	if [[ "x${checksum_value_new}" != "x${checksum_value_old}" ]]; then
		if [[ "x${PROJECT_SRCDIR}" != "x${PROJECT_BUILDDIR}" ]]; then
			rm -rf ${PROJECT_BUILDDIR}
			mkdir -p ${PROJECT_BUILDDIR}
			cp -rpP ${PROJECT_SRCDIR}/* ${PROJECT_BUILDDIR}
		fi
	fi

	cd ${CURRENT_DIR}
	rm -f ./${CHECKSUM_FILE}
	echo "$checksum_value_new" > ./${CHECKSUM_FILE}

        cd ${CURRENT_DIR}
])



