LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := platinum-jni
LOCAL_SRC_FILES  := platinum-jni.cpp \
					PltMediaRendererDelegate.cpp
LOCAL_LDLIBS     += -llog
LOCAL_LDLIBS     += -landroid

PLT_ROOT := $(LOCAL_PATH)/../../../../..
PLT_SRC_ROOT := $(PLT_ROOT)/Source
LOCAL_C_INCLUDES += $(PLT_SRC_ROOT)/Platinum
LOCAL_C_INCLUDES += $(PLT_SRC_ROOT)/Core
LOCAL_C_INCLUDES += $(PLT_SRC_ROOT)/Devices/MediaConnect
LOCAL_C_INCLUDES += $(PLT_SRC_ROOT)/Devices/MediaServer
LOCAL_C_INCLUDES += $(PLT_SRC_ROOT)/Devices/MediaRenderer
LOCAL_C_INCLUDES += $(PLT_SRC_ROOT)/Extras
LOCAL_C_INCLUDES += $(PLT_ROOT)/ThirdParty/Neptune/Source/Core

LOCAL_CFLAGS += -DNPT_CONFIG_ENABLE_LOGGING #-DNDK_DEBUG=1

LOCAL_LDLIBS += -L$(PLT_ROOT)/Build/Targets/arm-android-linux/Release

LOCAL_LDLIBS += -lPlatinum
LOCAL_LDLIBS += -lNeptune
LOCAL_LDLIBS += -laxTLS
LOCAL_LDLIBS += -lPltMediaRenderer

ifeq ($(NDK_DEBUG),1) 
LOCAL_LDLIBS += -L$(PLT_ROOT)/Build/Targets/arm-android-linux/Debug
endif

include $(BUILD_SHARED_LIBRARY)