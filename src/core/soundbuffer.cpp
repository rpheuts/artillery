/*
	PSPApp C++ OO Application Framework. (Initial Release: Sept. 2005)
	Copyright (C) 2005  Rafael Cabezas a.k.a. Raf

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <list>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <mad.h>
#include <malloc.h>
#include <errno.h>
#include <pspkernel.h>
#include "soundbuffer.h"
#include "artillery.h"

using namespace std;

/** Sound buffer class implementation */
SoundBuffer::SoundBuffer()
{
	/** Initialize */
	m_DeviceBuffer 		= NULL; /** One PSP sound buffer */
	m_bBuffering 		= true;
	m_mult = m_div 		= 1;
	m_NumBuffers 		= DEFAULT_NUM_BUFFERS; /** Configurable via config-file. Should be from 20 - 100 or so.. test! */
	m_BufferListMaxSize	= m_NumBuffers;
	m_DeviceBufferCount	= 0; /** Used for buffer percentage */
	m_LastDeviceBufferCount = 100; /** just has to be different from zero */

	m_DeviceBuffer		= (DeviceBuffer *)memalign(64, sizeof(DeviceBuffer));
	m_EmptyDeviceBuffer	= (DeviceBuffer *)memalign(64, sizeof(DeviceBuffer));

	memset(m_EmptyDeviceBuffer, 0, sizeof(DeviceBuffer));
	m_DeviceBufferList.clear();

}

SoundBuffer::~SoundBuffer()
{
	m_DeviceBufferList.clear();
	if (m_DeviceBuffer)
	{
		free(m_DeviceBuffer), m_DeviceBuffer = NULL;
	}
}

/*Takes the number of PSP sound buffers 20~100. If not changed, defaults to DEFAULT_NUM_BUFFERS.*/
void SoundBuffer::ChangeBufferSize(size_t buffer_size)
{
	m_BufferListMaxSize = m_NumBuffers;
	Empty();
}

void  SoundBuffer::Empty()
{
	m_DeviceBufferList.clear();
	m_bBuffering = true;
	m_mult = m_div = 1;
	m_DeviceBufferCount = 0; /** Used for buffer percentage */
	m_LastDeviceBufferCount = 100; /** just has to be different from zero */
}

size_t SoundBuffer::GetBufferFillPercentage()
{
	return 100*m_DeviceBufferCount/m_BufferListMaxSize;
};

void SoundBuffer::SampleRateChange(int newRate)
{
	if (newRate > 0)
	{
		m_mult = 1, m_div = 1;
		switch(newRate)
		{
		case 8000:
			m_mult=11;
			m_div = 2;
			break;
		case 11025:
			m_mult=4;
			m_div =1;
			break;
		case 16000:
			m_mult=11;
			m_div = 4;
			break;
		case 22050:
			m_mult=2;
			m_div =1;
			break;
		case 24000:
			m_mult=11;
			m_div =6;
			break;
		case 32000:
			m_mult=11;
			m_div = 8;
			break;
		case 44100:
			m_mult=1;
			m_div =1;
			break;
		case 47250:
			break;
		case 48000:
			m_mult=11;
			m_div=12;
			break;
		}
	}
}

/** This is called by the decoder for every decoded PCM frame */
void SoundBuffer::PushPCMFrame(Frame &frame) /** Push a frame at an arbitrary samplerate */
{
	static int timeLastPercentEvent = 500; /** Initialize to something so the percentage event is sent the first time */
	static size_t iDiv = 0;
	static DeviceBuffer buffer;
	static size_t buffer_index = 0;

	while (m_DeviceBufferCount == m_BufferListMaxSize)
	{
		sceKernelDelayThread(50); /** 50us */
	}
	
	for (size_t iMult = 0; iMult < m_mult; iMult++)
	{
		iDiv++;
		if ((iDiv % m_div) == 0)
		{
			buffer.data[buffer_index++] = frame;
			if (buffer_index == PSP_BUFFER_SIZE_IN_FRAMES)
			{
				m_DeviceBufferList.push_back(buffer);
				m_DeviceBufferCount++;
				buffer_index = 0;
			}
		}
	}

	if ((clock()*1000/CLOCKS_PER_SEC - timeLastPercentEvent) > 333) /** 3 times per sec */
	{
		/* If the buffer count has changed we should send an event */
		if (m_LastDeviceBufferCount != m_DeviceBufferCount)
		{
			m_LastDeviceBufferCount = m_DeviceBufferCount;
			timeLastPercentEvent = clock()*1000/CLOCKS_PER_SEC;
		}
	}
}

DeviceBuffer *SoundBuffer::PopDeviceBuffer()
{
	if (true == m_bBuffering)
	{
		/** Buffering!! */
		while ( (m_DeviceBufferCount < 10) )
		{
			sceKernelDelayThread(50); /** 50us */
		}
		m_bBuffering = false;
	}
	else
	{
		/** Buffer Empty!! */
		while ( (0 == m_DeviceBufferCount) )
		{
			sceKernelDelayThread(50); /** 50us */
			m_bBuffering = true;
		}
	}
	
	if (false == m_DeviceBufferList.empty())
	{
		*m_DeviceBuffer = m_DeviceBufferList.front();
		m_DeviceBufferList.pop_front();
		m_DeviceBufferCount--;

		return m_DeviceBuffer;
	}
	else
	{
		sceKernelDelayThread(50); /** 50us */
		return m_EmptyDeviceBuffer;
	}
}
