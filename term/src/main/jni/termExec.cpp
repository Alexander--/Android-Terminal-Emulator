/*
 * Copyright (C) 2008 The Android Open Source Project
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

/*
 * Copyright (C) 2007 The Android Open Source Project
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

#include "common.h"

#define LOG_TAG "Exec"

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#include "termExec.h"

static jclass class_fileDescriptor;
static jfieldID field_fileDescriptor_descriptor;
static jmethodID method_fileDescriptor_init;

static void android_os_Exec_setPtyWindowSize(JNIEnv *env, jobject clazz,
    jobject fileDescriptor, jint row, jint col, jint xpixel, jint ypixel)
{
    int fd;
    struct winsize sz;

    fd = env->GetIntField(fileDescriptor, field_fileDescriptor_descriptor);

    if (env->ExceptionOccurred() != NULL) {
        return;
    }

    sz.ws_row = row;
    sz.ws_col = col;
    sz.ws_xpixel = xpixel;
    sz.ws_ypixel = ypixel;

    ioctl(fd, TIOCSWINSZ, &sz);
}

static void android_os_Exec_setPtyUTF8Mode(JNIEnv *env, jobject clazz,
    jobject fileDescriptor, jboolean utf8Mode)
{
    int fd;
    struct termios tios;

    fd = env->GetIntField(fileDescriptor, field_fileDescriptor_descriptor);

    if (env->ExceptionOccurred() != NULL) {
        return;
    }

    tcgetattr(fd, &tios);
    if (utf8Mode) {
        tios.c_iflag |= IUTF8;
    } else {
        tios.c_iflag &= ~IUTF8;
    }
    tcsetattr(fd, TCSANOW, &tios);
}

static void android_os_Exec_close(JNIEnv *env, jobject clazz, jobject fileDescriptor)
{
    int fd;

    fd = env->GetIntField(fileDescriptor, field_fileDescriptor_descriptor);

    if (env->ExceptionOccurred() != NULL) {
        return;
    }

    close(fd);
}

static void android_os_Exec_hangupProcessGroup(JNIEnv *env, jobject clazz,
    jint procId) {
    kill(-procId, SIGHUP);
}


static int register_FileDescriptor(JNIEnv *env)
{
    jclass localRef_class_fileDescriptor = env->FindClass("java/io/FileDescriptor");

    if (localRef_class_fileDescriptor == NULL) {
        LOGE("Can't find class java/io/FileDescriptor");
        return -1;
    }

    class_fileDescriptor = (jclass) env->NewGlobalRef(localRef_class_fileDescriptor);

    env->DeleteLocalRef(localRef_class_fileDescriptor);

    if (class_fileDescriptor == NULL) {
        LOGE("Can't get global ref to class java/io/FileDescriptor");
        return -1;
    }

    field_fileDescriptor_descriptor = env->GetFieldID(class_fileDescriptor, "descriptor", "I");

    if (field_fileDescriptor_descriptor == NULL) {
        LOGE("Can't find FileDescriptor.descriptor");
        return -1;
    }

    method_fileDescriptor_init = env->GetMethodID(class_fileDescriptor, "<init>", "()V");
    if (method_fileDescriptor_init == NULL) {
        LOGE("Can't find FileDescriptor.init");
        return -1;
     }
     return 0;
}

static const char *classPathName = "jackpal/androidterm/Exec";
static JNINativeMethod method_table[] = {
    { "setPtyWindowSize", "(Ljava/io/FileDescriptor;IIII)V",
        (void*) android_os_Exec_setPtyWindowSize},
    { "setPtyUTF8Mode", "(Ljava/io/FileDescriptor;Z)V",
        (void*) android_os_Exec_setPtyUTF8Mode},
    { "close", "(Ljava/io/FileDescriptor;)V",
        (void*) android_os_Exec_close},
    { "hangupProcessGroup", "(I)V",
        (void*) android_os_Exec_hangupProcessGroup}
};

int init_Exec(JNIEnv *env) {
    if (register_FileDescriptor(env) < 0) {
        LOGE("Failed to register class java/io/FileDescriptor");
        return JNI_FALSE;
    }

    if (!registerNativeMethods(env, classPathName, method_table,
                 sizeof(method_table) / sizeof(method_table[0]))) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}
