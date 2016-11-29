#include <pspiofilemgr.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <malloc.h>
#include <pspaudiolib.h>
#include "playbackmp3.h"

int PlayBackMP3::_isPlaying;
int PlayBackMP3::_fileSize;
struct mad_stream PlayBackMP3::_stream;
struct mad_frame PlayBackMP3::_frame;
struct mad_synth PlayBackMP3::_synth;
mad_timer_t PlayBackMP3::_timer;
int PlayBackMP3::_channel;
int PlayBackMP3::_eos;
int PlayBackMP3::_eof;
int PlayBackMP3::_fd;
int PlayBackMP3::_donePlaying;
int PlayBackMP3::_readSize;

static unsigned int      filePos         = 0; 
static unsigned char   fileBuffer[2048]; 
static unsigned int      samplesRead; 

int PlayBackMP3::PrintFrameInfo(struct mad_header *Header)
{
    const char *Layer, *Mode, *Emphasis;

    /* Convert the layer number to it's printed representation. */
    switch (Header->layer) {
    	case MAD_LAYER_I:
			Layer = "I";
			break;
   	
   	case MAD_LAYER_II:
			Layer = "II";
			break;
   	
   	case MAD_LAYER_III:
			Layer = "III";
			break;
    	
    	default:
			Layer = "(unexpected layer value)";
			break;
    }

    /* Convert the audio mode to it's printed representation. */
    switch (Header->mode) {
   	case MAD_MODE_SINGLE_CHANNEL:
			Mode = "single channel";
			break;
   	
   	case MAD_MODE_DUAL_CHANNEL:
			Mode = "dual channel";
			break;
    	
    	case MAD_MODE_JOINT_STEREO:
			Mode = "joint (MS/intensity) stereo";
			break;
    	
    	case MAD_MODE_STEREO:
			Mode = "normal LR stereo";
			break;
    	
    	default:
			Mode = "(unexpected mode value)";
			break;
    }

    /* Convert the emphasis to it's printed representation. Note that
     * the MAD_EMPHASIS_RESERVED enumeration value appeared in libmad
     * version 0.15.0b.
     */
    switch (Header->emphasis) {
    	case MAD_EMPHASIS_NONE:
			Emphasis = "no";
			break;
    	
    	case MAD_EMPHASIS_50_15_US:
			Emphasis = "50/15 us";
			break;
    	
    	case MAD_EMPHASIS_CCITT_J_17:
			Emphasis = "CCITT J.17";
			break;
		
		#if (MAD_VERSION_MAJOR>=1) || ((MAD_VERSION_MAJOR==0) && (MAD_VERSION_MINOR>=15))
    	case MAD_EMPHASIS_RESERVED:
			Emphasis = "reserved(!)";
			break;
		#endif
    	
    	default:
			Emphasis = "(unexpected emphasis value)";
			break;
    }
    return (0);
}

int PlayBackMP3::Load(string filename)
{
   if ((_fd = sceIoOpen(filename.c_str(), PSP_O_RDONLY, 0777)) > 0)
   {
		_fileSize = sceIoLseek(_fd, 0, PSP_SEEK_END);
		sceIoLseek(_fd, 0, PSP_SEEK_SET);
		
		_isPlaying = 0;
		_donePlaying = 0;
		_readSize = 0;
		_eof = 0;
		return 1;
	}
	_isPlaying = 0;
	return 0;
}

int PlayBackMP3::Play()
{
    if (_isPlaying)
		return 0;

    _isPlaying = 1;
     pspAudioSetChannelCallback(_channel, FillOutputBuffer, NULL);
    return 1;
}

void PlayBackMP3::Pause()
{
    _isPlaying = !_isPlaying;
    pspAudioSetChannelCallback(_channel, 0, NULL);
}

int PlayBackMP3::Stop()
{
    _isPlaying = 0;
    filePos = 0; 
	 samplesRead = 0;
	 memset(fileBuffer, 0, sizeof(fileBuffer));
	 pspAudioSetChannelCallback(_channel, 0, NULL);
	 Free();
    return 1;
}

void PlayBackMP3::End()
{
    Stop();
    pspAudioSetChannelCallback(_channel, 0, NULL);
    Free();
}

void PlayBackMP3::Free()
{
    /* The input file was completely read; the memory allocated by our
     * reading module must be reclaimed.
     */
    sceIoClose(_fd);

    /* Mad is no longer used, the structures that were initialized must
     * now be cleared.
     */
    mad_synth_finish(&_synth);
    mad_frame_finish(&_frame);
    mad_stream_finish(&_stream);
}

int PlayBackMP3::EndOfFile()
{
	return _donePlaying;
}

int PlayBackMP3::GetPercentDone()
{
	if (_readSize && _fileSize)
		return ((_readSize * 100) / _fileSize);
	else
		return 0;
}

