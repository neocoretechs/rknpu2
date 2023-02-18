
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "rknn_api.h"

#ifndef _com_neocoretechs_rknn4j_rknpu2
#define __com_neocoretechs_rknn4j_rknpu2
#include "com_neocoretechs_rknn4j_rknpu2.h"
#endif

  rknn_context   ctx;
  int            ret;

  jbyteArray as_byte_array(JNIEnv* env, unsigned char* buf, int len) {
      jbyteArray array = env->NewByteArray(len);
      env->SetByteArrayRegion(array, 0, len, (jbyte*)(buf));
      return array;
  }

  unsigned char* as_unsigned_char_array(JNIEnv* env, jbyteArray array, int len) {
      //int len = env->GetArrayLength(array);
  	unsigned char* data;
  	data = NULL;
  	data = (unsigned char*)malloc(len);
  	if(data == NULL)
  	    return NULL;
      env->GetByteArrayRegion(array, 0, len, (jbyte*)(data));
      return data;
  }


/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_init
 * Signature: ([BII)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1init
  (JNIEnv* env, jobject thisObj, jbyteArray model, jint size, jint flag) {
	 unsigned char* data;
	 data = as_unsigned_char_array(env, model, size);
	 if(data == NULL)
	    return RKNN_ERR_MALLOC_FAIL;
	  ret = rknn_init(&ctx, data, size, flag, NULL);
	  if (ret < RKNN_SUCC) {
	    printf("rknn_init error ret=%d\n", ret);
	  } else {
		  printf("rknn_init success!=%d\n", ret);
	  }
	  return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_init2
 * Signature: ([BIILcom/neocoretechs/rknn4j/rknn_init_extend;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1init2
  (JNIEnv* env, jobject thisObj, jbyteArray model, jint size, jint flag, jobject extend) {
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_destroy
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1destroy
  (JNIEnv* env, jobject thisObj) {
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_query_sdk
 * Signature: (Lcom/neocoretechs/rknn4j/rknn_sdk_version;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1query_1sdk
  (JNIEnv* env, jobject thisObj, jobject info) {
	jstring jstrBuf;
	rknn_sdk_version output_attrs;
	memset(&output_attrs, 0, sizeof(output_attrs));
	ret = rknn_query(ctx, RKNN_QUERY_SDK_VERSION, &output_attrs, sizeof(rknn_sdk_version));
	if(ret >= RKNN_SUCC) {
		printf("sdk version acquired: %s driver version: %s\n", output_attrs.api_version, output_attrs.drv_version);
		jclass userDataClass=env->GetObjectClass(info);
		jmethodID methodId=env->GetMethodID(userDataClass, "setApi_Version", "(Ljava/lang/String;)V");
		jmethodID methodId2=env->GetMethodID( userDataClass, "setDrv_version", "(Ljava/lang/String;)V");
		jstrBuf = env->NewStringUTF(output_attrs.api_version);
		env->CallObjectMethod(info, methodId, jstrBuf);
		jstrBuf = env->NewStringUTF(output_attrs.drv_version);
		env->CallObjectMethod(info, methodId2, jstrBuf);
		printf("jdk values set: %s driver version: %s\n", output_attrs.api_version, output_attrs.drv_version);
	} else {
	    printf("rknn_query_sdk_version error ret=%d\n", ret);
	}
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_query_IO_num
 * Signature: (Lcom/neocoretechs/rknn4j/rknn_input_output_num;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1query_1IO_1num
  (JNIEnv* env, jobject thisObj, jobject info) {
	rknn_input_output_num io_num;
	ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
	if (ret >= RKNN_SUCC) {
		printf("model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);
		jclass userDataClass=env->GetObjectClass(info);
		jmethodID methodId=env->GetMethodID(userDataClass, "setN_input", "(I)V");
		jmethodID methodId2=env->GetMethodID(userDataClass, "setN_output", "(I)V");
		env->CallObjectMethod(info, methodId, io_num.n_input);
		env->CallObjectMethod(info, methodId2, io_num.n_output);
		printf("jdk values set: model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);
	} else {
	    printf("rknn_query_in_out_num error ret=%d\n", ret);
	}
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_query_input_attr
 * Signature: (Lcom/neocoretechs/rknn4j/rknn_tensor_attr;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1query_1input_1attr
  (JNIEnv* env, jobject thisObj, jobject info) {
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_query_output_attr
 * Signature: (Lcom/neocoretechs/rknn4j/rknn_tensor_attr;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1query_1output_1attr
  (JNIEnv* env, jobject thisObj, jobject info) {
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_inputs_set
 * Signature: (I[Lcom/neocoretechs/rknn4j/rknn_input;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1inputs_1set
  (JNIEnv* env, jobject thisObj, jint ninputs, jobjectArray inputs) {
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_run
 * Signature: (Lcom/neocoretechs/rknn4j/rknn_run_extend;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1run
  (JNIEnv* env, jobject thisObj, jobject extend) {
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_outputs_get
 * Signature: (I[Lcom/neocoretechs/rknn4j/rknn_output;Lcom/neocoretechs/rknn4j/rknn_output_extend;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1outputs_1get
  (JNIEnv* env, jobject thisObj, jint noutputs, jobjectArray outputsArray, jobject extend) {
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_outputs_release
 * Signature: (I[Lcom/neocoretechs/rknn4j/rknn_output;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1outputs_1release
  (JNIEnv* env, jobject thisObj, jint noutputs, jobjectArray outputsArray) {
	return ret;
}

