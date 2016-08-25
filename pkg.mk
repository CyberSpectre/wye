
PACKAGE=wye
VERSION=0.01

deb:
	rm -rf ${PACKAGE}-${VERSION}
	autoreconf -fi
	./configure
	make dist
	cp ${PACKAGE}-${VERSION}.tar.gz ${PACKAGE}_${VERSION}.orig.tar.gz
	tar xvfz ${PACKAGE}-${VERSION}.tar.gz
	cp -r debian/ ${PACKAGE}-${VERSION}/
	cd ${PACKAGE}-${VERSION}/; dpkg-buildpackage -us -uc

rpm:
	autoreconf -fi
	./configure
	make dist
	mkdir -p RPM/SOURCES
	cp ${PACKAGE}-${VERSION}.tar.gz RPM/SOURCES/
	rpmbuild --define "_topdir `pwd`/RPM" -ba ${PACKAGE}.spec