void PlayBackMP3::FillFileBuffer() 
{ 
         // Find out how much to keep and how much to fill. 
         const unsigned int   bytesToKeep   = _stream.bufend - _stream.next_frame; 
         unsigned int      bytesToFill   = sizeof(fileBuffer) - bytesToKeep; 

         // Want to keep any bytes? 
         if (bytesToKeep) 
         { 
            // Copy the tail to the head. 
            memmove(fileBuffer, fileBuffer + sizeof(fileBuffer) - bytesToKeep, bytesToKeep); 
         } 

         // Read into the rest of the file buffer. 
         unsigned char* bufferPos = fileBuffer + bytesToKeep; 
         while (bytesToFill > 0) 
         { 
            // Read some. 
            const unsigned int bytesRead = sceIoRead(_fd, bufferPos, bytesToFill); 
				
				_readSize += bytesRead;
            // EOF? 
            if (bytesRead == 0) 
            { 
               pspDebugScreenPrintf("End of file.\n"); 
               sceIoLseek(_fd, 0, SEEK_SET); 
               filePos = 0; 
               _eof = 1;
               continue; 
            } 

            // Adjust where we're writing to. 
            bytesToFill -= bytesRead; 
            bufferPos += bytesRead; 
            filePos += bytesRead; 
         } 
      } 

      void PlayBackMP3::Decode() 
      { 
         // While we need to fill the buffer... 
         while ( 
            (mad_frame_decode(&_frame, &_stream) == -1) && 
            ((_stream.error == MAD_ERROR_BUFLEN) || (_stream.error == MAD_ERROR_BUFPTR)) 
            ) 
         { 
            // Fill up the remainder of the file buffer. 
            FillFileBuffer(); 

            // Give new buffer to the stream. 
            mad_stream_buffer(&_stream, fileBuffer, sizeof(fileBuffer)); 
         } 

         // Synth the frame. 
         mad_synth_frame(&_synth, &_frame); 
      } 

      short PlayBackMP3::ConvertSample(mad_fixed_t sample) 
      { 
         /* round */ 
         sample += (1L << (MAD_F_FRACBITS - 16)); 

         /* clip */ 
         if (sample >= MAD_F_ONE) 
            sample = MAD_F_ONE - 1; 
         else if (sample < -MAD_F_ONE) 
            sample = -MAD_F_ONE; 

         /* quantize */ 
         return sample >> (MAD_F_FRACBITS + 1 - 16); 
      } 

      void PlayBackMP3::ConvertLeftSamples(Sample* first, Sample* last, const mad_fixed_t* src) 
      { 
         for (Sample* dst = first; dst != last; ++dst) 
         { 
            dst->left = ConvertSample(*src++); 
         } 
      } 

     void PlayBackMP3::ConvertRightSamples(Sample* first, Sample* last, const mad_fixed_t* src) 
      { 
         for (Sample* dst = first; dst != last; ++dst) 
         { 
            dst->right = ConvertSample(*src++); 
         } 
      } 

      void PlayBackMP3::FillOutputBuffer(void* buffer, unsigned int samplesToWrite, void* userData) 
      { 
      		if (_isPlaying)
			{
				if (_eof)
				{
					_donePlaying = 1;
					Stop();
					return;
				}
         // Where are we writing to? 
         Sample* destination = static_cast<Sample*> (buffer); 

         // While we've got samples to write... 
         while (samplesToWrite > 0) 
         { 
            // Enough samples available? 
            const unsigned int samplesAvailable = _synth.pcm.length - samplesRead; 
            if (samplesAvailable > samplesToWrite) 
            { 
               // Write samplesToWrite samples. 
               ConvertLeftSamples(destination, destination + samplesToWrite, &_synth.pcm.samples[0][samplesRead]); 
               ConvertRightSamples(destination, destination + samplesToWrite, &_synth.pcm.samples[1][samplesRead]); 

               // We're still using the same PCM data. 
               samplesRead += samplesToWrite; 

               // Done. 
               samplesToWrite = 0; 
            } 
            else 
            { 
               // Write samplesAvailable samples. 
               ConvertLeftSamples(destination, destination + samplesAvailable, &_synth.pcm.samples[0][samplesRead]); 
               ConvertRightSamples(destination, destination + samplesAvailable, &_synth.pcm.samples[1][samplesRead]); 

               // We need more PCM data. 
               samplesRead = 0; 
               Decode(); 

               // We've still got more to write. 
               destination += samplesAvailable; 
               samplesToWrite -= samplesAvailable; 
            } 
         }
         } 
      } 
      
void PlayBackMP3::Init(int channel)
{
	 _eos = 0;
    _channel = channel;
    _isPlaying = 0;
	 _readSize = 0;
    pspAudioSetChannelCallback(_channel, FillOutputBuffer, 0);
   
    mad_stream_init(&_stream);
    mad_frame_init(&_frame);
    mad_synth_init(&_synth);
}
