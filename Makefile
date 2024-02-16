CXX_x64 := x86_64-w64-mingw32-g++
CC_x64 := x86_64-w64-mingw32-gcc

all: bof standalone

bof:
	# SspiUACBypass
	mkdir -p SspiUacBypass/bin
	$(CXX_x64) -c SspiUacBypass/src/SspiUacBypassBOF.cpp -w -o SspiUacBypass/bin/SspiUacBypassBOF.o
	# TrustedPathDLLHijack
	mkdir -p TrustedPathDLLHijack/bin
	$(CC_x64) -c TrustedPathDLLHijack/src/TrustedPathDLLHijackBOF.c -w -o TrustedPathDLLHijack/bin/TrustedPathDLLHijackBOF.o
	# RegistryShellCommand
	mkdir -p RegistryShellCommand/bin
	$(CC_x64) -c RegistryShellCommand/src/RegistryShellCommandBOF.c -w -o RegistryShellCommand/bin/RegistryShellCommandBOF.o
	# SilentCleanupWinDir
	mkdir -p SilentCleanupWinDir/bin
	$(CC_x64) -c SilentCleanupWinDir/src/SilentCleanupWinDirBOF.c -w -o SilentCleanupWinDir/bin/SilentCleanupWinDirBOF.o
	# CmstpElevatedCOM
	mkdir -p CmstpElevatedCOM/bin
	$(CXX_x64) -c CmstpElevatedCOM/src/CmstpElevatedCOMBOF.cpp -w -o CmstpElevatedCOM/bin/CmstpElevatedCOMBOF.o
	# ColorDataProxy
	mkdir -p ColorDataProxy/bin
	$(CXX_x64) -c ColorDataProxy/src/ColorDataProxyBOF.cpp -w -o ColorDataProxy/bin/ColorDataProxyBOF.o
	# EditionUpgradeManager
	mkdir -p EditionUpgradeManager/bin
	$(CC_x64) -c EditionUpgradeManager/src/EditionUpgradeManagerBOF.c -w -o EditionUpgradeManager/bin/EditionUpgradeManagerBOF.o

standalone:
	# SspiUACBypass
	mkdir -p SspiUacBypass/bin/standalone
	$(CXX_x64) SspiUacBypass/src/standalone/SspiUacBypass.cpp SspiUacBypass/src/standalone/CreateSvcRpc.cpp -static -lsecur32 -s -w -o SspiUacBypass/bin/standalone/SspiUac.exe
	# TrustedPathDLLHijack
	mkdir -p TrustedPathDLLHijack/bin/standalone
	$(CC_x64) TrustedPathDLLHijack/src/standalone/TrustedPathDLLHijack.c -static -s -w -loleaut32 -lshlwapi -lole32 -o TrustedPathDLLHijack/bin/standalone/TrustedPathDLLHijack.exe
	# RegistryShellCommand
	mkdir -p RegistryShellCommand/bin/standalone
	$(CC_x64) RegistryShellCommand/src/standalone/RegistryShellCommand.c -static -s -w -lshlwapi -o RegistryShellCommand/bin/standalone/RegistryShellCommand.exe
	# SilentCleanupWinDir
	mkdir -p SilentCleanupWinDir/bin/standalone
	$(CC_x64) SilentCleanupWinDir/src/standalone/SilentCleanupWinDir.c -static -s -w -loleaut32 -lole32 -o SilentCleanupWinDir/bin/standalone/SilentCleanupWinDir.exe
	# CmstpElevatedCOM
	mkdir -p CmstpElevatedCOM/bin/standalone
	$(CXX_x64) CmstpElevatedCOM/src/standalone/CmstpElevatedCOM.cpp -static -s -w -lole32 -lntdll -o CmstpElevatedCOM/bin/standalone/CmstpElevatedCOM.exe
	# ColorDataProxy
	mkdir -p ColorDataProxy/bin/standalone
	$(CXX_x64) ColorDataProxy/src/standalone/ColorDataProxy.cpp -static -s -w -lole32 -lntdll -o ColorDataProxy/bin/standalone/ColorDataProxy.exe
	# EditionUpgradeManager
	mkdir -p EditionUpgradeManager/bin/standalone
	$(CC_x64) EditionUpgradeManager/src/standalone/EditionUpgradeManager.c -static -s -w -lole32 -lntdll -o EditionUpgradeManager/bin/standalone/EditionUpgradeManager.exe

clean:
	rm -rf TrustedPathDLLHijack/bin
	rm -rf SspiUacBypass/bin
	rm -rf RegistryShellCommand/bin
	rm -rf SilentCleanupWinDir/bin
	rm -rf CmstpElevatedCOM/bin
	rm -rf ColorDataProxy/bin
	rm -rf EditionUpgradeManager/bin
