#include <Windows.h>
#include "mq.h"  
#include "wchar.h"  

#define BUFLEN 256  
#define NUMBEROFPROPERTIES 2
  
HRESULT CreateMSMQQueue(LPWSTR wszPathName, PSECURITY_DESCRIPTOR pSecurityDescriptor, LPWSTR wszOutFormatName, DWORD *pdwOutFormatNameLength)  
{
	MQQUEUEPROPS   QueueProps;  
	MQPROPVARIANT  aQueuePropVar[NUMBEROFPROPERTIES];  
	QUEUEPROPID    aQueuePropId[NUMBEROFPROPERTIES];  
	HRESULT aQueueStatus[NUMBEROFPROPERTIES];
	HRESULT hr = MQ_OK;

	if(wszPathName == NULL || wszOutFormatName == NULL || pdwOutFormatNameLength == NULL)
		return MQ_ERROR_INVALID_PARAMETER;

	DWORD cPropId						 = 0;
	aQueuePropId[cPropId]			 = PROPID_Q_PATHNAME;
	aQueuePropVar[cPropId].vt		 = VT_LPWSTR;
	aQueuePropVar[cPropId].pwszVal = wszPathName;
	cPropId++;

	WCHAR wszLabel[MQ_MAX_Q_LABEL_LEN] = L"Test Queue";
	aQueuePropId[cPropId]				  = PROPID_Q_LABEL;
	aQueuePropVar[cPropId].vt			  = VT_LPWSTR;
	aQueuePropVar[cPropId].pwszVal	  = wszLabel;
	cPropId++;

	QueueProps.cProp	  = cPropId;
	QueueProps.aPropID  = aQueuePropId;
	QueueProps.aPropVar = aQueuePropVar;
	QueueProps.aStatus  = aQueueStatus;

	WCHAR wszFormatNameBuffer[BUFLEN];
	DWORD dwFormatNameBufferLength = BUFLEN;
	hr = MQCreateQueue(pSecurityDescriptor, &QueueProps, wszFormatNameBuffer, &dwFormatNameBufferLength);

	if(hr == MQ_OK || hr == MQ_INFORMATION_PROPERTY)
	{
		if(*pdwOutFormatNameLength >= dwFormatNameBufferLength)
		{
			wcsncpy_s(wszOutFormatName, *pdwOutFormatNameLength - 1, wszFormatNameBuffer, _TRUNCATE);

			wszOutFormatName[*pdwOutFormatNameLength - 1] = L'\0';
			*pdwOutFormatNameLength								 = dwFormatNameBufferLength;
		}
		else
			wprintf(L"The queue was created, but its format name cannot be returned.\n");
	}
	return hr;
}

int OpenMyQueue(LPWSTR wszPathName, DWORD dwAccess, DWORD dwShareMode, QUEUEHANDLE* phQueue)
{
	HRESULT hr = MQ_OK;

	if((wszPathName == NULL) || (phQueue == NULL))
		return MQ_ERROR_INVALID_PARAMETER;

	DWORD dwFormatNameBufferLength = 256;
	WCHAR wszFormatNameBuffer[256];

	hr = MQPathNameToFormatName(wszPathName, wszFormatNameBuffer, &dwFormatNameBufferLength);
	if(FAILED(hr))
	{
		fprintf(stderr, "An error occurred in MQPathNameToFormatName (error: 0x%x).\n", hr);
		return hr;
	}

	hr = MQOpenQueue(wszFormatNameBuffer, dwAccess, dwShareMode, phQueue);
	if(FAILED(hr))
	{
		fprintf(stderr, "An error occurred in MQOpenQueue (error: 0x%x.)\n", hr);
		return hr;
	}

	return hr;
}

