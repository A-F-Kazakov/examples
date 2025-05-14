#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static const int samplerate = 44100;

int16_t sine_wave(double time, double freq, double amp)
{
	double tpc = samplerate / freq; // ticks per cycle
	double cycles = time / tpc;
	double rad = 2 * M_PI * cycles;
	int16_t amplitude = (int16_t)(32767 * amp);
	int16_t result = (int16_t)(amplitude * sin(rad));
	return result;
}

int main(int argc, char** argv)
{
	int duration = 1;
	if(argc == 2)
		duration = atoi(argv[1]);

	for(int i = 0; i < duration * samplerate; ++i)
	{
		int16_t s = sine_wave(i, 440, 0.9);
		char* c = (char*)&s;
		printf("%c%c", c[0], c[1]);
	}
}
