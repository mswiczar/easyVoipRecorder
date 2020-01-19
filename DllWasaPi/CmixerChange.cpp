#include "windows.h"
#include "Mmsystem.h"
#include "stdafx.h"
#include "wav.h"


DWORD volant;

char*  _amix = "Mix";
char*  _awave = "Wave";
char*  _asum = "Sum";
char*  _astereo = "Stereo";
char*  _usado ="";


void SetVolume(char * aaa, DWORD posisicon, int desttt)
{


MIXERLINECONTROLS mymixerlinecontrols;
MIXERCAPS     mixercaps;
MIXERLINE     mixerline;
MMRESULT      err;
unsigned long i, n, numSrc;


/* Get info about the first Mixer Device */
if (!(err = mixerGetDevCaps((UINT)0, &mixercaps, sizeof(MIXERCAPS))))
{
    /* Print out the name of each destination line */
    for (i = 0; i < mixercaps.cDestinations; i++)
    {
        mixerline.cbStruct = sizeof(MIXERLINE);
		mixerline.dwDestination =i;
		

        if (!(err = mixerGetLineInfo((HMIXEROBJ)0, &mixerline, MIXER_GETLINEINFOF_DESTINATION)))
		//if (!(err = mixerGetLineInfo((HMIXEROBJ)0, &mixerline, MIXER_GETLINEINFOF_COMPONENTTYPE)))
		
        {
            printf("Destination #%lu = %s\n", i, mixerline.szName);

            /* Print out the name of each source line in this destination */
            numSrc = mixerline.cConnections;
            for (n = 0; n < numSrc; n++)
            {
                mixerline.cbStruct = sizeof(MIXERLINE);
                mixerline.dwDestination = i;
                mixerline.dwSource = n;

                if (!(err = mixerGetLineInfo((HMIXEROBJ)0, &mixerline, MIXER_GETLINEINFOF_SOURCE)))
                {

					if (NULL != strstr(mixerline.szName,aaa) && (mixerline.dwDestination !=desttt ) )
							{
								printf("ok found %s\n",mixerline.szName);
								
								MIXERLINECONTROLS  mixerLineControls;
								mixerLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
								mixerLineControls.cControls = mixerline.cControls;
								mixerLineControls.dwLineID =  mixerline.dwLineID;
								MIXERCONTROL       mixerControlArray[10];

								mixerLineControls.pamxctrl = &mixerControlArray[0];
								mixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);

								mixerGetLineControls((HMIXEROBJ)0, &mixerLineControls, MIXER_GETLINECONTROLSF_ALL);
								MIXERCONTROLDETAILS          mixerControlDetails;
								MIXERCONTROLDETAILS_UNSIGNED value;
								// averiguar cual es el canal
								for (int a=0; a<mixerLineControls.cControls;a++)
								{
									if (NULL != strstr(mixerLineControls.pamxctrl[a].szName,"olume"))
									{
																			
										mixerControlDetails.cbStruct = sizeof(MIXERCONTROLDETAILS);
										mixerControlDetails.dwControlID = mixerLineControls.pamxctrl[a].dwControlID;
										mixerControlDetails.cChannels = 1;
										mixerControlDetails.cMultipleItems = 0;
										mixerControlDetails.paDetails = &value;
										/* Tell mixerSetControlDetails() how big the MIXERCONTROLDETAILS_UNSIGNED is */
										mixerControlDetails.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
										/* Store the value */
										value.dwValue = posisicon;
										mixerSetControlDetails((HMIXEROBJ)0, &mixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);
										break; 
									}

								}
							}
					printf("\tSource #%lu = %s %u \n", i, mixerline.szName,mixerline.dwSource);
                }
            }
        }
    }
}

}









