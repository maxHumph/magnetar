CC=clang

build-linux:
	make -f "engine.linux.mk" all
	make -f "test.linux.mk" all

build-mac:

build-windows:

clean-linux:
	make -f "engine.linux.mk" clean
	make -f "test.linux.mk" clean

clean-mac:

clean-windows:
