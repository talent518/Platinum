/*****************************************************************
|
|      Android JNI Interface
|
|      (c) 2002-2012 Plutinosoft LLC
|      Author: Sylvain Rebaud (sylvain@plutinosoft.com)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <assert.h>
#include <jni.h>
#include <string.h>
#include <sys/types.h>

#include "com_geniusgithub_mediarender_jni_PlatinumJniProxy.h"
#include "Platinum.h"
#include "PltMediaRenderer.h"
#include <stdlib.h> 

#include "PltUPnP.h"
#include "PltDeviceHost.h"

#include "PltMediaRendererDelegate.h"
#include "PltService.h"

////////////////pngcui///////////////////////
#include <android/log.h>
#define TAG "Platinum-jni" 
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)

PLT_UPnP upnp;

PLT_DeviceHostReference *device = NULL;
PltMediaRendererDelegate *mediaRenderDelegate = NULL;
PLT_MediaRenderer *mediaRenderer = NULL;

JavaVM *g_vm = NULL;  
jclass g_inflectClass = NULL;  
jmethodID g_methodID = NULL; 


/*----------------------------------------------------------------------
|   logging
+---------------------------------------------------------------------*/
NPT_SET_LOCAL_LOGGER("platinum.android.jni")

/*----------------------------------------------------------------------
|   functions
+---------------------------------------------------------------------*/
__attribute__((constructor)) static void onDlOpen(void)
{
}

void InitInflectClass(JavaVM* vm)
{  
	g_vm = vm;
  
	JNIEnv *env = NULL;  
	int status = g_vm->GetEnv((void **) &env, JNI_VERSION_1_4);  
	if(status != 0)   
	{  
		return ;  
	}  
 
	jclass inflectClass = env->FindClass("com/geniusgithub/mediarender/jni/PlatinumReflection");  
	
	if (inflectClass == NULL)  
	{
		LOGI("inflectClass == NULL return");
		return ;
	}  
	g_inflectClass = (jclass)env->NewGlobalRef(inflectClass);
	jmethodID methodID = env->GetStaticMethodID(inflectClass, "onActionReflection", "(ILjava/lang/String;Ljava/lang/String;)V");  
	if (methodID == NULL)
	{  
		LOGI("methodID == NULL return");
		return ; 
	}
	g_methodID = methodID;
	return ;
}

/*----------------------------------------------------------------------
|    JNI_OnLoad
+---------------------------------------------------------------------*/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
#if NDK_DEBUG
	NPT_LogManager::GetDefault().Configure("plist:.level=FINE;.handlers=ConsoleHandler;.ConsoleHandler.outputs=2;.ConsoleHandler.colors=false;.ConsoleHandler.filter=59");
#endif

	LOGI("jni_OnLoad....");

	InitInflectClass(vm);

	return JNI_VERSION_1_4;
}

char* ConvertJByteaArrayToChars(JNIEnv *env, jbyteArray bytearray, jbyte *&bytes)
{	
	char *chars = NULL; 
	bytes = env->GetByteArrayElements(bytearray, 0);	
	int chars_len = env->GetArrayLength(bytearray); 
	chars = new char[chars_len + 1];
	memcpy(chars, bytes, chars_len);
	chars[chars_len] = 0;	
	return chars;
}

/*
 * Class:     com_geniusgithub_mediarender_jni_PlatinumJniProxy
 * Method:    startMediaRender
 * Signature: ()J
 */
JNIEXPORT void JNICALL Java_com_geniusgithub_mediarender_jni_PlatinumJniProxy_startMediaRender
		(JNIEnv *env, jclass, jbyteArray name, jbyteArray uuid)
{
	if(mediaRenderDelegate != NULL || mediaRenderer != NULL || device != NULL) {
		LOGI("started");
		return;
	}

	LOGI("starting");

	jbyte *bytes;
	char *c_name = ConvertJByteaArrayToChars(env, name, bytes);	
	jbyte *bytes2;
	char *c_uuid = ConvertJByteaArrayToChars(env, uuid, bytes2); 
	
	LOGI("name =  %s, uuid = %s", c_name, c_uuid);

	mediaRenderDelegate = new PltMediaRendererDelegate();

	mediaRenderer = new PLT_MediaRenderer(c_name, false, c_uuid);
	mediaRenderer->SetDelegate(mediaRenderDelegate);

	device = new PLT_DeviceHostReference(mediaRenderer);
	
	upnp.AddDevice(*device);
	upnp.Start();

	env->ReleaseByteArrayElements(name, bytes, JNI_ABORT);	
	env->ReleaseByteArrayElements(uuid, bytes2, JNI_ABORT); 

	// delete c_name;	
	// delete c_uuid;

	LOGI("started");
}

/*
 * Class:     com_geniusgithub_mediarender_jni_PlatinumJniProxy
 * Method:    stopMediaRender
 * Signature: (J)I
 */
JNIEXPORT void JNICALL Java_com_geniusgithub_mediarender_jni_PlatinumJniProxy_stopMediaRender
		(JNIEnv *env, jclass)
{
	if(mediaRenderDelegate == NULL || mediaRenderer == NULL || device == NULL) {
		LOGI("stopped");
		return;
	}

	LOGI("stopping");

	upnp.RemoveDevice(*device);
	LOGI("stopping-0");
	upnp.Stop();

	/*LOGI("stopping-1");
	delete mediaRenderDelegate;
	LOGI("stopping-2");
	delete mediaRenderer;
	LOGI("stopping-3");
	delete device;
	LOGI("stopping-4");*/

	mediaRenderDelegate = NULL;
	mediaRenderer = NULL;
	device = NULL;

	LOGI("stopped");
}

/*
 * Class:     com_geniusgithub_mediarender_jni_PlatinumJniProxy
 * Method:    responseGenaEvent
 * Signature: (J)I
 */
JNIEXPORT void JNICALL Java_com_geniusgithub_mediarender_jni_PlatinumJniProxy_responseGenaEvent
		(JNIEnv *env, jclass, jint cmd, jbyteArray value, jbyteArray data)
{
//    NPT_LOG_INFO("response");
	jbyte *bytes;
	char *c_value = ConvertJByteaArrayToChars(env, value, bytes);	
	jbyte *bytes2;
	char *c_data = ConvertJByteaArrayToChars(env, data, bytes2);

	LOGI("ResponseGenaEvent value = %s data = %s",c_value, c_data);
	
	mediaRenderer->UpdateServices(c_value, c_data);
	
	env->ReleaseByteArrayElements(value, bytes, JNI_ABORT);
	env->ReleaseByteArrayElements(data, bytes2, JNI_ABORT);
	delete c_value;	
	delete c_data;
}

