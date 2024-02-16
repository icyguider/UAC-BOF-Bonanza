CXX_x64 := x86_64-w64-mingw32-g++
CC_x64 := x86_64-w64-mingw32-gcc

all:
	mkdir -p bin
	$(CXX_x64) -c src/CmstpElevatedCOMBOF.cpp -w -o bin/CmstpElevatedCOMBOF.o
	mkdir -p bin/standalone
	$(CXX_x64) src/standalone/CmstpElevatedCOM.cpp -static -s -w -lole32 -lntdll -o bin/standalone/CmstpElevatedCOM.exe

clean:
	rm -rf bin
