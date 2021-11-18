/*
 * Copyright 2021 Alex Andres
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "JNI_AudioProcessing.h"
#include "Exception.h"
#include "JavaArrayList.h"
#include "JavaEnums.h"
#include "JavaError.h"
#include "JavaObject.h"
#include "JavaRef.h"
#include "JavaString.h"
#include "JavaUtils.h"

#include "media/audio/AudioProcessing.h"
#include "media/audio/AudioProcessingConfig.h"
#include "media/audio/AudioProcessingStreamConfig.h"
#include "api/scoped_refptr.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "rtc_base/logging.h"

JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_audio_AudioProcessing_applyConfig
(JNIEnv * env, jobject caller, jobject config)
{
	webrtc::AudioProcessing * apm = GetHandle<webrtc::AudioProcessing>(env, caller);
	CHECK_HANDLE(apm);

	apm->ApplyConfig(jni::AudioProcessingConfig::toNative(env, jni::JavaLocalRef<jobject>(env, config)));
}

JNIEXPORT jint JNICALL Java_dev_onvoid_webrtc_media_audio_AudioProcessing_processStream___3BLdev_onvoid_webrtc_media_audio_AudioProcessingStreamConfig_2Ldev_onvoid_webrtc_media_audio_AudioProcessingStreamConfig_2_3B
(JNIEnv * env, jobject caller, jbyteArray src, jobject inputConfig, jobject outputConfig, jbyteArray dest)
{
	webrtc::AudioProcessing * apm = GetHandle<webrtc::AudioProcessing>(env, caller);
	CHECK_HANDLEV(apm, 0);

	webrtc::StreamConfig srcConfig = jni::AudioProcessingStreamConfig::toNative(env, jni::JavaLocalRef<jobject>(env, inputConfig));
	webrtc::StreamConfig dstConfig = jni::AudioProcessingStreamConfig::toNative(env, jni::JavaLocalRef<jobject>(env, outputConfig));

	jboolean isDstCopy = JNI_FALSE;

	jbyte * srcPtr = env->GetByteArrayElements(src, nullptr);
	jbyte * dstPtr = env->GetByteArrayElements(dest, &isDstCopy);

	int result = apm->ProcessStream(reinterpret_cast<int16_t *>(srcPtr), srcConfig, dstConfig, reinterpret_cast<int16_t *>(dstPtr));

	if (isDstCopy == JNI_TRUE) {
		jsize dstLength = env->GetArrayLength(dest);

		env->SetByteArrayRegion(dest, 0, dstLength, dstPtr);
	}

	env->ReleaseByteArrayElements(src, srcPtr, JNI_ABORT);
	env->ReleaseByteArrayElements(dest, dstPtr, JNI_ABORT);

	return result;
}

JNIEXPORT jint JNICALL Java_dev_onvoid_webrtc_media_audio_AudioProcessing_processReverseStream___3BLdev_onvoid_webrtc_media_audio_AudioProcessingStreamConfig_2Ldev_onvoid_webrtc_media_audio_AudioProcessingStreamConfig_2_3B
(JNIEnv * env, jobject caller, jbyteArray src, jobject inputConfig, jobject outputConfig, jbyteArray dest)
{
	webrtc::AudioProcessing * apm = GetHandle<webrtc::AudioProcessing>(env, caller);
	CHECK_HANDLEV(apm, 0);

	webrtc::StreamConfig srcConfig = jni::AudioProcessingStreamConfig::toNative(env, jni::JavaLocalRef<jobject>(env, inputConfig));
	webrtc::StreamConfig dstConfig = jni::AudioProcessingStreamConfig::toNative(env, jni::JavaLocalRef<jobject>(env, outputConfig));

	jboolean isDstCopy = JNI_FALSE;

	jbyte * srcPtr = env->GetByteArrayElements(src, nullptr);
	jbyte * dstPtr = env->GetByteArrayElements(dest, &isDstCopy);

	int result = apm->ProcessReverseStream(reinterpret_cast<int16_t *>(srcPtr), srcConfig, dstConfig, reinterpret_cast<int16_t *>(dstPtr));

	if (isDstCopy == JNI_TRUE) {
		jsize dstLength = env->GetArrayLength(dest);

		env->SetByteArrayRegion(dest, 0, dstLength, dstPtr);
	}

	env->ReleaseByteArrayElements(src, srcPtr, JNI_ABORT);
	env->ReleaseByteArrayElements(dest, dstPtr, JNI_ABORT);

	return result;
}

JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_audio_AudioProcessing_dispose
(JNIEnv * env, jobject caller)
{
	webrtc::AudioProcessing * apm = GetHandle<webrtc::AudioProcessing>(env, caller);
	CHECK_HANDLE(apm);

	rtc::RefCountReleaseStatus status = apm->Release();

	if (status != rtc::RefCountReleaseStatus::kDroppedLastRef) {
		RTC_LOG(WARNING) << "Native object was not deleted. A reference is still around somewhere.";
	}

	SetHandle<std::nullptr_t>(env, caller, nullptr);

	apm = nullptr;
}

JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_audio_AudioProcessing_initialize
(JNIEnv * env, jobject caller)
{
	rtc::scoped_refptr<webrtc::AudioProcessing> apm = webrtc::AudioProcessingBuilder().Create();

	if (!apm) {
		env->Throw(jni::JavaError(env, "Create AudioProcessing failed"));
		return;
	}

	SetHandle(env, caller, apm.release());
}

JNIEXPORT void JNICALL Java_dev_onvoid_webrtc_media_audio_AudioProcessing_updateStats
(JNIEnv* env, jobject caller)
{
	webrtc::AudioProcessing * apm = GetHandle<webrtc::AudioProcessing>(env, caller);
	CHECK_HANDLE(apm);

	jni::AudioProcessing::updateStats(apm->GetStatistics(), env, jni::JavaLocalRef<jobject>(env, caller));
}