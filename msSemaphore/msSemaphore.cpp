#include <windows.h>
#include <stdio.h>

#define MAX_SEM_COUNT 10
#define THREADCOUNT 12

unsigned int counter;

HANDLE ghSemaphore;

DWORD WINAPI ThreadProc(LPVOID);

int main(int arc, char** argv)
{
	HANDLE aThread[THREADCOUNT];
	DWORD dwThreadId;
	int i;

	// Create a semaphore with ititial and max counts of MAX_SEM_COUNT
	ghSemaphore = CreateSemaphore(
		NULL,			// default security attribution
		MAX_SEM_COUNT,	// initial count
		MAX_SEM_COUNT,	// max count
		NULL			// unnamed semaphore
	);

	if (ghSemaphore == NULL)
	{
		printf("CreateSemaphore failed, error %d\n", GetLastError());
		return (EXIT_FAILURE);
	}

	// Create worker threads
	for (i = 0; i < THREADCOUNT; i++)
	{
		aThread[i] = CreateThread(
			NULL,	// default security attributes
			0,		// stack size
			(LPTHREAD_START_ROUTINE)ThreadProc,
			(LPVOID)(ULONGLONG)i,	// thread function arguments
			0,		// default creation flags
			&dwThreadId
		);

		printf("CreateThread[%d]\n", i);

		if (aThread[i] == NULL)
		{
			printf("CreateThread failed: %d\n", GetLastError());
			return (EXIT_FAILURE);
		}
	}

	// Wait for all threads to terminate
	WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);

	// Close thread and semaphore handles
	for (int i = 0; i < THREADCOUNT; i++)
	{
		CloseHandle(aThread[i]);
	}

	CloseHandle(ghSemaphore);

	getchar();
	return (EXIT_SUCCESS);
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	//UNREFERENCED_PARAMETER(lpParam);
	ULONGLONG threadId = (ULONGLONG)lpParam;
	DWORD dwWaitResult;
	BOOL bContinue = TRUE;

	while (bContinue)
	{
		// Try to enter the semaphore gait
		dwWaitResult = WaitForSingleObject(ghSemaphore, 0L); // second argument, timeout

		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
			// TODO: perform task
			printf("Thread[%llu] %d: wait succeeded\n", threadId, GetCurrentThreadId());
			bContinue = FALSE;

			// simulate thread work on task
			Sleep(5);

			// Release the semaphore when task is finished

			if (!ReleaseSemaphore(
				ghSemaphore,
				1,			// increase count by one
				NULL)) 	// not interested in previous count
			{
				printf("ReleaseSemaphore error: %d\n", GetLastError());
			}
			break;

			// The semaphore was nonsignaled, so a timeout ocurred
		case WAIT_TIMEOUT:
			printf("Thread %d: wait timed\n", GetCurrentThreadId());
			break;
		}
	}
	return TRUE;
}
