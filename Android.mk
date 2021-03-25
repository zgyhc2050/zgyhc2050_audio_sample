LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hippoaudio

LOCAL_SHARED_LIBRARIES := \
    libaaudio \
    libcutils \
    libutils \
    libtinyalsa \
    libdl \
    libmedia \
    libbinder \
    libusbhost \
    libstagefright

LOCAL_C_INCLUDES := \
    external/tinyalsa/include \
    frameworks/av/include/media/stagefright \
    frameworks/native/include/media/openmax \
    frameworks/av/media/libaaudio/include \
    frameworks/av/media/libmediametrics/include \
    $(LOCAL_PATH)/ \

LOCAL_SRC_FILES := \
    zgyhc2050_main.cpp \
    zgyhc_common.cpp \
    trackConfig.cpp \
    aaudioConfig.cpp \

LOCAL_LDLIBS += -llog
LOCAL_LDLIBS += -laudioclient


LOCAL_CFLAGS := -DANDROID_PLATFORM_SDK_VERSION=$(PLATFORM_SDK_VERSION)

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

include $(BUILD_EXECUTABLE)
