
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
OPENCV_CAMERA_MODULES:=off
#OPENCV_MK_PATH:=/home/anand/workspace/android_openCV/OpenCV-2.3.1/share/OpenCV/OpenCV.mk
#ifeq ("$(wildcard $(OPENCV_MK_PATH))","")
	#try to load OpenCV.mk from default install location
#	include $(TOOLCHAIN_PREBUILT_ROOT)/user/share/OpenCV/OpenCV.mk
#else
#	include $(OPENCV_MK_PATH)
#endif
include /home/anand/workspace/android_openCV/OpenCV-2.3.1/share/OpenCV/OpenCV.mk
LOCAL_MODULE    := parsesudo
LOCAL_SRC_FILES := DetectSudo.cpp
LOCAL_LDLIBS +=  -llog -ldl



#LOCAL_STATIC_LIBRARIES := opencv_core opencv_ml opencv_features2d

include $(BUILD_SHARED_LIBRARY)
