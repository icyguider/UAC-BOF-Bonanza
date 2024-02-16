CXX_x64 := x86_64-w64-mingw32-g++
CC_x64 := x86_64-w64-mingw32-gcc

all:
	mkdir -p bin
	$(CXX_x64) -c src/ColorDataProxyBOF.cpp -w -o bin/ColorDataProxyBOF.o
	mkdir -p bin/standalone
	$(CXX_x64) src/standalone/ColorDataProxy.cpp -static -s -w -lole32 -lntdll -o bin/standalone/ColorDataProxy.exe

clean:
	rm -rf bin