int SetMixerTo( HMIXER hMixer,  DWORD componentType) 
{
	MMRESULT ret;
	
	//MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
	//componentType = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT;
    DWORD dwControlType; 
    DWORD dwi = 0; 
	//hMixer  =  0;
    // get dwLineID 
    MIXERLINE mxl; 
    mxl.cbStruct = sizeof(MIXERLINE); 
    mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN; 

	ret = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);

        // get dwControlID 
    MIXERCONTROL mxc; 
    MIXERLINECONTROLS mxlc; 
    dwControlType = MIXERCONTROL_CONTROLTYPE_MIXER; 
    mxlc.cbStruct = sizeof(MIXERLINECONTROLS); 
    mxlc.dwLineID = mxl.dwLineID; 
    mxlc.dwControlType = dwControlType; 
    mxlc.cControls = 1; 
    mxlc.cbmxctrl = sizeof(MIXERCONTROL); 
    mxlc.pamxctrl = &mxc; 
    ret = mixerGetLineControls( (HMIXEROBJ)hMixer, &mxlc,MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
	if (ret != 0)
	{
                // no mixer, try MUX 
                dwControlType = MIXERCONTROL_CONTROLTYPE_MUX; 
                mxlc.cbStruct = sizeof(MIXERLINECONTROLS); 
                mxlc.dwLineID = mxl.dwLineID; 
                mxlc.dwControlType = dwControlType; 
                mxlc.cControls = 1; 
                mxlc.cbmxctrl = sizeof(MIXERCONTROL); 
                mxlc.pamxctrl = &mxc; 
				mixerGetLineControls( (HMIXEROBJ)hMixer, &mxlc,MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE);
     } 

	
        if (mxc.cMultipleItems == 0) 
                return 0; 


        // get the index of the Select control 
        MIXERCONTROLDETAILS_LISTTEXT *pmxcdSelectText = new MIXERCONTROLDETAILS_LISTTEXT[mxc.cMultipleItems]; 
        if(pmxcdSelectText != NULL) 
        { 
                MIXERCONTROLDETAILS mxcd; 
                mxcd.cbStruct = sizeof (MIXERCONTROLDETAILS); 
                mxcd.dwControlID = mxc.dwControlID; 
                mxcd.cChannels = 1; 
                mxcd.cMultipleItems = mxc.cMultipleItems; 
                mxcd.cbDetails = sizeof (MIXERCONTROLDETAILS_LISTTEXT); 
                mxcd.paDetails = pmxcdSelectText; 
                if(mixerGetControlDetails ((HMIXEROBJ)hMixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_LISTTEXT) == MMSYSERR_NOERROR) 
                { 
                        // determine which controls the source line 
                        for (dwi = 0; dwi < mxc.cMultipleItems; dwi++) 
                        { 
                                // get the line information 
                                MIXERLINE tmxl; 
                                tmxl.cbStruct = sizeof (MIXERLINE); 
                                tmxl.dwLineID = pmxcdSelectText[dwi].dwParam1; 
                                //if((mixerGetLineInfo ((HMIXEROBJ)hMixer, &tmxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID) == MMSYSERR_NOERROR) &&  tmxl.dwComponentType == componentType) 
                                if(mixerGetLineInfo ((HMIXEROBJ)hMixer, &tmxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID) == MMSYSERR_NOERROR) 

								{ 
									if ((tmxl.dwComponentType == componentType )&& (componentType != MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT ) )                                
									{
										break; 
									}
									// found, dwi is the index. 
                                } 
                        } 
                } 

        } 
    
    if(dwi >= mxc.cMultipleItems) 
	{
                    printf("Buscando\n");    
					for (dwi = 0; dwi < mxc.cMultipleItems; dwi++) 
                        { 
                                // get the line information 
                                MIXERLINE tmxl; 
                                tmxl.cbStruct = sizeof (MIXERLINE); 
                                tmxl.dwLineID = pmxcdSelectText[dwi].dwParam1; 
                                if(mixerGetLineInfo ((HMIXEROBJ)hMixer, &tmxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_LINEID) == MMSYSERR_NOERROR) 

								{ 

									printf("%s\n",tmxl.szName);

									if (NULL != strstr(tmxl.szName,_usado))
									{
										printf("ok found %s\n",tmxl.szName);
										break; 
									}
									// found, dwi is the index. 
								}
                        } 
					if(dwi >= mxc.cMultipleItems) 
					{
						delete []pmxcdSelectText; 
						return 0; 
					}

	}
	else
	{
        //delete []pmxcdSelectText; 
	}

		
		
		
		
		
		

        // get all the values first 
        MIXERCONTROLDETAILS_BOOLEAN *pmxcdSelectValue = new MIXERCONTROLDETAILS_BOOLEAN[mxc.cMultipleItems]; 
        if (pmxcdSelectValue != NULL) 
        { 
                MIXERCONTROLDETAILS mxcd; 
                mxcd.cbStruct = sizeof (MIXERCONTROLDETAILS); 
                mxcd.dwControlID = mxc.dwControlID; 
                mxcd.cChannels = 1; 
                mxcd.cMultipleItems = mxc.cMultipleItems; 
                mxcd.cbDetails = sizeof (MIXERCONTROLDETAILS_BOOLEAN); 
                mxcd.paDetails = pmxcdSelectValue; 
                if(mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR) 
                { 
                        // MUX restricts the line selection to one source line at a time. 
                        if (dwControlType == MIXERCONTROL_CONTROLTYPE_MUX) 
                                ZeroMemory (pmxcdSelectValue, mxc.cMultipleItems * sizeof (MIXERCONTROLDETAILS_BOOLEAN)); 

                        // set the value 
                        pmxcdSelectValue[dwi].fValue = 1; 
                        mxcd.cbStruct = sizeof (MIXERCONTROLDETAILS); 
                        mxcd.dwControlID = mxc.dwControlID; 
                        mxcd.cChannels = 1; 
                        mxcd.cMultipleItems = mxc.cMultipleItems; 
                        mxcd.cbDetails = sizeof (MIXERCONTROLDETAILS_BOOLEAN); 
                        mxcd.paDetails = pmxcdSelectValue; 
						
						if(mixerSetControlDetails ((HMIXEROBJ)(hMixer), &mxcd, MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) == MMSYSERR_NOERROR) 
                                return 1; 
                } 
                delete []pmxcdSelectValue; 
        } 


}


