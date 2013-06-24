LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := easyndk_static

LOCAL_MODULE_FILENAME := libeasyndk

LOCAL_SRC_FILES := ../../NDKCallbackData.cpp \
	../../NDKHelper.cpp \
	../../JsonHelper.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += jansson_static

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../..
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_PATH)/../../..

include $(BUILD_STATIC_LIBRARY)

$(call import-module,external/jansson)
