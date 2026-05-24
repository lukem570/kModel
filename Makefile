.PHONY: all run clean configure

all: configure
	cmake --build build

configure:
	cmake --preset debug -Wno-dev

run: all
	./build/src/kmodel

clean:
	rm -rf build
