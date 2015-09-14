LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := HelloNDK
LOCAL_SRC_FILES := HelloNDK.cpp
LOCAL_LDLIBS := -ljnigraphics
LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)
