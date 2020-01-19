#include "windows.h"
#include "encoder_example.h"

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "vorbis/vorbisenc.h"
#ifdef _WIN32 /* We need the following two to set stdin/stdout to binary */
#include <io.h>
#include <fcntl.h>
#endif


#define READ 1024

signed char readbuffer[READ*4+44]; /* out of the data segment, not the stack */




vorbis_dsp_state vd; 
vorbis_block     vb; 
  vorbis_info      vi; 
  vorbis_comment   vc; 
  ogg_stream_state os; 
ogg_page         og;

void CreateHeader(unsigned long Hz, DWORD Quality,WCHAR* Filenameout)
{
  ; /* one Ogg bitstream page.  Vorbis packets are inside */
  


  int ret;
  int i, founddata;



  vorbis_info_init(&vi);


  ret = vorbis_encode_init_vbr(&vi,2,Hz,(Quality / 100));

  if(ret)exit(1);
  /* add a comment */
  vorbis_comment_init(&vc);
  vorbis_comment_add_tag(&vc,"comment","This File Was Recorded using easyVoipRecorder, Please visit http://www.easyvoiprecorder.com");

  /* set up the analysis state and auxiliary encoding storage */
  vorbis_analysis_init(&vd,&vi);
  vorbis_block_init(&vd,&vb);
  
  srand(time(NULL));
  ogg_stream_init(&os,rand());

  /* Vorbis streams begin with three headers; the initial header (with
     most of the codec setup parameters) which is mandated by the Ogg
     bitstream spec.  The second header holds any comment fields.  The
     third header holds the bitstream codebook.  We merely need to
     make the headers, then pass them to libvorbis one at a time;
     libvorbis handles the additional Ogg bitstream constraints */
  {
    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&vd,&vc,&header,&header_comm,&header_code);
    ogg_stream_packetin(&os,&header); /* automatically placed in its own
					 page */
    ogg_stream_packetin(&os,&header_comm);
    ogg_stream_packetin(&os,&header_code);

	/* This ensures the actual
	 * audio data will start on a new page, as per spec
	 */

  }

  int eos=0;
  DWORD total_read=0;

  FILE *astdin; 
  FILE *astdout;
  ogg_packet       op; /* one raw packet of data for decode */

  astdout =   _wfopen(Filenameout,L"ab");
  
  while(!eos)
	{
		int result=ogg_stream_flush(&os,&og);
		if(result==0)break;
		fwrite(og.header,1,og.header_len,astdout);
		fwrite(og.body,1,og.body_len,astdout);
	}
  fclose(astdout);

}



  // aca hay que hacer el procedimiento

void ProcessData(WCHAR* FilenameMerged, WCHAR* Filenameout)
{
  int eos=0;
  DWORD total_read=0;

  FILE *astdin; 
  FILE *astdout;
  //ogg_page         og; 
  ogg_packet       op; /* one raw packet of data for decode */

  astdin =    _wfopen(FilenameMerged,L"rb");
  astdout =   _wfopen(Filenameout,L"ab");
/*  
  while(!eos)
	{
		int result=ogg_stream_flush(&os,&og);
		if(result==0)break;
		fwrite(og.header,1,og.header_len,astdout);
		fwrite(og.body,1,og.body_len,astdout);
	}
*/
  long bytes;
  while(!eos)
  {
    long i;
    bytes=fread(readbuffer,1,READ*4,astdin); /* stereo hardwired here */
	total_read = total_read + bytes;

    if(bytes==0)
	{
      /* end of file.  this can be done implicitly in the mainline,
         but it's easier to see here in non-clever fashion.
         Tell the library we're at end of stream so that it can handle
         the last frame and mark end of stream in the output properly */
      //vorbis_analysis_wrote(&vd,0);
		eos=1;
	}
	else
	{
			float **buffer=vorbis_analysis_buffer(&vd,READ);
			for(i=0;i<bytes/4;i++)
			{
				buffer[0][i]=((readbuffer[i*4+1]<<8)|
						(0x00ff&(int)readbuffer[i*4]))/32768.f;
				buffer[1][i]=((readbuffer[i*4+3]<<8)|
						(0x00ff&(int)readbuffer[i*4+2]))/32768.f;
			}
			vorbis_analysis_wrote(&vd,i);
	  }
	
		while(vorbis_analysis_blockout(&vd,&vb)==1)
		{
		/* analysis, assume we want to use bitrate management */
		vorbis_analysis(&vb,NULL);
		vorbis_bitrate_addblock(&vb);
		while(vorbis_bitrate_flushpacket(&vd,&op))
		{
			/* weld the packet into the bitstream */
			ogg_stream_packetin(&os,&op);
			/* write out pages (if any) */
			while(!eos)
			{
				int result=ogg_stream_pageout(&os,&og);
				if(result==0)break;
				fwrite(og.header,1,og.header_len,astdout);
				fwrite(og.body,1,og.body_len,astdout);
				/* this could be set above, but for illustrative purposes, I do
				it here (to show that vorbis does know where the stream ends) */
				if(ogg_page_eos(&og))eos=1;
			}
		}
	}
  }
  fclose(astdin);
  fclose(astdout);


  /* clean up and exit.  vorbis_info_clear() must be called last */

}

void EndOgg(void)
{
  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);
  vorbis_comment_clear(&vc);
  vorbis_info_clear(&vi);
  ogg_stream_clear(&os);

};
