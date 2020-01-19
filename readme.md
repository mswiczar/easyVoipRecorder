Initial Commit. of Windows Softphone Recorder.

After 15 years of the initial release. I made it public. 
This is my implementation of Windows Easy Voip Recorder.  (Windows soft phone Recorder)

The Main goal of this app is to record Voip Conversations in real time without the need of any driver to achieve it.
All the code works  inside the  win32 User Mode Memory.
Nothing is running in the Ring 0. Space. There is no need of  any driver.
Small amount of  memory usage.

The Aplication was made in c, c++ and inline asm



Fetaures:
Its support all the major  windows softphone applications like:
Skype
X-lite
Cisco Ip Communicator
GoogleTalk
VoipCheap.com
VoipStunt
12Voip
VoipBuster
Globe7
YahooMessenger
VoipWise
Net2Phone CommCenter
VoipRaider
JustVoip
VoipDiscount
LowRateVoip
FreeCall
nonoh
VoipbusterPro
SipDiscount
Internetcalls
WebCallDirect
PoivY
Msn Messenger
Olympic
XS4 All
IConnectHere
Rynga
SmartVoip

How any voip application works?

When you have a voip conversation there are 2 channels of sound.

1) The Mic Input.  ( your voice)
2) The Speaker Output. ( your caller voice)

The work done by the Voip Recorder is to get the 2 raw of data (input and output sound)  and merge them into one stereo file. 
Storing each party in a different channel.
Encoding it to ogg format , and store it in the local file system. 

How the recorder perform this Task?

To get the 2 raw of data. the voip patch the windows system calls of the softphone  (like skype) and store the data inside  a  memory List. 
Then every 4 seconds the List is merged, encoded  and flushed to the local hard disk drive, to use a minimal amount of memory.
Finally  when the call stops or the user stops the record. The application  add the Header of the ogg file and store it in the file system.
Available to any ogg player to use it.


How the Recorder works?

The application works in the voip instance  (the voip process memmory area) No in the recorder program. 
The Recorder inject the code stored in a dll into the host  app.  (The  Voip App like Skype). It works like a charm in mostly all of the softphone implementation becuse its use the windows api calls to perform the job.

Then the Recorder send system messages to the host app, to interact with the VOIP Softphone.


Now lets see in more detail the sorces.

DllWasaPi ->The DLL (the recorder main code)  
Interface  ->  The User Interface to record. stop. play any softphone app.
Service -> The Service that runs and patch all the softphones availables in the system.
Setup -> NSIS (Nullsoft Scriptable Install System) is a professional open source system to create Windows installers. It is designed to be as small and flexible as possible and is therefore very suitable for internet distribution.

TestVistaWasapi -> Unit Test fort the Api
MonitorVista -> Unit Test for the Monitor


If you need more info just make me know 
mswiczar [at] gmail [dot] com


















