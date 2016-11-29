#ifndef PLAYBACKMP3_H_
#define PLAYBACKMP3_H_

#include <mad.h>
#include <pspkernel.h>
#include "artillery.h"
#include "codec.h"

struct Sample
{
	short left;
	short right;
};

class PlayBackMP3{
	public:
		static int PrintFrameInfo(struct mad_header *Header);
		static void Init(int channel);
		static int Load(string filename);
		static int Play();
		static int Stop();
		static void Pause();
		static void End();
		static int EndOfFile();
		static int GetPercentDone();
		
	private:
		static void Free();
		static void Decode();
		static void FillFileBuffer();
		static void FillOutputBuffer(void* buffer, unsigned int samplesToWrite, void* userData);
		static inline short ConvertSample(mad_fixed_t sample);
		static void ConvertLeftSamples(Sample* first, Sample* last, const mad_fixed_t* src);
		static void ConvertRightSamples(Sample* first, Sample* last, const mad_fixed_t* src);
		
	private:
		static int _isPlaying;
		static int _fileSize;
		static struct mad_stream _stream;
		static struct mad_frame _frame;
		static struct mad_synth _synth;
		static mad_timer_t _timer;
		static int _channel;
		static int _eos;
		static int _eof;
		static int _fd;
		static int _donePlaying;
		static int _readSize;
};
#endif
