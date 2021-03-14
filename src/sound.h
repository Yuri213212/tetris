#define tempo	2205	//tempo=150bpm,speed=8x

const int note[12]={	//A-4=500Hz
	442,468,496,525,557,590,
	625,662,701,743,787,834,
};

#include "music.h"

int playing,qp,qc,phase,freq,rowcount,currentrow;
LARGE_INTEGER onesecond;
char pending[BufferLength];
LARGE_INTEGER time[BufferLength];

void sound_init(){
	QueryPerformanceFrequency(&onesecond);
	playing=ST_init;
	qp=0;
	qc=0;
	phase=0;
	freq=0;
	rowcount=0;
	currentrow=-1;
}

void sound_event(int type){
	QueryPerformanceCounter(&time[qp]);
	pending[qp]=type;
	qp=(qp+1)%BufferLength;
}

double render(int start,int end){
	double fs,fe;

	if (start&0x80000){
		fs=-1.0;
	}else{
		fs=0.0;
	}
	if (end&0x80000){
		fe=-1.0;
	}else{
		fe=0.0;
	}
	if (fs==fe){
		return fs;
	}
	if (start&0x80000){
		return (double)(0x100000-(start&0xFFFFF))/(start-end);
	}else{
		return (double)((end&0xFFFFF)-0x80000)/(start-end);
	}
}

void FillBuffer(short *buffer){
	int n,cdata;
	LARGE_INTEGER wallclock;
	long long current;

	QueryPerformanceCounter(&wallclock);
	for (n=0;n<BufferLength;++n){
		if (qc!=qp){
			current=wallclock.QuadPart+onesecond.QuadPart*(n-BufferLength)/SampleRate;
			while (qc!=qp){
				if (current<time[qc].QuadPart) break;
				if (pending[qc]>=playing){
					playing=pending[qc];
					rowcount=0;
					currentrow=-1;
					phase=0;
				}
				qc=(qc+1)%BufferLength;
			}
		}
		if (playing>=0){
			if (rowcount/tempo!=currentrow){
				++currentrow;
				cdata=music[playing][currentrow];
				switch (cdata&0x0F){
				case 0x0F:
					break;
				case 0x0E:
				case 0x0D:
					freq=0;
					break;
				case 0x0C:
					freq=0;
					phase=0;
					playing=ST_dummy;
					break;
				default:
					freq=note[cdata&0x0F]<<(cdata>>4);
					break;
				}

			}
			++rowcount;
		}
		cdata=phase;
		phase+=freq;
		buffer[n]=render(cdata,phase)*(vol<<7);
	}
}
