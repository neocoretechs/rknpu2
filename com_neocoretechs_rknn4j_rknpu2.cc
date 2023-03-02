
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
/*
 * JNI bindings to RKNN NPU API. Accessor and mutator methods favored to transfer
 * values to corresponding Java classes over direct field settings.
 * use: javap -s <javaVersionOfTypedef.class>
 * to see the accessor and mutator method signatures to use in env->GetMethodId(class, method, signature)
 */
  //rknn_context   ctx;
  int            ret;
  void *handle;
  int (*nn_init)(rknn_context*, void*, uint32_t, uint32_t, rknn_init_extend*);
  int (*nn_query)(rknn_context, rknn_query_cmd, void*, uint32_t);
  int (*nn_run)(rknn_context, rknn_run_extend*);
  int (*nn_inputs_set)(rknn_context, uint32_t, rknn_input[]);
  int (*nn_outputs_get)(rknn_context, uint32_t, rknn_output[], rknn_output_extend*);
  int (*nn_outputs_release)(rknn_context, uint32_t, rknn_output[]);
  int (*nn_destroy)(rknn_context);
  char *error;

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

  static void dump_tensor_attr(rknn_tensor_attr* attr) {
    printf("  index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, "
           "zp=%d, scale=%f, fl=%d, w_stride=%d, size_with_stride=%d, pass_through=%d, h_stride=%d\n",
           attr->index, attr->name, attr->n_dims, attr->dims[0], attr->dims[1], attr->dims[2], attr->dims[3],
           attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
           get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale, attr->fl, attr->w_stride,
		   attr->size_with_stride, attr->pass_through, attr->h_stride);
  }
