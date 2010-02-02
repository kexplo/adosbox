LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := dosbox

CG_SUBDIRS := \
src \
src/cpu \
src/cpu/core_dynrec \
src/cpu/core_dyn_x86 \
src/cpu/core_full \
src/cpu/core_normal \
src/debug \
src/dos \
src/fpu \
src/gui \
src/gui/gui_tk \
src/gui/zmbv \
src/hardware \
src/hardware/serialport \
src/ints \
src/libs \
src/misc \
src/platform \
src/platform/visualc \
src/shell \

# Add more subdirs here, like src/subdir1 src/subdir2

CG_SRCDIR=LOCAL_PATH
LOCAL_CFLAGS := $(foreach D, $(CG_SUBDIRS), -I$(CG_SRCDIR)/$(D)) \
				-I$(LOCAL_PATH)/../sdl/include \
				-I$(LOCAL_PATH)/../sdl_mixer \
				-I$(LOCAL_PATH)/../stlport/stlport \
				-I$(LOCAL_PATH)/include \
				-I$(LOCAL_PATH) \

#Change C++ file extension as appropriate
LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := $(foreach F, $(CG_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.cpp))))
# Uncomment to also add C sources
LOCAL_SRC_FILES += $(foreach F, $(CG_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.c))))

# LOCAL_STATIC_LIBRARIES := sdl_mixer sdl tremor stlport
LOCAL_STATIC_LIBRARIES := sdl_mixer sdl stlport 

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog

include $(BUILD_SHARED_LIBRARY)
