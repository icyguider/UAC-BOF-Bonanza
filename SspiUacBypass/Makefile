CXX_x64 := x86_64-w64-mingw32-g++
CC_x64 := x86_64-w64-mingw32-gcc

all:
	mkdir -p bin
	mkdir -p bin/standalone
	$(CXX_x64) -c src/SspiUacBypassBOF.cpp -w -o bin/SspiUacBypassBOF.o
	$(CXX_x64) src/standalone/SspiUacBypass.cpp src/standalone/CreateSvcRpc.cpp -static -lsecur32 -s -w -o bin/standalone/SspiUacBypass.exe

standalone:
	mkdir -p bin/standalone
	$(CXX_x64) src/standalone/SspiUacBypass.cpp src/standalone/CreateSvcRpc.cpp -static -lsecur32 -s -w -o bin/standalone/SspiUacBypass.exe

clean:
	rm -rf bin
