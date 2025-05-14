#include <initguid.h>
#include <Windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <stdio.h>

#pragma comment(lib, "Winmm.lib")

DEFINE_GUID(CLSID_MMDeviceEnumerator, 0xbcde0395, 0xe52f, 0x467c, 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e);
DEFINE_GUID(IID_IMMDeviceEnumerator, 0xa95664d2, 0x9614, 0x4f35, 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6);
DEFINE_GUID(IID_IAudioMeterInformation, 0xC02216F6, 0x8C67, 0x4B5B, 0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64);
DEFINE_GUID(IID_IAudioClient, 0x1CB9AD4C, 0xDBFA, 0x4c32, 0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2);
DEFINE_GUID(IID_IAudioCaptureClient, 0xc8adbd64, 0xe71e, 0x48a0, 0xa4,0xde, 0x18,0x5c,0x39,0x5c,0xd3,0x17);

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { punk->lpVtbl->Release((punk)); (punk) = NULL; }

static const int reftimes_per_sec = 10'000'000;
static const int reftimes_per_millisec = 10'000;

void recordAudioStream(DWORD samplerate, WORD channels, WORD bits_per_sample)
{
	UINT32 numFramesAvailable;
	UINT32 packetLength;
	BYTE* pData;
	DWORD flags;
	HRESULT hr;
	IMMDeviceEnumerator* enumerator = NULL;
	IMMDevice *device = NULL;
	IAudioClient *audio_client = NULL;
	IAudioCaptureClient* capture_client	  = NULL;

	hr = CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (void**)&enumerator);

	if(FAILED(hr))
		return;

	hr = enumerator->lpVtbl->GetDefaultAudioEndpoint(enumerator, eRender, eConsole, (IMMDevice**)&device);
	if(FAILED(hr))
	{
		SAFE_RELEASE(enumerator);
		return;
	}

	hr = device->lpVtbl->Activate(device, &IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&audio_client);
	if(FAILED(hr))
	{
		SAFE_RELEASE(device);
		SAFE_RELEASE(enumerator);
		return;
	}

	WAVEFORMATEX wf;

	wf.wFormatTag		= WAVE_FORMAT_PCM;
	wf.nSamplesPerSec	= samplerate;
	wf.nChannels		= channels;
	wf.wBitsPerSample	= bits_per_sample;
	wf.nBlockAlign      = wf.nChannels * (wf.wBitsPerSample / 8);
	wf.nAvgBytesPerSec  = wf.nSamplesPerSec * wf.nBlockAlign;
	wf.cbSize           = 0;

	WAVEFORMATEX *match = NULL;

	hr = audio_client->lpVtbl->IsFormatSupported(audio_client, AUDCLNT_SHAREMODE_SHARED, &wf, &match);

	if(hr == S_OK)
		fprintf(stderr, "Format supported");
	else if(hr == AUDCLNT_E_UNSUPPORTED_FORMAT)
		fprintf(stderr, "Format is not supported");
	else if(hr == S_FALSE)
	{
		fprintf(stderr, "Unsupported format");	
		return;
	}

	hr = audio_client->lpVtbl->Initialize(audio_client, AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, reftimes_per_sec, 0, match, NULL);
	if(FAILED(hr))
	{
		SAFE_RELEASE(audio_client);
		SAFE_RELEASE(device);
		SAFE_RELEASE(enumerator);
		return;
	}

	fprintf(stderr,
			  "channels: %d,  samplerate: %d,  bitrage: %d, bps:  %d\n",
			  wf.nChannels,
			  wf.nSamplesPerSec,
			  wf.nAvgBytesPerSec,
			  wf.wBitsPerSample);

	UINT32 bufferFrameCount;
	hr = audio_client->lpVtbl->GetBufferSize(audio_client, &bufferFrameCount);
	if(FAILED(hr))
	{
		SAFE_RELEASE(audio_client);
		SAFE_RELEASE(device);
		SAFE_RELEASE(enumerator);
		return;
	}

	hr = audio_client->lpVtbl->GetService(audio_client, &IID_IAudioCaptureClient, &capture_client);
	if(FAILED(hr))
	{
		SAFE_RELEASE(audio_client);
		SAFE_RELEASE(device);
		SAFE_RELEASE(enumerator);
		return;
	}

	REFERENCE_TIME hnsActualDuration = (REFERENCE_TIME)(1.0 * reftimes_per_sec * bufferFrameCount / wf.nSamplesPerSec);

	hr = audio_client->lpVtbl->Start(audio_client);
	if(FAILED(hr))
	{
		SAFE_RELEASE(capture_client);
		SAFE_RELEASE(audio_client);
		SAFE_RELEASE(device);
		SAFE_RELEASE(enumerator);
		return;
	}

	int counter = 0;
	while(counter < 200)
	{
		Sleep((DWORD)(hnsActualDuration / reftimes_per_millisec / 2));

		hr = capture_client->lpVtbl->GetNextPacketSize(capture_client, &packetLength);
		if(FAILED(hr))
		{
			SAFE_RELEASE(capture_client);
			SAFE_RELEASE(audio_client);
			SAFE_RELEASE(device);
			SAFE_RELEASE(enumerator);
			return;
		}

		while(packetLength != 0)
		{
			hr = capture_client->lpVtbl->GetBuffer(capture_client, &pData, &numFramesAvailable, &flags, NULL, NULL);
			if(FAILED(hr))
			{
				SAFE_RELEASE(capture_client);
				SAFE_RELEASE(audio_client);
				SAFE_RELEASE(device);
				SAFE_RELEASE(enumerator);
				return;
			}

			if(flags & AUDCLNT_BUFFERFLAGS_SILENT)
				pData = NULL;

			int cbBytesToCapture = packetLength * wf.nBlockAlign;

			fprintf(stderr, "Packet: %d,  paket length: %d,  bytes to copy: %d\n", counter++, packetLength, cbBytesToCapture);

			printf("%.*s", cbBytesToCapture, (char*)pData);

			hr = capture_client->lpVtbl->ReleaseBuffer(capture_client, numFramesAvailable);
			if(FAILED(hr))
			{
				SAFE_RELEASE(capture_client);
				SAFE_RELEASE(audio_client);
				SAFE_RELEASE(device);
				SAFE_RELEASE(enumerator);
				return;
			}

			hr = capture_client->lpVtbl->GetNextPacketSize(capture_client, &packetLength);
			if(FAILED(hr))
			{
				SAFE_RELEASE(capture_client);
				SAFE_RELEASE(audio_client);
				SAFE_RELEASE(device);
				SAFE_RELEASE(enumerator);
				return;
			}
		}
	}

	hr = audio_client->lpVtbl->Stop(audio_client);
			if(FAILED(hr))
			{
				SAFE_RELEASE(capture_client);
				SAFE_RELEASE(audio_client);
				SAFE_RELEASE(device);
				SAFE_RELEASE(enumerator);
				return;
			}
}

