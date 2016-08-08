/*
 * Copyright (C) 2012 The Android Open Source Project
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

#ifndef ART_RUNTIME_ENTRYPOINTS_QUICK_QUICK_ENTRYPOINTS_H_
#define ART_RUNTIME_ENTRYPOINTS_QUICK_QUICK_ENTRYPOINTS_H_

#include <jni.h>

#include "base/macros.h"
#include "base/mutex.h"
#include "offsets.h"

#ifdef MOE
#include "interpreter/interpreter.h"
#endif

#define QUICK_ENTRYPOINT_OFFSET(ptr_size, x) \
    Thread::QuickEntryPointOffset<ptr_size>(OFFSETOF_MEMBER(QuickEntryPoints, x))

namespace art {

namespace mirror {
class Array;
class Class;
template<class MirrorType> class CompressedReference;
class Object;
#ifdef MOE
class String;
template<typename T> class PrimitiveArray;
typedef PrimitiveArray<int8_t> ByteArray;
typedef PrimitiveArray<uint16_t> CharArray;
#endif
}  // namespace mirror

class ArtMethod;
template<class MirrorType> class GcRoot;
class Thread;

// Pointers to functions that are called by quick compiler generated code via thread-local storage.
#ifdef MOE_WINDOWS
#pragma pack(push, 1)
#endif
struct PACKED(4) QuickEntryPoints {
#ifndef MOE
#define ENTRYPOINT_ENUM(name, rettype, ...) rettype ( * p ## name )( __VA_ARGS__ );
#else
#define GET_MACRO(_0,_1,_2,_3,_4,NAME,...) NAME
#define ENTRYPOINT_ENUM(name, rettype, ...) rettype ( * p ## name ) (GET_MACRO(_0, ##__VA_ARGS__, ARGS, ARGS, ARGS, ARGS, ARGS0)(__VA_ARGS__));
#define ARGS(...) __VA_ARGS__, Thread*
#define ARGS0(...) Thread*
#endif
#include "quick_entrypoints_list.h"
  QUICK_ENTRYPOINT_LIST(ENTRYPOINT_ENUM)
#undef QUICK_ENTRYPOINT_LIST
#undef ENTRYPOINT_ENUM
};
#ifdef MOE_WINDOWS
#pragma pack(pop)
#endif


// JNI entrypoints.
// TODO: NO_THREAD_SAFETY_ANALYSIS due to different control paths depending on fast JNI.
#ifdef MOE
extern "C" {
#endif
#ifdef MOE_WINDOWS
__declspec(dllexport)
#endif
#ifdef MOE
extern void EnterInterpreterFromInvoke(Thread* self, ArtMethod* method, uintptr_t* args,
                                       JValue* result);
#endif

#ifdef MOE_WINDOWS
__declspec(dllexport)
#endif
extern uint32_t JniMethodStart(Thread* self) NO_THREAD_SAFETY_ANALYSIS HOT_ATTR;
#ifdef MOE_WINDOWS
__declspec(dllexport)
#endif
extern uint32_t JniMethodStartSynchronized(jobject to_lock, Thread* self)
    NO_THREAD_SAFETY_ANALYSIS HOT_ATTR;
#ifdef MOE_WINDOWS
__declspec(dllexport)
#endif
extern void JniMethodEnd(uint32_t saved_local_ref_cookie, Thread* self)
    NO_THREAD_SAFETY_ANALYSIS HOT_ATTR;
#ifdef MOE_WINDOWS
__declspec(dllexport)
#endif
extern void JniMethodEndSynchronized(uint32_t saved_local_ref_cookie, jobject locked,
                                     Thread* self)
    NO_THREAD_SAFETY_ANALYSIS HOT_ATTR;
#ifdef MOE_WINDOWS
__declspec(dllexport)
#endif
extern mirror::Object* JniMethodEndWithReference(jobject result, uint32_t saved_local_ref_cookie,
                                                 Thread* self)
    NO_THREAD_SAFETY_ANALYSIS HOT_ATTR;

#ifdef MOE_WINDOWS
__declspec(dllexport)
#endif
extern mirror::Object* JniMethodEndWithReferenceSynchronized(jobject result,
                                                             uint32_t saved_local_ref_cookie,
                                                             jobject locked, Thread* self)
    NO_THREAD_SAFETY_ANALYSIS HOT_ATTR;
#ifdef MOE
}
#endif

extern void ReadBarrierJni(mirror::CompressedReference<mirror::Object>* handle_on_stack,
                           Thread* self)
    NO_THREAD_SAFETY_ANALYSIS HOT_ATTR;


// Read barrier entrypoints.
//
// Compilers for ARM, ARM64, MIPS, MIPS64 can insert a call to these
// functions directly.  For x86 and x86-64, compilers need a wrapper
// assembly function, to handle mismatch in ABI.

// Mark the heap reference `obj`. This entry point is used by read
// barrier fast path implementations generated by the compiler to mark
// an object that is referenced by a field of a gray object.
extern "C" mirror::Object* artReadBarrierMark(mirror::Object* obj)
    SHARED_REQUIRES(Locks::mutator_lock_) HOT_ATTR;

// Read barrier entrypoint for heap references.
// This is the read barrier slow path for instance and static fields
// and reference type arrays.
extern "C" mirror::Object* artReadBarrierSlow(mirror::Object* ref,
                                              mirror::Object* obj,
                                              uint32_t offset)
    SHARED_REQUIRES(Locks::mutator_lock_) HOT_ATTR;

// Read barrier entrypoint for GC roots.
extern "C" mirror::Object* artReadBarrierForRootSlow(GcRoot<mirror::Object>* root)
    SHARED_REQUIRES(Locks::mutator_lock_) HOT_ATTR;

}  // namespace art

#endif  // ART_RUNTIME_ENTRYPOINTS_QUICK_QUICK_ENTRYPOINTS_H_
