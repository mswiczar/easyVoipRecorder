;--------------------------------
; The name of the installer
Name "easyVoipRecorder"

; The file to write
OutFile "easyVoipRecorderSetup48.exe"

Icon .\icon.ico
UninstallIcon .\Uninstall.ico

; The default installation directory
InstallDir $PROGRAMFILES\easyVoipRecorder


LicenseData .\eula.txt
LicenseBkColor /windows

XPStyle on
;--------------------------------

; Pages
Page license
Page directory
Page instfiles

;--------------------------------
!macro RegisterCOM file
    ExecWait '$SYSDIR\regsvr32.exe "/s" "${file}"'
!macroend

!macro UnRegisterCOM file
    ExecWait '$SYSDIR\regsvr32.exe "/u" "/s" "${file}"'
!macroend

!macro WMPRegisterType typeName description

  WriteRegStr HKCR "WMP.${typeName}" "" "${description}"
  WriteRegStr HKCR "WMP.${typeName}\shell" "" "open"

  WriteRegStr HKCR "WMP.${typeName}\shell\open" "" "&Open"
  WriteRegStr HKCR "WMP.${typeName}\shell\open\command" "" "$WMP_LOCATION /Open $\"%L$\""
  
  WriteRegStr HKCR "WMP.${typeName}\shell\play" "" "&Play"
  WriteRegStr HKCR "WMP.${typeName}\shell\play\command" "" "$WMP_LOCATION /Play $\"%L$\""    

  ; WMP extra integration
  WriteRegStr HKCR "WMP.${typeName}\shellex\ContextMenuHandlers\WMPAddToPlaylist" "" "{F1B9284F-E9DC-4e68-9D7E-42362A59F0FD}"
  WriteRegStr HKCR "WMP.${typeName}\shellex\ContextMenuHandlers\WMPPlayAsPlaylist" "" "{CE3FB1D1-02AE-4a5f-A6E9-D9F1B4073E6C}"

  WriteRegStr HKCR "WMP.${typeName}\DefaultIcon" "" "$INSTDIR\xifish.ico"

!macroend



