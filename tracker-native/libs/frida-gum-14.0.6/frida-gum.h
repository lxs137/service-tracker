#ifndef ARCH_SPECIFIC_FRIDA_GUM_H
#define ARCH_SPECIFIC_FRIDA_GUM_H

#if ARCH_ABI == armeabi-v7a
#include "armeabi-v7a/frida-gum.h"
#elif ARCH_ABI == arm64-v8a
#include "arm64-v8a/frida-gum.h"
#endif

#endif //ARCH_SPECIFIC_FRIDA_GUM_H
