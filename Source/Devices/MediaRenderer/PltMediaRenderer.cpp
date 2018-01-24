/*****************************************************************
|
|   Platinum - AV Media Renderer Device
|
| Copyright (c) 2004-2010, Plutinosoft, LLC.
| All rights reserved.
| http://www.plutinosoft.com
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or (at your option) any later version.
|
| OEMs, ISVs, VARs and other distributors that combine and
| distribute commercially licensed software with Platinum software
| and do not wish to distribute the source code for the commercially
| licensed software under version 2, or (at your option) any later
| version, of the GNU General Public License (the "GPL") must enter
| into a commercial license agreement with Plutinosoft, LLC.
| licensing@plutinosoft.com
|
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License
| along with this program; see the file LICENSE.txt. If not, write to
| the Free Software Foundation, Inc.,
| 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
| http://www.gnu.org/licenses/gpl-2.0.html
|
****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Neptune.h"
#include "PltMediaRenderer.h"
#include "PltService.h"

NPT_SET_LOCAL_LOGGER("platinum.media.renderer")

#ifdef ANDROID
#include <android/log.h>
#define TAG "Platinum-jni" 
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG ,__VA_ARGS__)
#define LOGI2(...) LOGI(__VA_ARGS__)
#define LOGI3(...) LOGI(__VA_ARGS__)
#else
#define LOGI(s) NPT_LOG_INFO(s)
#define LOGI2(fmt, arg1) NPT_LOG_INFO_1(fmt, arg1)
#define LOGI3(fmt, arg1, arg2) NPT_LOG_INFO_2(fmt, arg1, arg2)
#endif

/*----------------------------------------------------------------------
|   external references
+---------------------------------------------------------------------*/
extern NPT_UInt8 RDR_ConnectionManagerSCPD[];
extern NPT_UInt8 RDR_AVTransportSCPD[];
extern NPT_UInt8 RDR_RenderingControlSCPD[];

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::PLT_MediaRenderer
+---------------------------------------------------------------------*/
PLT_MediaRenderer::PLT_MediaRenderer(const char*  friendly_name,
									 bool         show_ip     /* = false */,
									 const char*  uuid        /* = NULL */,
									 unsigned int port        /* = 0 */,
									 bool         port_rebind /* = false */) :
	PLT_DeviceHost("/",
				   uuid,
				   "urn:schemas-upnp-org:device:MediaRenderer:1",
				   friendly_name,
				   show_ip,
				   port,
				   port_rebind),
	m_Delegate(NULL)
{
	m_Manufacturer     = "ABao";
	m_ManufacturerURL     = "https://github.com/talent518";
	m_ModelDescription = "ABao Dlna Media Renderer";
	m_ModelName        = "ABao Dlna Media Renderer";
	m_ModelURL         = "https://github.com/talent518/MediaRender";
	m_DlnaDoc          = "DMR-1.50";
	m_ModelNumber      = "1.0";
	m_SerialNumber     = "20180123130516";
	m_PresentationURL  = "https://github.com/talent518";
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::~PLT_MediaRenderer
+---------------------------------------------------------------------*/
PLT_MediaRenderer::~PLT_MediaRenderer()
{
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::SetupServices
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::SetupServices()
{
	NPT_Reference<PLT_Service> service;

	{
		/* AVTransport */
		service = new PLT_Service(
			this,
			"urn:schemas-upnp-org:service:AVTransport:1",
			"urn:upnp-org:serviceId:AVTransport",
			"AVTransport",
			"urn:schemas-upnp-org:metadata-1-0/AVT/");
		NPT_CHECK_FATAL(service->SetSCPDXML((const char*) RDR_AVTransportSCPD));
		NPT_CHECK_FATAL(AddService(service.AsPointer()));

		service->SetStateVariableRate("LastChange", NPT_TimeInterval(0.2f));
		service->SetStateVariable("A_ARG_TYPE_InstanceID", "0");

		// GetCurrentTransportActions
		service->SetStateVariable("CurrentTransportActions", "Play,Pause,Stop,Seek,Next,Previous");

		// GetDeviceCapabilities
		service->SetStateVariable("PossiblePlaybackStorageMedia", "NONE,NETWORK,HDD,CD-DA,UNKNOWN");
		service->SetStateVariable("PossibleRecordStorageMedia", "NOT_IMPLEMENTED");
		service->SetStateVariable("PossibleRecordQualityModes", "NOT_IMPLEMENTED");

		// GetMediaInfo
		service->SetStateVariable("NumberOfTracks", "0");
		service->SetStateVariable("CurrentMediaDuration", "00:00:00");
		service->SetStateVariable("AVTransportURI", "");
		service->SetStateVariable("AVTransportURIMetadata", "");;
		service->SetStateVariable("NextAVTransportURI", "NOT_IMPLEMENTED");
		service->SetStateVariable("NextAVTransportURIMetadata", "NOT_IMPLEMENTED");
		service->SetStateVariable("PlaybackStorageMedium", "NONE");
		service->SetStateVariable("RecordStorageMedium", "NOT_IMPLEMENTED");
		service->SetStateVariable("RecordMediumWriteStatus", "NOT_IMPLEMENTED");

		// GetPositionInfo
		service->SetStateVariable("CurrentTrack", "0");
		service->SetStateVariable("CurrentTrackDuration", "00:00:00");
		service->SetStateVariable("CurrentTrackMetadata", "");
		service->SetStateVariable("CurrentTrackURI", "");
		service->SetStateVariable("RelativeTimePosition", "00:00:00");
		service->SetStateVariable("AbsoluteTimePosition", "00:00:00");
		service->SetStateVariable("RelativeCounterPosition", "2147483647"); // means NOT_IMPLEMENTED
		service->SetStateVariable("AbsoluteCounterPosition", "2147483647"); // means NOT_IMPLEMENTED

		// disable indirect eventing for certain state variables
		PLT_StateVariable* var;
		var = service->FindStateVariable("RelativeTimePosition");
		if (var) var->DisableIndirectEventing();
		var = service->FindStateVariable("AbsoluteTimePosition");
		if (var) var->DisableIndirectEventing();
		var = service->FindStateVariable("RelativeCounterPosition");
		if (var) var->DisableIndirectEventing();
		var = service->FindStateVariable("AbsoluteCounterPosition");
		if (var) var->DisableIndirectEventing();

		// GetTransportInfo
		service->SetStateVariable("TransportState", "NO_MEDIA_PRESENT");
		service->SetStateVariable("TransportStatus", "OK");
		service->SetStateVariable("TransportPlaySpeed", "1");

		// GetTransportSettings
		service->SetStateVariable("CurrentPlayMode", "NORMAL");
		service->SetStateVariable("CurrentRecordQualityMode", "NOT_IMPLEMENTED");

		service.Detach();
		service = NULL;
	}

	{
		/* ConnectionManager */
		service = new PLT_Service(
			this,
			"urn:schemas-upnp-org:service:ConnectionManager:1",
			"urn:upnp-org:serviceId:ConnectionManager",
			"ConnectionManager");
		NPT_CHECK_FATAL(service->SetSCPDXML((const char*) RDR_ConnectionManagerSCPD));
		NPT_CHECK_FATAL(AddService(service.AsPointer()));

		service->SetStateVariable("CurrentConnectionIDs", "0");

		// put all supported mime types here instead
		service->SetStateVariable("SinkProtocolInfo", "http-get:*:image/x-ycbcr-yuv420:*,http-get:*:image/x-xpixmap:*,http-get:*:image/x-xfig:*,http-get:*:image/x-xbm:*,http-get:*:image/x-xbitmap:*,http-get:*:image/x-wmf:*,http-get:*:image/x-windows-bmp:*,http-get:*:image/x-rgb:*,http-get:*:image/x-quicktime:*,http-get:*:image/x-psd:*,http-get:*:image/x-portable-pixmap:*,http-get:*:image/x-portable-graymap:*,http-get:*:image/x-portable-bitmap:*,http-get:*:image/x-portable-anymap:*,http-get:*:image/x-png:*,http-get:*:image/x-pict:*,http-get:*:image/x-photoshop:*,http-get:*:image/x-pcx:*,http-get:*:image/x-ms-bmp:*,http-get:*:image/x-jg:*,http-get:*:image/x-icon:*,http-get:*:image/xicon:*,http-get:*:image/x-ico:*,http-get:*:image/x-guffaw:*,http-get:*:image/x-eps:*,http-get:*:image/x-emf:*,http-get:*:image/x.djvu:*,http-get:*:image/x-djvu:*,http-get:*:image/x-dcraw:*,http-get:*:image/x-citrix-pjpeg:*,http-get:*:image/x-bmp:*,http-get:*:image/x-bitmap:*,http-get:*:image/vnd.wap.wbmp:*,http-get:*:image/vnd.ms-photo:*,http-get:*:image/vnd.ms-modi:*,http-get:*:image/vnd.microsoft.icon:*,http-get:*:image/vnd.dxf:*,http-get:*:image/vnd.dwg:*,http-get:*:image/vnd.djvu:*,http-get:*:image/vnd.adobe.photoshop:*,http-get:*:image/tiff:*,http-get:*:image/svg+xml:*,http-get:*:image/png:*,http-get:*:image/pjpeg:*,http-get:*:image/pict:*,http-get:*:image/pdf:*,http-get:*:image/jpg:*,http-get:*:image/jpeg-cmyk:*,http-get:*:image/jpeg:*,http-get:*:image/jp2:*,http-get:*:image/icon:*,http-get:*:image/ico:*,http-get:*:image/GIF:*,http-get:*:image/gif:*,http-get:*:image/fits:*,http-get:*:image/cur:*,http-get:*:image/bmp:*,http-get:*:image/bitmap:*,http-get:*:application/x-shockwave-flash:*,http-get:*:video/m3u8:*,http-get:*:video/ogm:*,http-get:*:video/hlv:*,http-get:*:video/wtv:*,http-get:*:video/x-rmvb:*,http-get:*:video/rmvb:*,http-get:*:video/x-rm:*,http-get:*:video/rm:*,http-get:*:video/x-nerodigital-ps:*,http-get:*:video/wt:*,http-get:*:video/x-matroska:*,http-get:*:video/mkv:*,http-get:*:video/x-mkv:*,http-get:*:video/x-ms-avi:*,http-get:*:video/x-xvid:*,http-get:*:video/xvid:*,http-get:*:video/x-divx:*,http-get:*:video/divx:*,http-get:*:video/x-motion-jpeg:*,http-get:*:video/vnd.dlna.mpeg-tts:*,http-get:*:video/x-swf:*,http-get:*:video/x-sgi-movie:*,http-get:*:video/x-ms-video:*,http-get:*:video/x-pn-realvideo:*,http-get:*:video/x-pn-realaudio:*,http-get:*:video/x-ms-wvx:*,http-get:*:video/x-ms-wmx:*,http-get:*:video/x-ms-wmv:*,http-get:*:video/x-ms-wma:*,http-get:*:video/x-ms-wm:*,http-get:*:video/x-msvideo:*,http-get:*:video/x-ms-asx:*,http-get:*:video/x-ms-asf:*,http-get:*:video/mp2p:*,http-get:*:video/MP2T:*,http-get:*:video/mpeg2:*,http-get:*:video/x-mpeg:*,http-get:*:video/x-mp4:*,http-get:*:video/x-m4v:*,http-get:*:video/x-flv:*,http-get:*:video/x-dv:*,http-get:*:video/wmv:*,http-get:*:video/webm:*,http-get:*:video/vnd.objectvideo:*,http-get:*:video/unknown:*,http-get:*:video/swf:*,http-get:*:video/quicktime:*,http-get:*:video/msvideo:*,http-get:*:video/mpg4:*,http-get:*:video/mpeg4:*,http-get:*:video/mpeg3:*,http-get:*:video/mpeg:*,http-get:*:video/mp4v-es:*,http-get:*:video/mp4:*,http-get:*:video/m4v:*,http-get:*:video/flv:*,http-get:*:video/f4v:*,http-get:*:video/avi:*,http-get:*:video/asx:*,http-get:*:video/3gpp2:*,http-get:*:video/3gpp:*,http-get:*:video/ape:*,http-get:*:video/aiff:*,http-get:*:video/ra:*,http-get:*:video/flac:*,http-get:*:video/ac3:*,http-get:*:video/aac:*,http-get:*:video/ogg:*,http-get:*:video/m4a:*,http-get:*:video/wav:*,http-get:*:video/asf:*,http-get:*:video/wma:*,http-get:*:video/mp3:*,http-get:*:audio/ape:*,http-get:*:audio/x-asf-pf:*,http-get:*:audio/wma:*,http-get:*:audio/x-wav:*,http-get:*:audio/vorbis:*,http-get:*:audio/x-scpls:*,http-get:*:audio/x-ra:*,http-get:*:audio/ra:*,http-get:*:audio/x-realaudio:*,http-get:*:audio/x-pn-realaudio-plugin:*,http-get:*:audio/x-pn-realaudio:*,http-get:*:audio/x-ms-wmv:*,http-get:*:audio/x-ms-wma:*,http-get:*:audio/x-ms-wax:*,http-get:*:audio/x-mpeg-url:*,http-get:*:audio/x-mpeg3:*,http-get:*:audio/x-mp3:*,http-get:*:audio/x-midi:*,http-get:*:audio/x-m4a:*,http-get:*:audio/x-flac:*,http-get:*:audio/flac:*,http-get:*:audio/x-ac3:*,http-get:*:audio/ac3:*,http-get:*:audio/x-aac:*,http-get:*:audio/aac:*,http-get:*:audio/x-aiff:*,http-get:*:audio/wave:*,http-get:*:audio/wav:*,http-get:*:audio/vnd.rn-realaudio:*,http-get:*:audio/vnd.qcelp:*,http-get:*:audio/vnd.dlna.adts:*,http-get:*:audio/unknown:*,http-get:*:audio/playlist:*,http-get:*:audio/x-ogg:*,http-get:*:audio/ogg:*,http-get:*:audio/mpg:*,http-get:*:audio/mpeg-url:*,http-get:*:audio/mpeg3:*,http-get:*:audio/mpeg2:*,http-get:*:audio/x-mpeg:*,http-get:*:audio/mpeg:*,http-get:*:audio/mp4a-latm:*,http-get:*:audio/mp4:*,http-get:*:audio/mp3:*,http-get:*:audio/mp2:*,http-get:*:audio/mp1:*,http-get:*:audio/x-dts:*,http-get:*:audio/midi:*,http-get:*:audio/mid:*,http-get:*:audio/m4a:*,http-get:*:audio/x-atrac3:*,http-get:*:audio/basic:*,http-get:*:audio/asf:*,http-get:*:audio/aiff:*,http-get:*:audio/L16:*,http-get:*:audio/L8:*,http-get:*:audio/L16:DLNA.ORG_PN=LPCM,http-get:*:audio/L16:DLNA.ORG_PN=LPCM_low,http-get:*:audio/L16;rate=44100;channels=1:DLNA.ORG_PN=LPCM,http-get:*:audio/L16;rate=44100;channels=2:DLNA.ORG_PN=LPCM,http-get:*:audio/L16;rate=48000;channels=2:DLNA.ORG_PN=LPCM,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3,http-get:*:audio/mpeg:DLNA.ORG_PN=MP3X,http-get:*:audio/vnd.dlna.adts:DLNA.ORG_PN=AAC_ADTS,http-get:*:audio/vnd.dlna.adts:DLNA.ORG_PN=AAC_ADTS_192,http-get:*:audio/vnd.dlna.adts:DLNA.ORG_PN=AAC_ADTS_320,http-get:*:audio/vnd.dlna.adts:DLNA.ORG_PN=AAC_MULT5_ADTS,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO,http-get:*:audio/3gpp:DLNA.ORG_PN=AAC_ISO,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO_192,http-get:*:audio/3gpp:DLNA.ORG_PN=AAC_ISO_192,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_ISO_320,http-get:*:audio/3gpp:DLNA.ORG_PN=AAC_ISO_320,http-get:*:audio/mp4:DLNA.ORG_PN=AAC_MULT5_ISO,http-get:*:audio/3gpp:DLNA.ORG_PN=AAC_MULT5_ISO,http-get:*:audio/mp4:DLNA.ORG_PN=HEAACv2_L2,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAACv2_L2,http-get:*:audio/mp4:DLNA.ORG_PN=HEAACv2_L2_128,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAACv2_L2_128,http-get:*:audio/mp4:DLNA.ORG_PN=HEAACv2_L2_320,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAACv2_L2_320,http-get:*:audio/mp4:DLNA.ORG_PN=HEAACv2_L3,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAACv2_L3,http-get:*:audio/mp4:DLNA.ORG_PN=HEAACv2_L4,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAACv2_L4,http-get:*:audio/mp4:DLNA.ORG_PN=HEAACv2_MULT5,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAACv2_MULT5,http-get:*:audio/mp4:DLNA.ORG_PN=HEAAC_L2_ISO,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAAC_L2_ISO,http-get:*:audio/mp4:DLNA.ORG_PN=HEAAC_L2_ISO_128,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAAC_L2_ISO_128,http-get:*:audio/mp4:DLNA.ORG_PN=HEAAC_L2_ISO_320,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAAC_L2_ISO_320,http-get:*:audio/mp4:DLNA.ORG_PN=HEAAC_L3_ISO,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAAC_L3_ISO,http-get:*:audio/mp4:DLNA.ORG_PN=HEAAC_MULT5_ISO,http-get:*:audio/3gpp:DLNA.ORG_PN=HEAAC_MULT5_ISO,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAFULL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMABASE,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMAPRO,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMALSL_MULT5,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMDRM_WMABASE,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMDRM_WMAFULL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMDRM_WMALSL,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMDRM_WMALSL_MULT5,http-get:*:audio/x-ms-wma:DLNA.ORG_PN=WMDRM_WMAPRO,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM_ICO,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG,http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG_ICO,http-get:*:image/png:DLNA.ORG_PN=PNG_LRG,http-get:*:image/png:DLNA.ORG_PN=PNG_LRG_ICO,http-get:*:image/png:DLNA.ORG_PN=PNG_TN,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVSPML_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVSPLL_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVMED_PRO,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVHIGH_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVMED_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVHIGH_PRO,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVHM_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVMED_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_PRO,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMDRM_WMVSPML_MP3,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHIGH_PRO,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVMED_FULL,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_MP3,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPML_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVSPLL_BASE,http-get:*:video/x-ms-wmv:DLNA.ORG_PN=WMVHM_BASE,http-get:*:video/x-ms-asf:DLNA.ORG_PN=VC1_ASF_AP_L2_WMA,http-get:*:video/x-ms-asf:DLNA.ORG_PN=VC1_ASF_AP_L1_WMA,http-get:*:video/x-ms-asf:DLNA.ORG_PN=WMDRM_VC1_ASF_AP_L1_WMA,http-get:*:video/x-ms-asf:DLNA.ORG_PN=WMDRM_VC1_ASF_AP_L2_WMA,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_ASP_L4_SO_G726,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_SP_G726,http-get:*:video/x-ms-asf:DLNA.ORG_PN=MPEG4_P2_ASF_ASP_L5_SO_G726,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_BL_CIF15_AAC_540_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_BL_CIF15_AAC_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_BL_CIF30_AAC_940_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_BL_CIF30_AAC_MULT5_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_BL_CIF30_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_BL_CIF30_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG1,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG4_P2_TS_SP_MPEG1_L3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG4_P2_TS_SP_AC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG4_P2_TS_SP_MPEG2_L2_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL_XAC3,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC_XAC3,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_KO_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_KO_XAC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_NA_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_HD_NA_XAC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_MP_LL_AAC_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_EU_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_KO_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_KO_XAC3_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_ISO,http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_XAC3_ISO,http-get:*:video/3gpp:DLNA.ORG_PN=AVC_3GPP_BL_QCIF15_AAC,http-get:*:video/3gpp:DLNA.ORG_PN=AVC_3GPP_BL_QCIF15_HEAAC,http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AAC,http-get:*:video/3gpp:DLNA.ORG_PN=MPEG4_P2_3GPP_SP_L0B_AMR,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_AAC_350,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_AAC_520,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_HEAAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_HEAAC_350,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF30_AAC_940,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF30_AAC_MULT5,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF30_HEAAC_L2,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF30_MPEG1_L3,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L12_CIF15_HEAAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L12_CIF15_HEAACv2,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L12_CIF15_HEAACv2_350,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L1B_QCIF15_HEAAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L1B_QCIF15_HEAACv2,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L2_CIF30_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L31_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L32_HD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3L_SD_HEAAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_L3_SD_AAC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AAC_LC,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_AAC_MULT5,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_HEAAC_L2,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_HEAAC_L4,http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_MP_SD_MPEG1_L3,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_ASP_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_ASP_HEAAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_ASP_HEAAC_MULT5,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_SP_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_SP_HEAAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_SP_L2_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_SP_VGA_AAC,http-get:*:video/mp4:DLNA.ORG_PN=MPEG4_P2_MP4_SP_VGA_HEAAC,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG4_P2_TS_SP_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG4_P2_TS_SP_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG4_P2_TS_SP_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG4_P2_TS_SP_MPEG1_L3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG4_P2_TS_SP_MPEG2_L2,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG4_P2_TS_SP_MPEG2_L2_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_KO,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_KO_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_KO_XAC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_KO_XAC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA_XAC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_HD_NA_XAC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_JP_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_MP_LL_AAC,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_MP_LL_AAC_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_KO,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_KO_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_KO_XAC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_KO_XAC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_XAC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_XAC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF15_AAC,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF15_AAC_540,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF15_AAC_540_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF15_AAC_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF30_AAC_940,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF30_AAC_940_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF30_AAC_MULT5,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF30_AAC_MULT5_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF30_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF30_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF30_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_BL_CIF30_MPEG1_L3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_JP_AAC_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AAC_MULT5_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_HD_MPEG1_L3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AAC_MULT5_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_AC3_T,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3,http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=AVC_TS_MP_SD_MPEG1_L3_T");
		service->SetStateVariable("SourceProtocolInfo", "");

		service.Detach();
		service = NULL;
	}

	{
		/* RenderingControl */
		service = new PLT_Service(
			this,
			"urn:schemas-upnp-org:service:RenderingControl:1",
			"urn:upnp-org:serviceId:RenderingControl",
			"RenderingControl",
			"urn:schemas-upnp-org:metadata-1-0/RCS/");
		NPT_CHECK_FATAL(service->SetSCPDXML((const char*) RDR_RenderingControlSCPD));
		NPT_CHECK_FATAL(AddService(service.AsPointer()));

		service->SetStateVariableRate("LastChange", NPT_TimeInterval(0.2f));

		service->SetStateVariable("Mute", "0");
		service->SetStateVariableExtraAttribute("Mute", "Channel", "Master");
		service->SetStateVariable("Volume", "100");
		service->SetStateVariableExtraAttribute("Volume", "Channel", "Master");
		service->SetStateVariable("VolumeDB", "0");
		service->SetStateVariableExtraAttribute("VolumeDB", "Channel", "Master");

		service->SetStateVariable("PresetNameList", "FactoryDefaults");

		service.Detach();
		service = NULL;
	}

	return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::UpdateServices
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::UpdateServices(const char* value,const char* data)
{

	PLT_Service* serviceUpdate;
	if(FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", serviceUpdate) == NPT_ERROR_NO_SUCH_ITEM){

		LOGI("cant find PLT_Service.....");
		return NPT_FAILURE;
	}
	LOGI3("\n----UpdateServices----value =%s data =%s\n", value, data);

	if(*(value+2) == ':' && *(value+5) == ':')
	{
		/*if(*(data+0) == 'd')
		{*/
			serviceUpdate->SetStateVariable("CurrentTrackDuration", value);
			serviceUpdate->SetStateVariable("CurrentMediaDuration", value);
		/*}
		else
		{*/
			serviceUpdate->SetStateVariable("RelativeTimePosition", value);
			serviceUpdate->SetStateVariable("AbsoluteTimePosition", value);
		//}
	}
	else
	{
		serviceUpdate->SetStateVariable("TransportState", value);
	}

	return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnAction
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnAction(PLT_ActionReference&          action,
							const PLT_HttpRequestContext& context)
{
	NPT_COMPILER_UNUSED(context);

	/* parse the action name */
	NPT_String name = action->GetActionDesc().GetName();
	NPT_String serviceType = action->GetActionDesc().GetService()->GetServiceType();

	LOGI3("OnAction -> %s => %s", name.GetChars(), serviceType.GetChars());

	// since all actions take an instance ID and we only support 1 instance
	// verify that the Instance ID is 0 and return an error here now if not
	if (serviceType.Compare("urn:schemas-upnp-org:service:AVTransport:1", true) == 0) {
		if (NPT_FAILED(action->VerifyArgumentValue("InstanceID", "0"))) {
			action->SetError(718, "Not valid InstanceID");
			return NPT_FAILURE;
		}
	}
	if (serviceType.Compare("urn:schemas-upnp-org:service:RenderingControl:1", true) == 0) {
		if (NPT_FAILED(action->VerifyArgumentValue("InstanceID", "0"))) {
			action->SetError(702, "Not valid InstanceID");
			return NPT_FAILURE;
		}
	}

	/* Is it a ConnectionManager Service Action ? */
	if (name.Compare("GetCurrentConnectionInfo", true) == 0) {
		return OnGetCurrentConnectionInfo(action);
	}

	/* Is it a AVTransport Service Action ? */
	if (name.Compare("Next", true) == 0) {
		return OnNext(action);
	}
	if (name.Compare("Pause", true) == 0) {
		return OnPause(action);
	}
	if (name.Compare("Play", true) == 0) {
		return OnPlay(action);
	}
	if (name.Compare("Previous", true) == 0) {
		return OnPrevious(action);
	}
	if (name.Compare("Seek", true) == 0) {
		return OnSeek(action);
	}
	if (name.Compare("Stop", true) == 0) {
		return OnStop(action);
	}
	if (name.Compare("SetAVTransportURI", true) == 0) {
		return OnSetAVTransportURI(action);
	}
	if (name.Compare("SetPlayMode", true) == 0) {
		return OnSetPlayMode(action);
	}

	/* Is it a RendererControl Service Action ? */
	if (name.Compare("SetVolume", true) == 0) {
		  return OnSetVolume(action);
	}
	if (name.Compare("SetVolumeDB", true) == 0) {
		return OnSetVolumeDB(action);
	}
	if (name.Compare("GetVolumeDBRange", true) == 0) {
		return OnGetVolumeDBRange(action);

	}
	if (name.Compare("SetMute", true) == 0) {
		return OnSetMute(action);
	}
	if (name.Compare("GetMediaInfo", true) == 0) {
		return OnGetMediaInfo(action);
	}
	if (name.Compare("GetPositionInfo", true) == 0) {
		return OnGetPositionInfo(action);
	}

	// other actions rely on state variables
	NPT_CHECK_LABEL_WARNING(action->SetArgumentsOutFromStateVariable(), failure);
	return NPT_SUCCESS;

failure:
	action->SetError(401,"No Such Action.");
	return NPT_FAILURE;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnGetCurrentConnectionInfo
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnGetCurrentConnectionInfo(PLT_ActionReference& action)
{
	if (NPT_FAILED(action->VerifyArgumentValue("ConnectionID", "0"))) {
		action->SetError(706,"No Such Connection.");
		return NPT_FAILURE;
	}

	if (NPT_FAILED(action->SetArgumentValue("RcsID", "0"))){
		return NPT_FAILURE;
	}
	if (NPT_FAILED(action->SetArgumentValue("AVTransportID", "0"))) {
		return NPT_FAILURE;
	}
	if (NPT_FAILED(action->SetArgumentOutFromStateVariable("ProtocolInfo"))) {
		return NPT_FAILURE;
	}
	if (NPT_FAILED(action->SetArgumentValue("PeerConnectionManager", "/"))) {
		return NPT_FAILURE;
	}
	if (NPT_FAILED(action->SetArgumentValue("PeerConnectionID", "-1"))) {
		return NPT_FAILURE;
	}
	if (NPT_FAILED(action->SetArgumentValue("Direction", "Input"))) {
		return NPT_FAILURE;
	}
	if (NPT_FAILED(action->SetArgumentValue("Status", "Unknown"))) {
		return NPT_FAILURE;
	}

	if (m_Delegate) {
		return m_Delegate->OnGetCurrentConnectionInfo(action);
	}

	return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnNext
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnNext(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnNext(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnPause
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnPause(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnPause(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnPlay
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnPlay(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnPlay(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnPrevious
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnPrevious(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnPrevious(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnSeek
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnSeek(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnSeek(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnStop
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnStop(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnStop(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnSetAVTransportURI
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnSetAVTransportURI(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnSetAVTransportURI(action);
	}

	// default implementation is using state variable
	NPT_String uri;
	NPT_CHECK_WARNING(action->GetArgumentValue("CurrentURI", uri));

	NPT_String metadata;
	NPT_CHECK_WARNING(action->GetArgumentValue("CurrentURIMetaData", metadata));

	PLT_Service* serviceAVT;
	NPT_CHECK_WARNING(FindServiceByType("urn:schemas-upnp-org:service:AVTransport:1", serviceAVT));

	// update service state variables
	serviceAVT->SetStateVariable("AVTransportURI", uri);
	serviceAVT->SetStateVariable("AVTransportURIMetaData", metadata);

	return NPT_SUCCESS;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnSetPlayMode
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnSetPlayMode(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnSetPlayMode(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnSetVolume
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnSetVolume(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnSetVolume(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnSetVolumeDB
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnSetVolumeDB(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnSetVolumeDB(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnGetVolumeDBRange
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnGetVolumeDBRange(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnGetVolumeDBRange(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnSetMute
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnSetMute(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnSetMute(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnGetMediaInfo
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnGetMediaInfo(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnGetMediaInfo(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------------
|   PLT_MediaRenderer::OnGetMediaInfo
+---------------------------------------------------------------------*/
NPT_Result
PLT_MediaRenderer::OnGetPositionInfo(PLT_ActionReference& action)
{
	if (m_Delegate) {
		return m_Delegate->OnGetPositionInfo(action);
	}
	return NPT_ERROR_NOT_IMPLEMENTED;
}