; The stuff to install
Section "" ;No components page, name is not important

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  WriteUninstaller $INSTDIR\uninstaller.exe

  SetOutPath $SYSDIR
	File .\Bin\easyVoipRecorder.dll
	File .\Bin\ogg.dll
	File .\Bin\vorbis.dll
	File .\Bin\lame.exe

  
	

  SetOutPath $INSTDIR
	File .\Bin\easyVoipRecorder.exe
	File .\Bin\EasyVoipRecorderUpload.exe
	File .\Bin\easyVoipRecorderMonitor.exe
	File .\Bin\easyVoipRecorder.ini
  
  CreateDirectory "$INSTDIR\utils"
  
  SetOutPath $INSTDIR\utils
	File .\Bin\oggdec.exe
	File .\Bin\oggenc.exe
	File .\Bin\speexenc.exe
	File .\Bin\speexdec.exe
  
  SetOutPath $INSTDIR
  
  
  CreateDirectory "$DOCUMENTS\easyVoipRecorder"
  CreateDirectory "$DOCUMENTS\easyVoipRecorder\Recordings"
  CreateDirectory "$DOCUMENTS\easyVoipRecorder\Exports"
  CreateDirectory "$DOCUMENTS\easyVoipRecorder\Database"
  
  CreateDirectory "$SMPROGRAMS\easyVoipRecorder"					
  CreateShortCut "$SMPROGRAMS\easyVoipRecorder\easyVoipRecorder.lnk" "$INSTDIR\easyVoipRecorder.exe"
  CreateShortCut "$SMPROGRAMS\easyVoipRecorder\easyVoipRecorderMonitor.lnk" "$INSTDIR\easyVoipRecorderMonitor.exe"
  CreateShortCut "$DESKTOP\easyVoipRecorder.lnk" "$INSTDIR\easyVoipRecorder.exe"

  
  WriteRegStr HKCU Software\Microsoft\Windows\CurrentVersion\Run easyVoipRecorder "$INSTDIR\easyVoipRecorder.exe /Minimize"
  WriteRegStr HKCU Software\Microsoft\Windows\CurrentVersion\Run easyVoipRecorderMonitor "$INSTDIR\easyVoipRecorderMonitor.exe /Minimize"
  WriteRegStr HKCU Software\easyVoipRecorder\ InstallDir "$INSTDIR\"
  WriteRegStr HKCU Software\easyVoipRecorder\ DestinationFolder "$DOCUMENTS\easyVoipRecorder\Recordings\"
  WriteRegStr HKCU Software\easyVoipRecorder\ ExportFolder "$DOCUMENTS\easyVoipRecorder\Exports\"
  WriteRegStr HKCU Software\easyVoipRecorder\ DBFolder "$DOCUMENTS\easyVoipRecorder\"
  WriteRegStr HKCU Software\easyVoipRecorder\ utilsFolder "$INSTDIR\utils\"
  WriteRegStr HKCU Software\easyVoipRecorder\ pingued ""


  WriteRegDWORD HKCU Software\easyVoipRecorder\ AutoRecord 0x00000000
  WriteRegDWORD HKCU Software\easyVoipRecorder\ AutoUpload 0x00000000
  WriteRegDWORD HKCU Software\easyVoipRecorder\ gauge 0x00000000
  WriteRegDWORD HKCU Software\easyVoipRecorder\ playlist 0x00000001
  WriteRegDWORD HKCU Software\easyVoipRecorder\ history 0x00000000
  WriteRegDWORD HKCU Software\easyVoipRecorder\ opacity 0x00000064

  WriteRegStr HKCU Software\easyVoipRecorder\ version "4.8"


  WriteRegDWORD HKCU Software\easyVoipRecorder\RecordingOptions BITRATE 0x00000001
  WriteRegDWORD HKCU Software\easyVoipRecorder\RecordingOptions FORMAT 0x00000001
  WriteRegDWORD HKCU Software\easyVoipRecorder\RecordingOptions KHZ 0x00000001

  WriteRegStr HKCU Software\easyVoipRecorder\UploadOptions DOMAINNAME ""
  WriteRegStr HKCU Software\easyVoipRecorder\UploadOptions KEYCODE ""
  WriteRegStr HKCU Software\easyVoipRecorder\UploadOptions PASSWORD ""
  WriteRegStr HKCU Software\easyVoipRecorder\UploadOptions USERNAME ""
  WriteRegDWORD HKCU Software\easyVoipRecorder\UploadOptions METHOD 0x00000000

  WriteRegStr HKCU Software\easyVoipRecorder\Registration email ""
  WriteRegStr HKCU Software\easyVoipRecorder\Registration RegCode ""
  WriteRegStr HKCU Software\easyVoipRecorder\Registration Code ""


  WriteRegStr HKCU Software\easyVoipRecorder\WASAPI IN_CHANNELS 0x00000000
  WriteRegStr HKCU Software\easyVoipRecorder\WASAPI IN_HZ 0x00000000
  WriteRegStr HKCU Software\easyVoipRecorder\WASAPI IN_POS 0x00000000

  WriteRegStr HKCU Software\easyVoipRecorder\WASAPI OUT_CHANNELS 0x00000000
  WriteRegStr HKCU Software\easyVoipRecorder\WASAPI OUT_HZ 0x00000000
  WriteRegStr HKCU Software\easyVoipRecorder\WASAPI OUT_POS 0x00000000

  WriteRegStr HKCU Software\easyVoipRecorder\WASAPI BUS_HZ 0x00000000



WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\easyVoipRecorder" "DisplayName" "easyVoipRecorder (remove only)"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\easyVoipRecorder" "UninstallString" '"$INSTDIR\uninstaller.exe"'


