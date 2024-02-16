CXX_x64 := x86_64-w64-mingw32-g++
CC_x64 := x86_64-w64-mingw32-gcc

all:
	mkdir -p bin
	$(CC_x64) -c src/TrustedPathDLLHijackBOF.c -w -o bin/TrustedPathDLLHijackBOF.o
	mkdir -p bin/standalone
	$(CC_x64) src/standalone/TrustedPathDLLHijack.c -static -s -w -loleaut32 -lshlwapi -lole32 -o bin/standalone/TrustedPathDLLHijack.exe

clean:
	rm -rf bin