HRESULT ReadingDestQueue(WCHAR* wszQueueName, WCHAR* wszComputerName)
{
	// const int NUMBEROFPROPERTIES = 5;
	DWORD cPropId		 = 0;
	HRESULT hr			 = MQ_OK;
	HANDLE hQueue		 = NULL;
	ULONG ulBufferSize = 2;

	MQMSGPROPS msgprops;
	MSGPROPID aMsgPropId[NUMBEROFPROPERTIES];
	MQPROPVARIANT aMsgPropVar[NUMBEROFPROPERTIES];
	HRESULT aMsgStatus[NUMBEROFPROPERTIES];

	aMsgPropId[cPropId]			= PROPID_M_LABEL_LEN;
	aMsgPropVar[cPropId].vt		= VT_UI4;
	aMsgPropVar[cPropId].ulVal = MQ_MAX_MSG_LABEL_LEN;
	cPropId++;

	WCHAR wszLabelBuffer[MQ_MAX_MSG_LABEL_LEN];
	aMsgPropId[cPropId]			  = PROPID_M_LABEL;
	aMsgPropVar[cPropId].vt		  = VT_LPWSTR;
	aMsgPropVar[cPropId].pwszVal = wszLabelBuffer;
	cPropId++;

	UCHAR* pucBodyBuffer = NULL;
	pucBodyBuffer			= (UCHAR*)malloc(ulBufferSize);
	if(pucBodyBuffer == NULL)
		return MQ_ERROR_INSUFFICIENT_RESOURCES;

	memset(pucBodyBuffer, 0, ulBufferSize);
	aMsgPropId[cPropId]		= PROPID_M_BODY_SIZE;
	aMsgPropVar[cPropId].vt = VT_NULL;
	cPropId++;

	aMsgPropId[cPropId]					= PROPID_M_BODY;
	aMsgPropVar[cPropId].vt				= VT_VECTOR | VT_UI1;
	aMsgPropVar[cPropId].caub.pElems = (UCHAR*)pucBodyBuffer;
	aMsgPropVar[cPropId].caub.cElems = ulBufferSize;
	cPropId++;

	aMsgPropId[cPropId]		= PROPID_M_BODY_TYPE;
	aMsgPropVar[cPropId].vt = VT_NULL;
	cPropId++;

	msgprops.cProp		= cPropId;
	msgprops.aPropID	= aMsgPropId;
	msgprops.aPropVar = aMsgPropVar;
	msgprops.aStatus	= aMsgStatus;

	if(wszQueueName == NULL || wszComputerName == NULL)
		return MQ_ERROR_INVALID_PARAMETER;

	WCHAR* wszFormatName		 = NULL;
	DWORD dwFormatNameLength = 0;
	dwFormatNameLength		 = wcslen(wszQueueName) + wcslen(wszComputerName) + 12;
	wszFormatName				 = malloc(dwFormatNameLength * sizeof(WCHAR));

	if(wszFormatName == NULL)
		return MQ_ERROR_INSUFFICIENT_RESOURCES;

	memset(wszFormatName, 0, dwFormatNameLength);

	hr = MQOpenQueue(wszFormatName, MQ_RECEIVE_ACCESS, MQ_DENY_NONE, &hQueue);

	if(wszFormatName)
		free(wszFormatName);

	if(FAILED(hr))
		return hr;

	for(;;)
	{
		aMsgPropVar[0].ulVal = MQ_MAX_MSG_LABEL_LEN;
		hr							= MQReceiveMessage(hQueue,	// Queue handle
									 1000,						// Max time to (msec) to receive the message
									 MQ_ACTION_RECEIVE,			// Receive action
									 &msgprops,					// Message property structure
									 NULL,						// No OVERLAPPED structure
									 NULL,						// No callback function
									 NULL,						// No cursor handle
									 MQ_NO_TRANSACTION			// Not in a transaction
		);

		if(hr == MQ_ERROR_BUFFER_OVERFLOW)
		{
			ulBufferSize  = aMsgPropVar[2].ulVal * sizeof(UCHAR);
			pucBodyBuffer = (UCHAR*)realloc(pucBodyBuffer, ulBufferSize);
			if(pucBodyBuffer == NULL)
				return MQ_ERROR_INSUFFICIENT_RESOURCES;

			memset(pucBodyBuffer, 0, ulBufferSize);
			aMsgPropVar[3].caub.pElems = (UCHAR*)pucBodyBuffer;
			aMsgPropVar[3].caub.cElems = ulBufferSize;
			continue;
		}

		if(FAILED(hr))
		{
			wprintf(L"No messages. Closing queue\n");
			break;
		}

		if(msgprops.aPropVar[0].ulVal == 0)
			wprintf(L"Removed message from queue.\n");
		else
			wprintf(L"Removed message '%s' from queue.\n", wszLabelBuffer);

		if(msgprops.aPropVar[4].ulVal == VT_BSTR)
		{
			wprintf(L"Body: %s", (WCHAR*)pucBodyBuffer);
			wprintf(L"\n");
		}
	}

	hr = MQCloseQueue(hQueue);
	free(pucBodyBuffer);

	return hr;
}

VOID CALLBACK fnReceiveCallback(HRESULT hr,
										  QUEUEHANDLE hQueue,
										  DWORD dwTimeOut,
										  DWORD dwAction,
										  MQMSGPROPS* pmsgprops,
										  LPOVERLAPPED pov,
										  HANDLE hCursor)
{
	if(SUCCEEDED(hr))
	{
		long lAppSpecific = 0;
		lAppSpecific		= pmsgprops->aPropVar[0].lVal;
		wprintf(L"Message received. Application-specific data: %ld\n", lAppSpecific);
		//SetEvent(g_hRecEvent);
	}
	else if(hr == MQ_ERROR_IO_TIMEOUT)
		//SetEvent(g_hTimeOutEvent);
	//else
		//wprintf(L"The I/O operation failed. Error: 0x%X\n", hr);

	if(pmsgprops)
	{
		//delete pmsgprops->aStatus;
		//delete pmsgprops->aPropVar;
		//delete pmsgprops->aPropID;
		//delete pmsgprops;
	}
}