SetOutPath $INSTDIR
  
 ; Runtime libraries from visual studio - 2
  File ".\filters\msvcp80.dll"
  File ".\filters\msvcr80.dll"
  File ".\filters\Microsoft.VC80.CRT.manifest"


  ; Libraries - 10
  File ".\filters\libOOOgg.dll"
  File ".\filters\libOOOggSeek.dll"
  File ".\filters\libCMMLTags.dll"
  File ".\filters\libCMMLParse.dll"
  File ".\filters\vorbis.dll"
 
  File ".\filters\libOOTheora.dll"
  File ".\filters\libFLAC.dll"
  File ".\filters\libFLAC++.dll"
  File ".\filters\libVorbisComment.dll"

  File ".\filters\libTemporalURI.dll"


  File ".\filters\Ogg Codecs.manifest" 
  
  ; Install Filters - 16  

  File ".\filters\dsfFLACEncoder.dll"
  File ".\filters\dsfSpeexEncoder.dll"
  File ".\filters\dsfTheoraEncoder.dll"
  File ".\filters\dsfVorbisEncoder.dll"

  File ".\filters\dsfNativeFLACSource.dll"
  File ".\filters\dsfSpeexDecoder.dll"
  File ".\filters\dsfTheoraDecoder.dll"
  File ".\filters\dsfFLACDecoder.dll"
  File ".\filters\dsfVorbisDecoder.dll"

  File ".\filters\dsfOGMDecoder.dll"

  File ".\filters\dsfOggDemux2.dll"
  File ".\filters\dsfOggMux.dll"

  ; File "bin\dsfSeeking.dll"

  File ".\filters\dsfCMMLDecoder.dll"
  File ".\filters\dsfCMMLRawSource.dll"
  File ".\filters\dsfSubtitleVMR9.dll"

  ; File "bin\dsfAnxDemux.dll"
  File ".\filters\dsfAnxMux.dll"                                           


  SetOutPath "$INSTDIR"
  ; Register libraries - 16

  !insertmacro RegisterCOM "$INSTDIR\dsfFLACEncoder.dll"
  !insertmacro RegisterCOM "$INSTDIR\dsfSpeexEncoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfTheoraEncoder.dll"
  !insertmacro RegisterCOM "$INSTDIR\dsfVorbisEncoder.dll" 
  
  !insertmacro RegisterCOM "$INSTDIR\dsfNativeFLACSource.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfSpeexDecoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfTheoraDecoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfFLACDecoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfVorbisDecoder.dll" 
  
  !insertmacro RegisterCOM "$INSTDIR\dsfOGMDecoder.dll" 
  
  !insertmacro RegisterCOM "$INSTDIR\dsfOggDemux2.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfOggMux.dll" 
  
  !insertmacro RegisterCOM "$INSTDIR\dsfCMMLDecoder.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfCMMLRawSource.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfSubtitleVMR9.dll" 
  
  ;!insertmacro RegisterCOM "$INSTDIR\dsfAnxDemux.dll" 
  !insertmacro RegisterCOM "$INSTDIR\dsfAnxMux.dll"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Registry Entries for directshow and WMP
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	*	Media Group Entries for WMP
;;;			-	flac (audio)
;;;			-	oga
;;;			-	ogv
;;;			-	axa
;;;			-	axv
;;;			-	spx
;;;			-	ogm(????? TODO:::)
;;;			-	ogg(TODO::: Check if can have no group)
;;;	*	Mime Type Entries for WMP
;;;	*	Extension Entries for WMP - TODO::: Other entries, icons
;;;	*	Media Type Entries/Filter association for Directshow
;;;	*	MLS(?) Entries for WMP






;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; Media Group Entries - 6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\FLAC" "" "FLAC File (flac)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\FLAC" "Extensions" ".flac"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\FLAC" "MIME Types" "audio/x-flac"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGA" "" "Ogg Audio File (oga)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGA" "Extensions" ".oga"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGA" "MIME Types" "audio/x-ogg"
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\OGV" "" "Ogg Video File (ogv)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\OGV" "Extensions" ".ogv"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\OGV" "MIME Types" "video/x-ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\AXA" "" "Annodex Audio File (axa)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\AXA" "Extensions" ".axa"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\AXA" "MIME Types" "audio/x-annodex"
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\AXV" "" "Annodex Video File (axv)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\AXV" "Extensions" ".axv"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Video\AXV" "MIME Types" "video/x-annodex"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\SPX" "" "Speex File (spx)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\SPX" "Extensions" ".spx"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\SPX" "MIME Types" "audio/x-ogg"
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	WMP Mime type entries - 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;





  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "" "Annodex File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "Extension.Key" ".anx"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "Extensions.CommaSep" "anx,axa,axv"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/x-annodex" "Extensions.SpaceSep" ".anx .axa .axv"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "" "Ogg File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "Extension.Key" ".ogg"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "Extensions.CommaSep" "ogg,oga,ogv,spx"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\application/ogg" "Extensions.SpaceSep" ".ogg .oga .ogv .spx"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-flac" "" "FLAC Audio File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-flac" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-flac" "Extension.Key" ".flac"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "" "Ogg Audio File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "Extension.Key" ".oga"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "Extensions.CommaSep" "oga,spx"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-ogg" "Extensions.SpaceSep" ".oga .spx"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-ogg" "" "Ogg Video File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-ogg" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-ogg" "Extension.Key" ".ogv"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "" "Annodex Audio File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "Extension.Key" ".axa"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "Extensions.CommaSep" "axa"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\audio/x-annodex" "Extensions.SpaceSep" ".axa"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-annodex" "" "Annodex Video File"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-annodex" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\MIME Types\video/x-annodex" "Extension.Key" ".axv"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	WMP extension entries - 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "MediaType.Description" "Annodex"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.anx" "Extension.MIME" "application/x-annodex"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "MediaType.Description" "Annodex Audio"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "PerceivedType" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axa" "Extension.MIME" "audio/x-annodex"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "MediaType.Description" "Annodex Video"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "PerceivedType" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.axv" "Extension.MIME" "video/x-annodex"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "MediaType.Description" "FLAC Audio"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "PerceivedType" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.flac" "Extension.MIME" "audio/x-flac"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "MediaType.Description" "Ogg Audio"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "PerceivedType" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.oga" "Extension.MIME" "audio/x-ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "MediaType.Description" "Ogg"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "Extension.MIME" "application/ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "MediaType.Description" "Ogg Video"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "PerceivedType" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogv" "Extension.MIME" "video/x-ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "AlreadyRegistered" "yes"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "MediaType.Description" "Speex Audio"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "Permissions" 0x0000000f
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "Runtime" 0x00000007
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "PerceivedType" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.spx" "Extension.MIME" "audio/x-ogg"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	Directshow extension to filter mapping - 8
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.anx" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.axa" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.axv" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.flac" "Source Filter" "{6DDA37BA-0553-499a-AE0D-BEBA67204548}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.oga" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.ogg" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.ogv" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


  WriteRegStr HKCR "Media Type\Extensions\.spx" "Source Filter" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	Directshow extension to filter mapping for HTTP - 7
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKCR "http\Extensions" ".OGG" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".OGV" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".OGA" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".SPX" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".ANX" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".AXV" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  WriteRegStr HKCR "http\Extensions" ".AXA" "{C9361F5A-3282-4944-9899-6D99CDC5370B}"
  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;	MLS Perceived type - 6
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "ogv" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "oga" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "axv" "video"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "axa" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "spx" "audio"
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "flac" "audio"  



 Var /GLOBAL WMP_LOCATION  
  
  StrCpy $WMP_LOCATION "$PROGRAMFILES\Windows Media Player\wmplayer.exe"

  
  ; Make .ogg recognised as audio
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGG" "" "Ogg File (ogg)"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGG" "Extensions" ".ogg"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Groups\Audio\OGG" "MIME Types" "application/ogg"  
    
  WriteRegStr HKLM "SOFTWARE\Microsoft\Multimedia\WMPlayer\Extensions\.ogg" "PerceivedType" "audio"
  
  WriteRegStr HKLM "SOFTWARE\Microsoft\MediaPlayer\MLS\Extensions" "ogg" "audio"  


  WriteRegStr HKCR ".ogg" "" "WMP.OggFile"
  !insertmacro WMPRegisterType "OggFile" "Ogg File"

  
