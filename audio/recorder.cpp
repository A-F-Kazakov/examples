#include <Windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>

#include <fstream>
#include <iostream>

#pragma comment(lib, "Winmm.lib")

using namespace std;

static constexpr auto reftimes_per_sec{10'000'000};
static constexpr auto reftimes_per_millisec{10'000};

const CLSID CLSID_MMDeviceEnumerator{__uuidof(MMDeviceEnumerator)};
const IID IID_IMMDeviceEnumerator{__uuidof(IMMDeviceEnumerator)};
const IID IID_IAudioClient{__uuidof(IAudioClient)};
const IID IID_IAudioCaptureClient{__uuidof(IAudioCaptureClient)};

template<typename T>
static void deleter(T* p) { p->Release(); }

template<typename T>
static auto wrap(T* ptr)
{
	return unique_ptr<T, void(*)(T*)>(ptr, deleter);
}

template<typename T, typename Func>
static auto wrap(Func&& fn)
{
	T* ptr;
	fn(reinterpret_cast<LPVOID*>(&ptr));
	return wrap<T>(ptr);
}

void recordAudioStream(DWORD samplerate, WORD channels, WORD bits_per_sample)
{
	UINT32 numFramesAvailable;
	UINT32 packetLength;
	BYTE* pData;
	DWORD flags;
	HRESULT hr;

	auto enumerator = wrap<IMMDeviceEnumerator>([&](void** ptr) {
		hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, ptr);
		if(hr != S_OK)
			throw runtime_error("Fail to create enumerator");
	});

	auto device = wrap<IMMDevice>([&](void** ptr) {
		hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, reinterpret_cast<IMMDevice**>(ptr));
		if(hr != S_OK)
			throw runtime_error("Fail to create audio device");
	});

	auto audio_client = wrap<IAudioClient>([&](void** ptr) {
		hr = device->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr, ptr);
		if(hr != S_OK)
			throw runtime_error("Fail to create audio client");
	});

	WAVEFORMATEX wf;

	wf.wFormatTag		= WAVE_FORMAT_PCM;
	wf.nSamplesPerSec	= samplerate;
	wf.nChannels		= channels;
	wf.wBitsPerSample	= bits_per_sample;
	wf.nBlockAlign      = wf.nChannels * (wf.wBitsPerSample / 8);
	wf.nAvgBytesPerSec  = wf.nSamplesPerSec * wf.nBlockAlign;
	wf.cbSize           = 0;

	auto wReturn = audio_client->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &wf, nullptr);

	if(wReturn == S_OK)
		cerr << "Format supported" << endl;
	else if(wReturn == AUDCLNT_E_UNSUPPORTED_FORMAT)
		cerr << "Format is not supported" << endl;
	else if(wReturn == S_FALSE)
		throw runtime_error("Unsupported format");	

	hr = audio_client->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, reftimes_per_sec, 0, &wf, nullptr);
	if(hr != S_OK)
		throw runtime_error("Fail to initialize audio client");

	cerr << "channels: " << wf.nChannels
		<< " samplerate: " << wf.nSamplesPerSec
		<< " bitrage: " << wf.nAvgBytesPerSec
		<< " bps: " << wf.wBitsPerSample
		<< endl;

	UINT32 bufferFrameCount;
	hr = audio_client->GetBufferSize(&bufferFrameCount);
	if(hr != S_OK)
		throw runtime_error("Fail to obtain frame count");

	auto capture_client = wrap<IAudioCaptureClient>([&](void** ptr) {
		hr = audio_client->GetService(IID_IAudioCaptureClient, ptr);
		if(hr != S_OK)
			throw runtime_error("Fail to create capture client");
	});

	auto hnsActualDuration = static_cast<REFERENCE_TIME>(1.0 * reftimes_per_sec * bufferFrameCount / wf.nSamplesPerSec);

	hr = audio_client->Start();
	if(hr != S_OK)
		throw runtime_error("Fail to start audio");

	int counter{};
	while(counter < 200)
	{
		Sleep(static_cast<DWORD>(hnsActualDuration / reftimes_per_millisec / 2));

		hr = capture_client->GetNextPacketSize(&packetLength);
		if(hr != S_OK)
			throw runtime_error("Fail to get packet size");

		while(packetLength != 0)
		{
			hr = capture_client->GetBuffer(&pData, &numFramesAvailable, &flags, nullptr, nullptr);
			if(hr != S_OK)
				throw runtime_error("Fail to get buffer");

			if(flags & AUDCLNT_BUFFERFLAGS_SILENT)
				pData = nullptr;

			auto cbBytesToCapture = packetLength * wf.nBlockAlign;

			cerr << "Packet: " << counter++
				<< " paket length: " << packetLength
				<< " bytes to copy: " << cbBytesToCapture
				<< endl;

			cout.write((char*)pData, cbBytesToCapture);

			hr = capture_client->ReleaseBuffer(numFramesAvailable);
			if(hr != S_OK)
				throw runtime_error("Fail to release buffer");

			hr = capture_client->GetNextPacketSize(&packetLength);
			if(hr != S_OK)
				throw runtime_error("Fail to get packet size");
		}
	}

	hr = audio_client->Stop();
	if(hr != S_OK)
		throw runtime_error("Fail to stop audio client");
}

static void usage()
{
	cout << "recorder [-sr <num>] [-c <num>] [-bps <num>] file\n"
			"   file - file path where to store data\n"
			"   sr - samperate\n"
			"   c - channel count\n"
			"   bps - bits per sample\n"
		  << endl;
}

int main(int argc, char** argv)
{
	DWORD samplerate{48'000};
	WORD channels{2};
	WORD bits_per_sample{16};

	if(argc == 1)
		return usage(), 0;

	const auto end{argc - 1};

	for(int i{}; i < argc - 1; ++i)
	{
		string_view sv{argv[i]};
		if(sv == "-sr")
		{
			if(i + 1 < end)
				samplerate = std::atoi(argv[++i]);
			continue;
		}
		else if(sv == "-c")
		{
			if(i + 1 < end)
				channels = static_cast<WORD>(std::atol(argv[++i]));
			continue;
		}
		else if(sv == "-bps")
		{
			if(i + 1 < end)
				bits_per_sample = static_cast<WORD>(std::atoi(argv[++i]));
		}
	}

	auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if(hr != S_OK)
		return 1;

	try
	{
		ofstream ofs{argv[end], ios::out | ios::binary};

		cout.rdbuf(ofs.rdbuf());

		recordAudioStream(samplerate, channels, bits_per_sample);
	}
	catch (const exception& e)
	{
		cerr << e.what() << endl;
	}

	CoUninitialize();
}