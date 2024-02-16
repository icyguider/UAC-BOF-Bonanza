CXX_x64 := x86_64-w64-mingw32-g++
CC_x64 := x86_64-w64-mingw32-gcc

all:
	mkdir -p bin
	$(CC_x64) -c src/EditionUpgradeManagerBOF.c -w -o bin/EditionUpgradeManagerBOF.o
	mkdir -p bin/standalone
	$(CC_x64) src/standalone/EditionUpgradeManager.c -static -s -w -lole32 -lntdll -o bin/standalone/EditionUpgradeManager.exe

clean:
	rm -rf bin