SectionEnd ; end the section



Section "Uninstall"
  DeleteRegValue HKCU Software\Microsoft\Windows\CurrentVersion\Run easyVoipRecorder
  DeleteRegValue HKCU Software\Microsoft\Windows\CurrentVersion\Run easyVoipRecorderMonitor
  DeleteRegValue HKCU Software\easyVoipRecorder\ InstallDir
  DeleteRegValue HKCU Software\easyVoipRecorder\ DestinationFolder
  DeleteRegValue HKCU Software\easyVoipRecorder\ ExportFolder
  DeleteRegValue HKCU Software\easyVoipRecorder\ AutoRecord 
  DeleteRegValue HKCU Software\easyVoipRecorder\ AutoUpload
  DeleteRegValue HKCU Software\easyVoipRecorder\ gauge 
  DeleteRegValue HKCU Software\easyVoipRecorder\ playlist 
  DeleteRegValue HKCU Software\easyVoipRecorder\ history 
  DeleteRegValue HKCU Software\easyVoipRecorder\ opacity 
  DeleteRegValue HKCU Software\easyVoipRecorder\ utilsFolder 
  DeleteRegValue HKCU Software\easyVoipRecorder\ pingued


  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\easyVoipRecorder"
	
  DeleteRegKey HKLM Software\easyVoipRecorder\ 

  
  KillProcDLL::KillProc "easyVoipRecorderMonitor.exe"
  KillProcDLL::KillProc "easyVoipRecorder.exe"


  !insertmacro UnRegisterCOM "$INSTDIR\dsfOggDemux2.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfOggMux.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfVorbisDecoder.dll"
  !insertmacro UnRegisterCOM "$INSTDIR\dsfOGMDecoder.dll"

  
  
  Delete $INSTDIR\Uninstaller.exe ; delete self (see explanation below why this works)
  Delete $INSTDIR\utils\oggdec.exe
  Delete $INSTDIR\utils\oggenc.exe
  Delete $INSTDIR\utils\speexdec.exe
  Delete $INSTDIR\utils\speexenc.exe
  Delete $INSTDIR\easyVoipRecorder.exe
  Delete $INSTDIR\EasyVoipRecorderUpload.exe
  Delete $SYSDIR\easyVoipRecorder.dll
  Delete $INSTDIR\easyVoipRecorderMonitor.exe
  Delete $INSTDIR\easyVoipRecorder.ini
  Delete "$SMPROGRAMS\easyVoipRecorder"
  Delete "$SMPROGRAMS\easyVoipRecorder\easyVoipRecorder.lnk"
  Delete "$SMPROGRAMS\easyVoipRecorder\easyVoipRecorderMonitor.lnk"
  Delete "$DESKTOP\easyVoipRecorder.lnk"
  Delete "$SMPROGRAMS\easyVoipRecorder\Uninstall.lnk" 
  RMDir "$SMPROGRAMS\easyVoipRecorder"
  RMDir "$INSTDIR\utils"
SectionEnd
