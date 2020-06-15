SRC = nar.cpp

make:
	@echo "Compiling nar 2.0..."
	@g++ nar.cpp -o nar -std=c++17 && echo "Compilation finished successfully" || echo "Compilation failed"

install:
	@echo Installing nar 2.0 to ${DESTDIR}${PREFIX}/bin... && mkdir -p ${DESTDIR}${PREFIX}/bin && cp -f nar ${DESTDIR}${PREFIX}/bin && chmod 755 ${DESTDIR}${PREFIX}/bin/nar && echo "Installation finished successfully" || echo "Something gone wrong..."