/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_init
 * Signature: ([BII)I
 */
JNIEXPORT jlong JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1init
  (JNIEnv* env, jobject thisObj, jbyteArray model, jint size, jint flag) {
	rknn_context ctx;
	unsigned char* data;
	data = as_unsigned_char_array(env, model, size);
	if(data == NULL)
	    return (jlong)RKNN_ERR_MALLOC_FAIL;
	//ret = rknn_init(&ctx, data, size, flag, NULL);
	handle = dlopen("/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j/librknnrt.so", RTLD_NOW);
	if (!handle) {
		  printf("dlerr=%s\n",dlerror());
		  return (jlong)RKNN_ERR_FAIL;
	}
	dlerror();
	nn_init =(int (*)(rknn_context*, void*, uint32_t, uint32_t, rknn_init_extend*)) dlsym(handle, "rknn_init");
	dlerror();
	ret = (*nn_init)(&ctx, data, size, flag, NULL);
	dlclose(handle);
	if (ret < RKNN_SUCC) {
	    printf("rknn_init error ret=%d\n", ret);
	} else {
		  //printf("rknn_init success=%d ctx=%ld\n", ret,ctx);
	}
	return (jlong)ctx;
}

/*
 * API v1.4.0 guide says rknn_init_extend: The extended information during specific initialization.
 * It is disabled at the moment, which indicates this must be passed by the NULL. If using share weight， it
 * should pass the pointer of another rknn_context pointing to another
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_init2
 * Signature: ([BIILcom/neocoretechs/rknn4j/rknn_init_extend;)I
 */
JNIEXPORT jlong JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1init2
  (JNIEnv* env, jobject thisObj, jbyteArray model, jint size, jint flag, jobject extend) {
	return (jlong)RKNN_ERR_FAIL;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_destroy
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1destroy
  (JNIEnv* env, jobject thisObj, jlong ctx) {
	// release
	//ret = rknn_destroy(ctx);
	handle = dlopen("/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j/librknnrt.so", RTLD_NOW);
	if (!handle) {
		  printf("dlerr=%s\n",dlerror());
		  return RKNN_ERR_FAIL;
	}
	dlerror();
	nn_destroy =(int (*)(rknn_context)) dlsym(handle, "rknn_destroy");
	dlerror();
	ret = (*nn_destroy)((rknn_context)ctx);
	dlclose(handle);
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_query_sdk
 * Signature: (Lcom/neocoretechs/rknn4j/rknn_sdk_version;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1query_1sdk
  (JNIEnv* env, jobject thisObj, jlong ctx, jobject info) {
	jstring jstrBuf;
	rknn_sdk_version output_attrs;
	memset(&output_attrs, 0, sizeof(output_attrs));
	//ret = rknn_query(ctx, RKNN_QUERY_SDK_VERSION, &output_attrs, sizeof(rknn_sdk_version));
	handle = dlopen("/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j/librknnrt.so", RTLD_NOW);
	if (!handle) {
		  printf("dlerr=%s\n",dlerror());
		  return RKNN_ERR_FAIL;
	}
	//printf("query init %p\n",handle);
	dlerror();
	nn_query =(int (*)(rknn_context, rknn_query_cmd, void*, uint32_t)) dlsym(handle, "rknn_query");
	//printf("function pointer query %p\n",nn_query);
	dlerror();
	ret = (*nn_query)((rknn_context)ctx, RKNN_QUERY_SDK_VERSION, &output_attrs, sizeof(rknn_sdk_version));
	dlclose(handle);
	if(ret >= RKNN_SUCC) {
		//printf("sdk version acquired: %s driver version: %s\n", output_attrs.api_version, output_attrs.drv_version);
		jclass userDataClass=env->GetObjectClass(info);
		jmethodID methodId=env->GetMethodID(userDataClass, "setApi_version", "(Ljava/lang/String;)V");
		jmethodID methodId2=env->GetMethodID( userDataClass, "setDrv_version", "(Ljava/lang/String;)V");
		jstrBuf = env->NewStringUTF(output_attrs.api_version);
		env->CallObjectMethod(info, methodId, jstrBuf);
		jstrBuf = env->NewStringUTF(output_attrs.drv_version);
		env->CallObjectMethod(info, methodId2, jstrBuf);
		//printf("jdk values set: %s driver version: %s\n", output_attrs.api_version, output_attrs.drv_version);
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
  (JNIEnv* env, jobject thisObj, jlong ctx, jobject info) {
	rknn_input_output_num io_num;
	//ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
	handle = dlopen("/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j/librknnrt.so", RTLD_NOW);
	if (!handle) {
		  printf("dlerr=%s\n",dlerror());
		  return RKNN_ERR_FAIL;
	}
	dlerror();
	nn_query =(int (*)(rknn_context, rknn_query_cmd, void*, uint32_t)) dlsym(handle, "rknn_query");
	dlerror();
	ret = (*nn_query)((rknn_context)ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
	dlclose(handle);
	if (ret >= RKNN_SUCC) {
		//printf("model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);
		jclass userDataClass=env->GetObjectClass(info);
		jmethodID methodId=env->GetMethodID(userDataClass, "setN_input", "(I)V");
		jmethodID methodId2=env->GetMethodID(userDataClass, "setN_output", "(I)V");
		env->CallObjectMethod(info, methodId, io_num.n_input);
		env->CallObjectMethod(info, methodId2, io_num.n_output);
		//printf("jdk values set: model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);
	} else {
	    printf("rknn_query_in_out_num error ret=%d\n", ret);
	}
	return ret;
}

/*
 * Get the index from the input java instance and place it into outgoing buffer
 * to indicate to the query which object to process.
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_query_input_attr
 * Signature: (Lcom/neocoretechs/rknn4j/rknn_tensor_attr;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1query_1input_1attr
  (JNIEnv* env, jobject thisObj, jlong ctx, jobject info) {
	jstring jstrBuf;
	rknn_tensor_attr input_attr;
	memset(&input_attr, 0, sizeof(input_attr));
    //ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
	handle = dlopen("/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j/librknnrt.so", RTLD_NOW);
	if (!handle) {
		  printf("dlerr=%s\n",dlerror());
		  return RKNN_ERR_FAIL;
	}
	dlerror();
	jclass userDataClass=env->GetObjectClass(info);
	jmethodID getIndex=env->GetMethodID(userDataClass, "getIndex", "()I");
	input_attr.index = 	env->CallIntMethod(info, getIndex);
	nn_query =(int (*)(rknn_context, rknn_query_cmd, void*, uint32_t)) dlsym(handle, "rknn_query");
	dlerror();
	ret = (*nn_query)((rknn_context)ctx, RKNN_QUERY_INPUT_ATTR, &input_attr, sizeof(input_attr));
	dlclose(handle);
	if (ret >= RKNN_SUCC) {
		//dump_tensor_attr(&input_attr);
		jmethodID setN_dims=env->GetMethodID(userDataClass, "setN_dims", "(I)V");
		jmethodID setDim=env->GetMethodID(userDataClass, "setDim", "(II)V");
		jmethodID setN_elems=env->GetMethodID(userDataClass, "setN_elems", "(I)V");
		jmethodID setName=env->GetMethodID(userDataClass, "setName", "(Ljava/lang/String;)V");
		jmethodID setSize=env->GetMethodID(userDataClass, "setSize", "(I)V");
		jmethodID setFmt=env->GetMethodID(userDataClass, "setFmt", "(Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;)V");
		jmethodID setType=env->GetMethodID(userDataClass, "setType", "(Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;)V");
		jmethodID setQnt_type=env->GetMethodID(userDataClass, "setQnt_type", "(Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;)V");
		jmethodID setZp=env->GetMethodID(userDataClass, "setZp", "(I)V");
		jmethodID setScale=env->GetMethodID(userDataClass, "setScale", "(F)V");
		jmethodID setFl=env->GetMethodID(userDataClass, "setFl", "(B)V");
		jmethodID setW_stride=env->GetMethodID(userDataClass, "setW_stride", "(I)V");
		jmethodID setSize_with_stride=env->GetMethodID(userDataClass, "setSize_with_stride", "(I)V");
		jmethodID setPass_through=env->GetMethodID(userDataClass, "setPass_through", "(B)V");
		jmethodID setH_stride=env->GetMethodID(userDataClass, "setH_stride", "(I)V");
		env->CallObjectMethod(info, setN_dims, input_attr.n_dims);
		env->CallObjectMethod(info, setDim, 0, input_attr.dims[0]);
		env->CallObjectMethod(info, setDim, 1, input_attr.dims[1]);
		env->CallObjectMethod(info, setDim, 2, input_attr.dims[2]);
		env->CallObjectMethod(info, setDim, 3, input_attr.dims[3]);
		env->CallObjectMethod(info, setN_elems, input_attr.n_elems);
		jstrBuf = env->NewStringUTF(input_attr.name);
		env->CallObjectMethod(info, setName, jstrBuf);
		env->CallObjectMethod(info, setSize, input_attr.size);
		// tensor format enum conversion
		jclass tensorFmtClass = env->FindClass("com/neocoretechs/rknn4j/RKNN$rknn_tensor_format");
		jfieldID tensorFmtField;
		// determine which enum corresponds to our java enum field
		switch(input_attr.fmt) {
			case RKNN_TENSOR_NCHW:                               /* data format is NCHW. */
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_NCHW", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			case RKNN_TENSOR_NHWC:                                   /* data format is NHWC. */
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_NHWC", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			case RKNN_TENSOR_NC1HWC2:                                /* data format is NC1HWC2. */
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_NC1HWC2", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			case RKNN_TENSOR_UNDEFINED:
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_UNDEFINED", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			case RKNN_TENSOR_FORMAT_MAX:
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_FORMAT_MAX", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			default:
				return RKNN_ERR_FAIL;
		}
		// now get the corresponding java enum
		jobject state = env->GetStaticObjectField(tensorFmtClass, tensorFmtField);
		// now call out mutator method to set the main target java instance with our newly created java enum
		env->CallObjectMethod(info, setFmt, state);
		//
		// tensor type enum conversion
		jclass tensorTypeClass = env->FindClass("com/neocoretechs/rknn4j/RKNN$rknn_tensor_type");
		jfieldID tensorTypeField;
		// determine which enum corresponds to our java enum field
		switch(input_attr.type) {
			case RKNN_TENSOR_FLOAT32:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_FLOAT32", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_FLOAT16:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_FLOAT16", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_INT8:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_INT8", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_UINT8:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_UINT8", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_INT16:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_INT16", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_UINT16:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_UINT16", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_INT32:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_INT32", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_UINT32:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_UINT32", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_INT64:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_INT64", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_BOOL:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_BOOL", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_TYPE_MAX:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_TYPE_MAX", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			default:
				return RKNN_ERR_FAIL;
		}
		// now get the corresponding java enum
		state = env->GetStaticObjectField(tensorTypeClass, tensorTypeField);
		// now call out mutator method to set the main target java instance with our newly created java enum
		env->CallObjectMethod(info, setType, state);
		//
		// tensor Qnt_type enum conversion
		jclass tensorQntTypeClass = env->FindClass("com/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type");
		jfieldID tensorQntTypeField;
		// determine which enum corresponds to our java enum field
		switch(input_attr.qnt_type) {
			case RKNN_TENSOR_QNT_NONE:
				tensorQntTypeField = env->GetStaticFieldID(tensorQntTypeClass , "RKNN_TENSOR_QNT_NONE", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;");
				break;
			case RKNN_TENSOR_QNT_DFP:
				tensorQntTypeField = env->GetStaticFieldID(tensorQntTypeClass , "RKNN_TENSOR_QNT_DFP", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;");
				break;
			case RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC:
				tensorQntTypeField = env->GetStaticFieldID(tensorQntTypeClass , "RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;");
				break;
			case RKNN_TENSOR_QNT_MAX:
				tensorQntTypeField = env->GetStaticFieldID(tensorQntTypeClass , "RKNN_TENSOR_QNT_MAX", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;");
				break;
			default:
				return RKNN_ERR_FAIL;
		}
		// now get the corresponding java enum
		state = env->GetStaticObjectField(tensorQntTypeClass, tensorQntTypeField);
		// now call out mutator method to set the main target java instance with our newly created java enum
		env->CallObjectMethod(info, setQnt_type, state);
		// set the rest of the fields via mutator
		env->CallObjectMethod(info, setZp, input_attr.zp);
		env->CallObjectMethod(info, setScale, input_attr.scale);
		env->CallObjectMethod(info, setFl, input_attr.fl);
		env->CallObjectMethod(info, setW_stride, input_attr.w_stride);
		env->CallObjectMethod(info, setSize_with_stride, input_attr.size_with_stride);
		env->CallObjectMethod(info, setPass_through, input_attr.pass_through);
		env->CallObjectMethod(info, setH_stride, input_attr.h_stride);
		//printf("jdk input attr tensor values set\n");
	} else {
	    printf("rknn_query_input_attr error ret=%d\n", ret);
	}
	return ret;
}

/*
 * Get the index from the input java instance and place it into outgoing buffer
 * to indicate to the query which object to process.
 * Identical process for input attributes, but query output
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_query_output_attr
 * Signature: (Lcom/neocoretechs/rknn4j/rknn_tensor_attr;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1query_1output_1attr
  (JNIEnv* env, jobject thisObj, jlong ctx, jobject info) {
	jstring jstrBuf;
	rknn_tensor_attr output_attr;
	memset(&output_attr, 0, sizeof(output_attr));
    //ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
	handle = dlopen("/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j/librknnrt.so", RTLD_NOW);
	if (!handle) {
		  printf("dlerr=%s\n",dlerror());
		  return RKNN_ERR_FAIL;
	}
	dlerror();
	jclass userDataClass=env->GetObjectClass(info);
	jmethodID getIndex=env->GetMethodID(userDataClass, "getIndex", "()I");
	output_attr.index = 	env->CallIntMethod(info, getIndex);
	nn_query =(int (*)(rknn_context, rknn_query_cmd, void*, uint32_t)) dlsym(handle, "rknn_query");
	dlerror();
	ret = (*nn_query)((rknn_context)ctx, RKNN_QUERY_OUTPUT_ATTR, &output_attr, sizeof(output_attr));
	dlclose(handle);
	if (ret >= RKNN_SUCC) {
		//dump_tensor_attr(&output_attr);
		jmethodID setN_dims=env->GetMethodID(userDataClass, "setN_dims", "(I)V");
		jmethodID setDim=env->GetMethodID(userDataClass, "setDim", "(II)V");
		jmethodID setN_elems=env->GetMethodID(userDataClass, "setN_elems", "(I)V");
		jmethodID setName=env->GetMethodID(userDataClass, "setName", "(Ljava/lang/String;)V");
		jmethodID setSize=env->GetMethodID(userDataClass, "setSize", "(I)V");
		jmethodID setFmt=env->GetMethodID(userDataClass, "setFmt", "(Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;)V");
		jmethodID setType=env->GetMethodID(userDataClass, "setType", "(Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;)V");
		jmethodID setQnt_type=env->GetMethodID(userDataClass, "setQnt_type", "(Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;)V");
		jmethodID setZp=env->GetMethodID(userDataClass, "setZp", "(I)V");
		jmethodID setScale=env->GetMethodID(userDataClass, "setScale", "(F)V");
		jmethodID setFl=env->GetMethodID(userDataClass, "setFl", "(B)V");
		jmethodID setW_stride=env->GetMethodID(userDataClass, "setW_stride", "(I)V");
		jmethodID setSize_with_stride=env->GetMethodID(userDataClass, "setSize_with_stride", "(I)V");
		jmethodID setPass_through=env->GetMethodID(userDataClass, "setPass_through", "(B)V");
		jmethodID setH_stride=env->GetMethodID(userDataClass, "setH_stride", "(I)V");
		env->CallObjectMethod(info, setN_dims, output_attr.n_dims);
		env->CallObjectMethod(info, setDim, 0, output_attr.dims[0]);
		env->CallObjectMethod(info, setDim, 1, output_attr.dims[1]);
		env->CallObjectMethod(info, setDim, 2, output_attr.dims[2]);
		env->CallObjectMethod(info, setDim, 3, output_attr.dims[3]);
		env->CallObjectMethod(info, setN_elems, output_attr.n_elems);
		jstrBuf = env->NewStringUTF(output_attr.name);
		env->CallObjectMethod(info, setName, jstrBuf);
		env->CallObjectMethod(info, setSize, output_attr.size);
		// tensor format enum conversion
		jclass tensorFmtClass = env->FindClass("com/neocoretechs/rknn4j/RKNN$rknn_tensor_format");
		jfieldID tensorFmtField;
		// determine which enum corresponds to our java enum field
		switch(output_attr.fmt) {
			case RKNN_TENSOR_NCHW:                               /* data format is NCHW. */
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_NCHW", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			case RKNN_TENSOR_NHWC:                                   /* data format is NHWC. */
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_NHWC", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			case RKNN_TENSOR_NC1HWC2:                                /* data format is NC1HWC2. */
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_NC1HWC2", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			case RKNN_TENSOR_UNDEFINED:
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_UNDEFINED", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			case RKNN_TENSOR_FORMAT_MAX:
				tensorFmtField = env->GetStaticFieldID(tensorFmtClass , "RKNN_TENSOR_FORMAT_MAX", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
				break;
			default:
				return RKNN_ERR_FAIL;
		}
		// now get the corresponding java enum
		jobject state = env->GetStaticObjectField(tensorFmtClass, tensorFmtField);
		// now call out mutator method to set the main target java instance with our newly created java enum
		env->CallObjectMethod(info, setFmt, state);
		//
		// tensor type enum conversion
		jclass tensorTypeClass = env->FindClass("com/neocoretechs/rknn4j/RKNN$rknn_tensor_type");
		jfieldID tensorTypeField;
		// determine which enum corresponds to our java enum field
		switch(output_attr.type) {
			case RKNN_TENSOR_FLOAT32:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_FLOAT32", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_FLOAT16:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_FLOAT16", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_INT8:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_INT8", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_UINT8:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_UINT8", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_INT16:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_INT16", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_UINT16:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_UINT16", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_INT32:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_INT32", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_UINT32:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_UINT32", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_INT64:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_INT64", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_BOOL:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_BOOL", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			case RKNN_TENSOR_TYPE_MAX:
				tensorTypeField = env->GetStaticFieldID(tensorTypeClass , "RKNN_TENSOR_TYPE_MAX", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
				break;
			default:
				return RKNN_ERR_FAIL;
		}
		// now get the corresponding java enum
		state = env->GetStaticObjectField(tensorTypeClass, tensorTypeField);
		// now call out mutator method to set the main target java instance with our newly created java enum
		env->CallObjectMethod(info, setType, state);
		//
		// tensor Qnt_type enum conversion
		jclass tensorQntTypeClass = env->FindClass("com/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type");
		jfieldID tensorQntTypeField;
		// determine which enum corresponds to our java enum field
		switch(output_attr.qnt_type) {
			case RKNN_TENSOR_QNT_NONE:
				tensorQntTypeField = env->GetStaticFieldID(tensorQntTypeClass , "RKNN_TENSOR_QNT_NONE", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;");
				break;
			case RKNN_TENSOR_QNT_DFP:
				tensorQntTypeField = env->GetStaticFieldID(tensorQntTypeClass , "RKNN_TENSOR_QNT_DFP", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;");
				break;
			case RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC:
				tensorQntTypeField = env->GetStaticFieldID(tensorQntTypeClass , "RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;");
				break;
			case RKNN_TENSOR_QNT_MAX:
				tensorQntTypeField = env->GetStaticFieldID(tensorQntTypeClass , "RKNN_TENSOR_QNT_MAX", "Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_qnt_type;");
				break;
			default:
				return RKNN_ERR_FAIL;
		}
		// now get the corresponding java enum
		state = env->GetStaticObjectField(tensorQntTypeClass, tensorQntTypeField);
		// now call out mutator method to set the main target java instance with our newly created java enum
		env->CallObjectMethod(info, setQnt_type, state);
		// set the rest of the fields via mutator
		env->CallObjectMethod(info, setZp, output_attr.zp);
		env->CallObjectMethod(info, setScale, output_attr.scale);
		env->CallObjectMethod(info, setFl, output_attr.fl);
		env->CallObjectMethod(info, setW_stride, output_attr.w_stride);
		env->CallObjectMethod(info, setSize_with_stride, output_attr.size_with_stride);
		env->CallObjectMethod(info, setPass_through, output_attr.pass_through);
		env->CallObjectMethod(info, setH_stride, output_attr.h_stride);
		//printf("jdk output attr tensor values set\n");
	} else {
	    printf("rknn_query_output_attr error ret=%d\n", ret);
	}
	return ret;
}

/*
 * Take an array of inputs and use it to set up run
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_inputs_set
 * Signature: (I[Lcom/neocoretechs/rknn4j/rknn_input;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1inputs_1set
  (JNIEnv* env, jobject thisObj, jlong ctx, jint ninputs, jobjectArray inputs) {
	//int ret = rknn_inputs_set(ctx, io_num.n_input, inputs);
	rknn_input npuinputs[ninputs];
	memset(npuinputs, 0, sizeof(npuinputs));
	for(int i = 0; i < ninputs; i++) {
		//printf("Getting java array element %d\n",i);
		jobject input = env->GetObjectArrayElement(inputs, i);
		jclass rknnInputClass=env->GetObjectClass(input);
		// rknnInputClass should now be rknn_input
		jmethodID getBuf=env->GetMethodID(rknnInputClass, "getBuf", "()[B");
		jmethodID getType=env->GetMethodID(rknnInputClass, "getType", "()Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_type;");
		jmethodID getSize=env->GetMethodID(rknnInputClass, "getSize", "()I");
		jmethodID getFmt=env->GetMethodID(rknnInputClass, "getFmt", "()Lcom/neocoretechs/rknn4j/RKNN$rknn_tensor_format;");
		jmethodID getPass_through=env->GetMethodID(rknnInputClass, "getPass_through", "()Z");
		jbyteArray jbuf = (jbyteArray)env->CallObjectMethod(input, getBuf);
		jbyte* b = env->GetByteArrayElements(jbuf, NULL);
		npuinputs[i].buf = (void*)b;
		// translate the enums via ordinal
		jobject tensorType = env->CallObjectMethod(input, getType);
		jclass tensorTypeClass = env->FindClass("com/neocoretechs/rknn4j/RKNN$rknn_tensor_type");
		jmethodID tensorTypeOrdinalMethod = env->GetMethodID(tensorTypeClass, "ordinal","()I");
		jint tensorTypeOrdinal = env->CallIntMethod(tensorType, tensorTypeOrdinalMethod);
		//printf("Tensor type ordinal=%d\n",tensorTypeOrdinal);
		// determine which enum corresponds to our java enum field
		switch(tensorTypeOrdinal) {
			case 0:
				npuinputs[i].type=RKNN_TENSOR_FLOAT32;
				break;
			case 1:
				npuinputs[i].type=RKNN_TENSOR_FLOAT16;
				break;
			case 2:
				npuinputs[i].type=RKNN_TENSOR_INT8;
				break;
			case 3:
				npuinputs[i].type=RKNN_TENSOR_UINT8;
				break;
			case 4:
				npuinputs[i].type=RKNN_TENSOR_INT16;
				break;
			case 5:
				npuinputs[i].type=RKNN_TENSOR_UINT16;
				break;
			case 6:
				npuinputs[i].type=RKNN_TENSOR_INT32;
				break;
			case 7:
				npuinputs[i].type=RKNN_TENSOR_UINT32;
				break;
			case 8:
				npuinputs[i].type=RKNN_TENSOR_INT64;
				break;
			case 9:
				npuinputs[i].type=RKNN_TENSOR_BOOL;
				break;
			case 10:
			default:
				return RKNN_ERR_FAIL;
		}
		jobject tensorFmt = env->CallObjectMethod(input, getFmt);
		jclass tensorFmtClass = env->FindClass("com/neocoretechs/rknn4j/RKNN$rknn_tensor_format");
		jmethodID tensorFmtOrdinalMethod = env->GetMethodID(tensorFmtClass, "ordinal","()I");
		jint tensorFmtOrdinal = env->CallIntMethod(tensorFmt, tensorFmtOrdinalMethod);
		//printf("Tensor fmt ordinal=%d\n",tensorFmtOrdinal);
		// determine which enum corresponds to our java enum field
		switch(tensorFmtOrdinal) {
			case 0:
				npuinputs[i].fmt=RKNN_TENSOR_NCHW;
				break;
			case 1:
				npuinputs[i].fmt=RKNN_TENSOR_NHWC;
				break;
			case 2:
				npuinputs[i].fmt=RKNN_TENSOR_NC1HWC2;
				break;
			case 3:
			case 4:
			default:
				return RKNN_ERR_FAIL;
		}
		npuinputs[i].size = env->CallIntMethod(input, getSize);
		npuinputs[i].pass_through = env->CallBooleanMethod(input, getPass_through) ? 1 : 0;
		//env->ReleaseByteArrayElements(jbuf, b, 0);
		npuinputs[i].index        = i;
		//npuinputs[i].type         = RKNN_TENSOR_INT8;
		//npuinputs[i].size         = 640 * 640 * 3;
		//npuinputs[i].fmt          = RKNN_TENSOR_NHWC;
		//npuinputs[i].pass_through = 0;

	}
	handle = dlopen("/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j/librknnrt.so", RTLD_NOW);
	if (!handle) {
		  printf("dlerr=%s\n",dlerror());
		  return RKNN_ERR_FAIL;
	}
	dlerror();
	nn_inputs_set =(int (*)(rknn_context, uint32_t, rknn_input[])) dlsym(handle, "rknn_inputs_set");
	dlerror();
	//printf("Calling function pointer %p\n",nn_inputs_set);
	ret = (*nn_inputs_set)((rknn_context)ctx, ninputs, npuinputs);
	dlclose(handle);
	return ret;
}

/*
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_run
 * Signature: (Lcom/neocoretechs/rknn4j/rknn_run_extend;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1run
  (JNIEnv* env, jobject thisObj, jlong ctx, jobject extend) {
	/* we will have to extract the extend structure and use it at some point */
	//ret = rknn_run(ctx, NULL);
	  handle = dlopen("/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j/librknnrt.so", RTLD_NOW);
	  if (!handle) {
		  printf("dlerr=%s\n",dlerror());
		  return RKNN_ERR_FAIL;
	  }
	  //printf("run init %p\n",handle);
	  dlerror();
	  nn_run =(int (*)(rknn_context, rknn_run_extend*)) dlsym(handle, "rknn_run");
	  //printf("function pointer run %p\n",nn_run);
	  dlerror();
	  ret = (*nn_run)((rknn_context)ctx, NULL);
	  dlclose(handle);
	return ret;
}

/*
 * rknn_outputs_get
 *
 * wait for the inference to finish and get the outputs. this function will block until inference finishes.
 * the results will set to outputs[].
 *   input:
 *       rknn_context context        the handle of context.
 *       uint32_t n_outputs          the number of outputs.
 *       rknn_output outputs[]       the arrays of output, see rknn_output.
 *       rknn_output_extend*         the extend information of output.
 *   return:
 *      int                         error code.
 *
 * int rknn_outputs_get(rknn_context context, uint32_t n_outputs, rknn_output outputs[], rknn_output_extend* extend);
 *
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_outputs_get
 * Signature: (I[Lcom/neocoretechs/rknn4j/rknn_output;Lcom/neocoretechs/rknn4j/rknn_output_extend;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1outputs_1get
  (JNIEnv* env, jobject thisObj, jlong ctx, jint noutputs, jobjectArray outputsArray, jobject extend) {
	rknn_output npuoutputs[noutputs];
	memset(npuoutputs, 0, sizeof(npuoutputs));
	for (int i = 0; i < noutputs; i++) {
		jobject output = env->GetObjectArrayElement(outputsArray, i);
		jclass rknnOutputClass=env->GetObjectClass(output);
		jmethodID getIndex=env->GetMethodID(rknnOutputClass, "getIndex", "()I");
		npuoutputs[i].index = env->CallIntMethod(output, getIndex);
		jmethodID getWant_float=env->GetMethodID(rknnOutputClass, "getWant_float", "()Z");
		npuoutputs[i].want_float = env->CallBooleanMethod(output, getWant_float) ? 1: 0;
		jmethodID getIs_prealloc=env->GetMethodID(rknnOutputClass, "getIs_prealloc", "()Z");
		npuoutputs[i].is_prealloc = env->CallBooleanMethod(output, getIs_prealloc) ? 1: 0;
	}
	handle = dlopen("/home/jg/npu/rknpu2-master/rknpu2-master/rknn4j/librknnrt.so", RTLD_NOW);
	if (!handle) {
		  printf("dlerr=%s\n",dlerror());
		  return RKNN_ERR_FAIL;
	}
	dlerror();
	nn_outputs_get =(int (*)(rknn_context, uint32_t, rknn_output[], rknn_output_extend*)) dlsym(handle, "rknn_outputs_get");
	dlerror();
	ret = (*nn_outputs_get)((rknn_context)ctx, noutputs, npuoutputs, NULL);
	dlclose(handle);
	// now transfer the values from the call
	if(ret >= RKNN_SUCC) {
		for (int i = 0; i < noutputs; i++) {
			jobject output = env->GetObjectArrayElement(outputsArray, i);
			jclass rknnOutputClass=env->GetObjectClass(output);
			jmethodID setBuf=env->GetMethodID(rknnOutputClass, "setBuf", "([B)V");
			jbyteArray jbuf = as_byte_array(env, (unsigned char*)npuoutputs[i].buf, npuoutputs[i].size);
			env->CallObjectMethod(output, setBuf, jbuf);
			jmethodID setSize=env->GetMethodID(rknnOutputClass, "setSize", "(I)V");
			env->CallObjectMethod(output, setSize, npuoutputs[i].size);
		}
	} else {
		return ret;
	}
	dlerror();
	nn_outputs_release =(int (*)(rknn_context, uint32_t, rknn_output[])) dlsym(handle, "rknn_outputs_release");
	dlerror();
	ret = (*nn_outputs_release)((rknn_context)ctx, noutputs, npuoutputs);
	dlclose(handle);
	return ret;
}

/*
 * rknn_outputs_release
 * We are choosing to do this as part of outputs_get due to access to actual buffers.
 * release the outputs obtained by rknn_outputs_get.
 * after calling, the rknn_output[x].buf get from rknn_outputs_get will
 * also be freed when rknn_output[x].is_prealloc = FALSE.
 *
 *   input:
 *       rknn_context context        the handle of context.
 *       uint32_t n_ouputs           the number of outputs.
 *       rknn_output outputs[]       the arrays of output.
 *   return:
 *       int                         error code
 *
 * int rknn_outputs_release(rknn_context context, uint32_t n_ouputs, rknn_output outputs[]);
 * Class:     com_neocoretechs_rknn4j_rknpu2
 * Method:    rknn_outputs_release
 * Signature: (I[Lcom/neocoretechs/rknn4j/rknn_output;)I
 */
JNIEXPORT jint JNICALL Java_com_neocoretechs_rknn4j_rknpu2_rknn_1outputs_1release
  (JNIEnv* env, jobject thisObj, jlong ctx, jint noutputs, jobjectArray outputsArray) {
	// ret = rknn_outputs_release(ctx, io_num.n_output, outputs);
	return ret;
}