int  EnableMixer(char * aaa, int desttt)
{
int enables = 0;

MIXERLINECONTROLS mymixerlinecontrols;
MIXERCAPS     mixercaps;
MIXERLINE     mixerline;
MMRESULT      err;
unsigned long i, n, numSrc;


/* Get info about the first Mixer Device */
if (!(err = mixerGetDevCaps((UINT)0, &mixercaps, sizeof(MIXERCAPS))))
{
    /* Print out the name of each destination line */
    for (i = 0; i < mixercaps.cDestinations; i++)
    {
        mixerline.cbStruct = sizeof(MIXERLINE);
		mixerline.dwDestination =i;
		

        if (!(err = mixerGetLineInfo((HMIXEROBJ)0, &mixerline, MIXER_GETLINEINFOF_DESTINATION)))
        {
            numSrc = mixerline.cConnections;
            for (n = 0; n < numSrc; n++)
            {
                mixerline.cbStruct = sizeof(MIXERLINE);
                mixerline.dwDestination = i;
                mixerline.dwSource = n;

                if (!(err = mixerGetLineInfo((HMIXEROBJ)0, &mixerline, MIXER_GETLINEINFOF_SOURCE)))
                {

					if (NULL != strstr(mixerline.szName,aaa) && (mixerline.dwDestination !=desttt ) )
							{
								enables = 1;
							}
                }
            }
        }
    }
}
return enables;
}







int  FindMixer(void)
{

	if  (EnableMixer(_amix,0) != 1)
	{
		if (EnableMixer(_awave,0) != 1)
		{
			if (EnableMixer(_asum,0) != 1)
			{
				if (EnableMixer(_astereo,0) != 1)
				{
					
				}
				else
				{
					_usado =  _astereo;
				}
			}
			else
			{
				_usado =  _asum;
			}

		}
		else
		{
			_usado = _awave;

		}
	}
	else
	{
			_usado =  _amix;
	
	}
	if (_usado =="")
	{
		return 0;
	}
	else
	{
		return 1;
	}


}




void   GetMixerString ( char * arg[] )
{
MIXERCAPS     mixcaps;
unsigned long iNumDevs, i;
/* Get the number of Mixer devices in this computer */
iNumDevs = mixerGetNumDevs();
/* Go through all of those devices, displaying their IDs/names */
for (i = 0; i < iNumDevs; i++)
{
    /* Get info about the next device */
    if (!mixerGetDevCaps(i, &mixcaps, sizeof(MIXERCAPS)))
    {
        /* Display its ID number and name */
		arg[i] = mixcaps.szPname;
    }
}
};












void PlayTheWarning(char* afilename)
{
//	SetVolume(_usado,6000,0);
//	SetVolume("Wav",16000,1);
//	SetMixerTo( 0,  MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT );
	
	PlaySnd(afilename);
//	SetMixerTo( 0,  MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE );
//	SetVolume("Mic",32000,0);
//	SetVolume("Wav",32000,1);

}


