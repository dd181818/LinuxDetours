#include <cstdio>
#include "detours.h"


static unsigned int(WINAPI * TrueSleepEx)(unsigned int seconds) = sleep;

unsigned int WINAPI TimedSleepEx(unsigned int seconds)
{
	DWORD ret = TrueSleepEx(seconds);

	return ret;
}

VOID* WINAPI TestSleep(void*)
{
	printf("\n");
	fflush(stdout);


	printf("detours: Calling Sleep for 1 second.\n");
	sleep(1000);
	printf("detours: Calling SleepEx for 1 second.\n");

	printf("detours: Calling Sleep again for 1 second.\n");
	sleep(1000);
	printf("detours: Calling TimedSleep for 1 second.\n");

	printf("detours: Calling UntimedSleep for 1 second.\n");

	printf("detours: Done sleeping.\n\n");

	return NULL;
}
int main()
{
	LhBarrierProcessAttach();

	LhCriticalInitialize();


	LONG selfHandle = NULL;
	LONG selfHandle2 = NULL;
	TRACED_HOOK_HANDLE outHandle = (TRACED_HOOK_HANDLE)new BYTE[sizeof(TRACED_HOOK_HANDLE)];

	LhInstallHook((void*)TrueSleepEx, (void*)TimedSleepEx, &selfHandle, outHandle);
	ULONG ret = LhSetExclusiveACL(new ULONG[1]{ 0 }, 1, (TRACED_HOOK_HANDLE)outHandle);
	pthread_t t;
	pthread_create(&t, NULL, TestSleep, NULL);
	pthread_join(t, NULL);
	LhUninstallHook(outHandle);

	delete[] outHandle;
    return 0;
}