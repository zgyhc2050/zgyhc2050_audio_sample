LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hippoaudio

LOCAL_SHARED_LIBRARIES := libcutils libutils libtinyalsa libdl \
    libmedia libbinder libusbhost libstagefright

LOCAL_C_INCLUDES := \
    external/tinyalsa/include \
    frameworks/av/include/media/stagefright \
    frameworks/native/include/media/openmax \
    $(LOCAL_PATH)/ \

LOCAL_SRC_FILES := \
    trackConfig.cpp \
    zgyhc2050_main.cpp \

LOCAL_LDLIBS += -llog
LOCAL_LDLIBS += -laudioclient


LOCAL_CFLAGS := -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_EXECUTABLE)
