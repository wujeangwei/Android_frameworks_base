/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "UsbMidiDeviceJNI"
#define LOG_NDEBUG 0
#include "utils/Log.h"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "android_runtime/Log.h"

#include <stdio.h>
#include <errno.h>
#include <asm/byteorder.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sound/asound.h>

namespace android
{

static jclass sFileDescriptorClass;

static jint
android_server_UsbMidiDevice_get_subdevice_count(JNIEnv *env, jobject /* thiz */,
        jint card, jint device)
{
    char    path[100];

    snprintf(path, sizeof(path), "/dev/snd/controlC%d", card);
    int fd = open(path, O_RDWR);
    if (fd < 0) {
        ALOGE("could not open %s", path);
        return 0;
    }

    struct snd_rawmidi_info info;
    memset(&info, 0, sizeof(info));
    info.device = device;
    int ret = ioctl(fd, SNDRV_CTL_IOCTL_RAWMIDI_INFO, &info);
    close(fd);

    if (ret < 0) {
        ALOGE("SNDRV_CTL_IOCTL_RAWMIDI_INFO failed, errno: %d path: %s", errno, path);
        return -1;
    }

    ALOGD("subdevices_count: %d", info.subdevices_count);
    return info.subdevices_count;
}

static jobjectArray
android_server_UsbMidiDevice_open(JNIEnv *env, jobject /* thiz */, jint card, jint device,
        jint subdevice_count)
{
    char    path[100];

    snprintf(path, sizeof(path), "/dev/snd/midiC%dD%d", card, device);

    jobjectArray fds = env->NewObjectArray(subdevice_count, sFileDescriptorClass, NULL);
    if (!fds) {
        return NULL;
    }

    // to support multiple subdevices we open the same file multiple times
    for (int i = 0; i < subdevice_count; i++) {
        int fd = open(path, O_RDWR);
        if (fd < 0) {
            ALOGE("open failed on %s for index %d", path, i);
            return NULL;
        }

        jobject fileDescriptor = jniCreateFileDescriptor(env, fd);
        env->SetObjectArrayElement(fds, i, fileDescriptor);
        env->DeleteLocalRef(fileDescriptor);
    }

    return fds;
}

static JNINativeMethod method_table[] = {
    { "nativeGetSubdeviceCount", "(II)I", (void*)android_server_UsbMidiDevice_get_subdevice_count },
    { "nativeOpen", "(III)[Ljava/io/FileDescriptor;", (void*)android_server_UsbMidiDevice_open },
};

int register_android_server_UsbMidiDevice(JNIEnv *env)
{
    jclass clazz = env->FindClass("java/io/FileDescriptor");
    if (clazz == NULL) {
        ALOGE("Can't find java/io/FileDescriptor");
        return -1;
    }
    sFileDescriptorClass = (jclass)env->NewGlobalRef(clazz);;

    clazz = env->FindClass("com/android/server/usb/UsbMidiDevice");
    if (clazz == NULL) {
        ALOGE("Can't find com/android/server/usb/UsbMidiDevice");
        return -1;
    }

    return jniRegisterNativeMethods(env, "com/android/server/usb/UsbMidiDevice",
            method_table, NELEM(method_table));
}

};