static void usage()
{
	printf("recorder [-sr <num>] [-c <num>] [-bps <num>] file\n"
			"   file - file path where to store data\n"
			"   sr - samperate\n"
			"   c - channel count\n"
			"   bps - bits per sample\n");
}

int main(int argc, char** argv)
{
	DWORD samplerate = 48'000;
	WORD channels = 2;
	WORD bits_per_sample = 16;

	if(argc == 1)
		return usage(), 0;

	const int end = argc - 1;

	for(int i = 0; i < argc - 1; ++i)
	{
		if(strcmp(argv[i], "-sr") == 0)
		{
			if(i + 1 < end)
				samplerate = (DWORD)atoi(argv[++i]);
			continue;
		}
		else if(strcmp(argv[i], "-c") == 0)
		{
			if(i + 1 < end)
				channels = (WORD)atol(argv[++i]);
			continue;
		}
		else if(strcmp(argv[i], "-bps") == 0)
		{
			if(i + 1 < end)
				bits_per_sample = (WORD)atoi(argv[++i]);
		}
	}

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(hr != S_OK)
		return 1;

	FILE *stream;
	errno_t err = freopen_s(&stream, argv[end], "w", stdout);

	if(err != 0)
	{
		fprintf(stderr, "error opening file\n");
		return;
	}

	recordAudioStream(samplerate, channels, bits_per_sample);

	fflush(stream);
	fclose(stream);
	CoUninitialize();
}
