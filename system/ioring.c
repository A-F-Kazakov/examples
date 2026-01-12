#include <windows.h>
#include <ioringapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "kernel32.lib")

#define BUFFER_SIZE 256

int main()
{
	HANDLE hStdin	= NULL;
	PVOID buffer	= NULL;
	HIORING ioRing = NULL;
	IORING_CREATE_FLAGS flags;
	IORING_HANDLE_REF handleRef;
	IORING_BUFFER_REF bufferRef;
	IORING_CQE cqe;
	HRESULT hr;

	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if(hStdin == INVALID_HANDLE_VALUE)
	{
		printf("Failed to get stdin handle: %lu\n", GetLastError());
		return 1;
	}

	flags.Required = IORING_CREATE_REQUIRED_FLAGS_NONE;
	flags.Advisory = IORING_CREATE_ADVISORY_FLAGS_NONE;

	hr = CreateIoRing(IORING_VERSION_1, flags, 0, 0, &ioRing);
	if(FAILED(hr))
	{
		printf("Failed to create I/O Ring: 0x%08X\n", hr);
		return 1;
	}

	printf("I/O Ring created successfully\n");

	buffer = VirtualAlloc(NULL, BUFFER_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if(!buffer)
	{
		printf("Failed to allocate buffer: %lu\n", GetLastError());
		goto cleanup;
	}

	handleRef.Kind	  = IORING_REF_RAW;
	handleRef.Handle.Handle = hStdin;

	bufferRef.Kind	  = IORING_REF_RAW;
	bufferRef.Buffer.Address = buffer;

	printf("\nAsynchronous Console Input Example\n");
	printf("Type something and press Enter (or Ctrl+Z to exit)\n\n");

	while(1)
	{
		printf("Waiting for input\n");

		memset(buffer, 0, BUFFER_SIZE);

		hr = BuildIoRingReadFile(ioRing, handleRef, bufferRef, BUFFER_SIZE - 1, 0, 0, IOSQE_FLAGS_NONE);

		if(FAILED(hr))
		{
			printf("Failed to build read operation: 0x%08X\n", hr);
			break;
		}

		hr = SubmitIoRing(ioRing, 0, 0, NULL);
		if(FAILED(hr))
		{
			printf("Failed to submit I/O Ring: 0x%08X\n", hr);
			break;
		}

		while(1)
		{
			hr = PopIoRingCompletion(ioRing, &cqe);

			if(hr == S_OK)
			{
				if(SUCCEEDED(cqe.ResultCode))
				{
					DWORD bytesRead = (DWORD)cqe.Information;

					((char*)buffer)[bytesRead] = '\0';

					char* input = (char*)buffer;
					size_t len	= strlen(input);
					while(len > 0 && (input[len - 1] == '\n' || input[len - 1] == '\r'))
						input[--len] = '\0';

					printf("Input: \"%s\"\n", input);
					printf("Bytes read: %lu\n\n", bytesRead);

					if(strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0)
					{
						printf("Exit command received\n");
						goto cleanup;
					}
				}
				else
					printf("Read operation failed with error: 0x%08X\n", cqe.ResultCode);
				break;
			}
		}
		Sleep(100);
	}

cleanup:
	if(buffer)
		VirtualFree(buffer, 0, MEM_RELEASE);

	if(ioRing)
		CloseIoRing(ioRing);

	return 0;
}
