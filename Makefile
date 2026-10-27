CC=clang

build-linux:
	make -f "engine.linux.mk" all
	make -f "test.linux.mk" all

build-mac:
	make -f "engine.mac.mk" all
	make -f "test.mac.mk" all

build-windows:

clean-linux:
	make -f "engine.linux.mk" clean
	make -f "test.linux.mk" clean

clean-mac:
	make -f "engine.mac.mk" clean
	make -f "test.mac.mk" clean

clean-windows:
