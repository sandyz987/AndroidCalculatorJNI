LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := eval
LOCAL_SRC_FILES:= \
    eval.c\
    utils.c

LOCAL_LDLIBS += -llog -lOpenSLES -landroid
#LOCAL_SHARED_LIBRARIES := liblog libOpenSLES libandroid
include $(BUILD_SHARED_LIBRARY)