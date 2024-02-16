CXX_x64 := x86_64-w64-mingw32-g++
CC_x64 := x86_64-w64-mingw32-gcc

all:
	mkdir -p bin
	mkdir -p bin/standalone
	$(CC_x64) -c src/SilentCleanupWinDirBOF.c -w -o bin/SilentCleanupWinDirBOF.o
	$(CC_x64) src/standalone/SilentCleanupWinDir.c -static -s -w -loleaut32 -lole32 -o bin/standalone/SilentCleanupWinDir.exe

clean:
	rm -rf bin
