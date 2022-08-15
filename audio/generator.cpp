#include <cmath>
#include <cstdint>
#include <iostream>

static constexpr int samplerate{44'100};

namespace sound
{
	int16_t sine_wave(double time, double freq, double amp)
	{
		double tpc = samplerate / freq; // ticks per cycle
		double cycles = time / tpc;
		double rad = 2 * M_PI * cycles;
		auto amplitude = static_cast<int16_t>(32767 * amp);
		auto result = static_cast<int16_t>(amplitude * sin(rad));
		return result;
	}
}

using namespace std;

int main(int argc, char** argv)
{
	int duration{1};
	if(argc == 2)
		duration = atoi(argv[1]);

	for(int i{}; i < duration * samplerate; ++i)
	{
		auto s = sound::sine_wave(i, 440, 0.9);
		cout.write((char*)&s, 2);
	}
}
