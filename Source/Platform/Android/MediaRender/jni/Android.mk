LOCAL_PATH := $(call my-dir)

PLT_ROOT := $(LOCAL_PATH)/../../../../..
PLT_SRC_ROOT := $(PLT_ROOT)/Source
PLT_INCLUDES := $(PLT_SRC_ROOT)/Platinum
PLT_INCLUDES += $(PLT_SRC_ROOT)/Core
PLT_INCLUDES += $(PLT_SRC_ROOT)/Devices/MediaConnect
PLT_INCLUDES += $(PLT_SRC_ROOT)/Devices/MediaServer
PLT_INCLUDES += $(PLT_SRC_ROOT)/Devices/MediaRenderer
PLT_INCLUDES += $(PLT_SRC_ROOT)/Extras
PLT_INCLUDES += $(PLT_ROOT)/ThirdParty/Neptune/Source/Core

PLT_CFLAGS += -DNPT_CONFIG_ENABLE_LOGGING #-DNDK_DEBUG=1

PLT_LDLIBS := -L$(PLT_ROOT)/Build/Targets/arm-android-linux/Release
PLT_LDLIBS += -llog
PLT_LDLIBS += -landroid
PLT_LDLIBS += -lPlatinum
PLT_LDLIBS += -lNeptune
PLT_LDLIBS += -laxTLS
PLT_LDLIBS += -lPltMediaRenderer

ifeq ($(NDK_DEBUG),1) 
PLT_LDLIBS += -L$(PLT_ROOT)/Build/Targets/arm-android-linux/Debug
endif

include $(CLEAR_VARS)

LOCAL_MODULE     := platinum-jni
LOCAL_SRC_FILES  := platinum-jni.cpp PltMediaRendererDelegate.cpp
LOCAL_C_INCLUDES := $(PLT_INCLUDES)
LOCAL_LDLIBS     := $(PLT_LDLIBS)

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE     := MediaRendererTest
LOCAL_SRC_FILES  := MediaRendererTest.cpp
LOCAL_C_INCLUDES := $(PLT_INCLUDES)
LOCAL_LDLIBS     := $(PLT_LDLIBS)

include $(BUILD_EXECUTABLE)
