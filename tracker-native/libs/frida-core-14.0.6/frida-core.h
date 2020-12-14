#ifndef ARCH_SPECIFIC_FRIDA_CORE_H
#define ARCH_SPECIFIC_FRIDA_CORE_H

#if ARCH_ABI == arm64-v8a
#include "arm64-v8a/frida-core.h"
#elif ARCH_ABI == armeabi-v7a
#include "armeabi-v7a/frida-core.h"
#endif

#endif //ARCH_SPECIFIC_FRIDA_CORE_H
