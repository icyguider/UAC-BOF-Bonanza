CXX_x64 := x86_64-w64-mingw32-g++
CC_x64 := x86_64-w64-mingw32-gcc

all:
	mkdir -p bin
	$(CC_x64) -c src/RegistryShellCommandBOF.c -w -o bin/RegistryShellCommandBOF.o
	mkdir -p bin/standalone
	$(CC_x64) src/standalone/RegistryShellCommand.c -static -s -w -lshlwapi -o bin/standalone/RegistryShellCommand.exe

clean:
	rm -rf bin
