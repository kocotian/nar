SRC = nar.cpp

make:
	@g++ nar.cpp -o nar -std=c++17

install:
	@echo Installing to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f nar ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/nar
