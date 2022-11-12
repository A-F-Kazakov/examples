#define _UNICODE

#include <windows.h>
#include <tchar.h>

#include <stdio.h>

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

LPTSTR servicePath;
LPTSTR serviceName = "TestService";

void log(const char* str) { fprintf(stderr, str); }

void controlHandler(DWORD request)
{
	switch(request)
	{
		case SERVICE_CONTROL_STOP:
			log("Stopped.");

			serviceStatus.dwWin32ExitCode = 0;
			serviceStatus.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
			return;

		case SERVICE_CONTROL_SHUTDOWN:
			log("Shutdown.");

			serviceStatus.dwWin32ExitCode = 0;
			serviceStatus.dwCurrentState	= SERVICE_STOPPED;
			SetServiceStatus(serviceStatusHandle, &serviceStatus);
			return;

		default:
			break;
	}

	SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

void serviceMain(int argc, char** argv)
{
	(void)argc;
	(void*)argv;

	int i = 0;

	serviceStatus.dwServiceType				= SERVICE_WIN32_OWN_PROCESS;
	serviceStatus.dwCurrentState			= SERVICE_START_PENDING;
	serviceStatus.dwControlsAccepted		= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	serviceStatus.dwWin32ExitCode			= 0;
	serviceStatus.dwServiceSpecificExitCode	= 0;
	serviceStatus.dwCheckPoint				= 0;
	serviceStatus.dwWaitHint				= 0;

	serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)controlHandler);
	if(serviceStatusHandle == (SERVICE_STATUS_HANDLE)0)
		return;

	serviceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);

	while(serviceStatus.dwCurrentState == SERVICE_RUNNING)
	{
		char buffer[255];
		sprintf_s(buffer, sizeof buffer - 1, "%u", i);
		log(buffer);
		i++;
	}
}

int installService()
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if(!hSCManager)
	{
		log("Error: Can't open Service Control Manager");
		return -1;
	}

	SC_HANDLE hService = CreateService(hSCManager,
												  serviceName,
												  serviceName,
												  SERVICE_ALL_ACCESS,
												  SERVICE_WIN32_OWN_PROCESS,
												  SERVICE_DEMAND_START,
												  SERVICE_ERROR_NORMAL,
												  servicePath,
												  NULL,
												  NULL,
												  NULL,
												  NULL,
												  NULL);

	if(!hService)
	{
		int err = GetLastError();
		switch(err)
		{
			case ERROR_ACCESS_DENIED:
				log("Error: ERROR_ACCESS_DENIED");
				break;
			case ERROR_CIRCULAR_DEPENDENCY:
				log("Error: ERROR_CIRCULAR_DEPENDENCY");
				break;
			case ERROR_DUPLICATE_SERVICE_NAME:
				log("Error: ERROR_DUPLICATE_SERVICE_NAME");
				break;
			case ERROR_INVALID_HANDLE:
				log("Error: ERROR_INVALID_HANDLE");
				break;
			case ERROR_INVALID_NAME:
				log("Error: ERROR_INVALID_NAME");
				break;
			case ERROR_INVALID_PARAMETER:
				log("Error: ERROR_INVALID_PARAMETER");
				break;
			case ERROR_INVALID_SERVICE_ACCOUNT:
				log("Error: ERROR_INVALID_SERVICE_ACCOUNT");
				break;
			case ERROR_SERVICE_EXISTS:
				log("Error: ERROR_SERVICE_EXISTS");
				break;
			default:
				log("Error: Undefined");
		}
		CloseServiceHandle(hSCManager);
		return -1;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	log("Success install service!");

	return 0;
}

int removeService()
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(!hSCManager)
	{
		log("Error: Can't open Service Control Manager");
		return -1;
	}

	SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_STOP | DELETE);
	if(!hService)
	{
		log("Error: Can't remove service");
		CloseServiceHandle(hSCManager);
		return -1;
	}

	DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	log("Success remove service!");

	return 0;
}

int startService()
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_START);
	if(!StartService(hService, 0, NULL))
	{
		CloseServiceHandle(hSCManager);
		log("Error: Can't start service");
		return -1;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return 0;
}

int main(int argc, _TCHAR** argv)
{
	servicePath = (LPTSTR)argv[0];

	if(argc - 1 == 0)
	{
		SERVICE_TABLE_ENTRY serviceTable[2];
		serviceTable[0].lpServiceName = serviceName;
		serviceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)serviceMain;
		serviceTable[1].lpServiceName = NULL;
		serviceTable[1].lpServiceProc = NULL;

		if(!StartServiceCtrlDispatcher(serviceTable))
			fprintf(stderr, "Error: StartServiceCtrlDispatcher");
	}
	else if(wcscmp(argv[argc - 1], _T("install")) == 0)
		installService();
	else if(wcscmp(argv[argc - 1], _T("remove")) == 0)
		removeService();
	else if(wcscmp(argv[argc - 1], _T("start")) == 0)
		startService();

	return 0;
}