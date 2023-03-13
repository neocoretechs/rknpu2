#include "com_neocoretechs_rknn4j_image_Instance.h"
#include "RgaUtils.h"
#include "im2d.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
//#include "opencv2/videoio.hpp"
#include "rga.h"
  // init rga context
  rga_buffer_t src;
  rga_buffer_t dst;
  im_rect      src_rect;
  im_rect      dst_rect;
  /*
  int bcapture0 = 1;
  int bcapture1 = 1;
  int bcapture2 = 1;
  int bcapture3 = 1;
  cv::VideoCapture capture0;
  cv::VideoCapture capture1;
  cv::VideoCapture capture2;
  cv::VideoCapture capture3;
  */

  jbyteArray as_byte_array(JNIEnv* env, unsigned char* buf, int len) {
      jbyteArray array = env->NewByteArray(len);
      env->SetByteArrayRegion(array, 0, len, (jbyte*)(buf));
      return array;
  }

/*
 * Class:     com_neocoretechs_rknn4j_image_Instance
 * Method:    getRGB
 * Signature: ([BIIIII)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_neocoretechs_rknn4j_image_Instance_getRGB
  (JNIEnv *env, jobject jobj, jbyteArray byteData, jint img_width, jint img_height, jint channel, jint width, jint height) {
		jbyte* _b_data= env->GetByteArrayElements(byteData, 0);
		cv::Mat mdata(img_height, img_width, CV_8UC3, (unsigned char *)_b_data);
		cv::Mat orig_img = imdecode(mdata,1);
		cv::Mat img;
		cv::cvtColor(orig_img, img, cv::COLOR_BGR2RGB);
		if (img_width != width || img_height != height) {
			cv::Mat newimg = img.clone();
			//printf("resize %d %d to %d %d\n", img.cols, img.rows, width, height);
			cv::resize(img, newimg, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);
			env->ReleaseByteArrayElements(byteData, _b_data, 0);
			return as_byte_array(env, newimg.data, height*width*channel);
		}
		env->ReleaseByteArrayElements(byteData, _b_data, 0);
		// return img as java byte array
		return as_byte_array(env, img.data, height*width*channel);

}

/*
 * Class:     com_neocoretechs_rknn4j_image_Instance
 * Method:    getRGARGB
 * Signature: ([BIIIII)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_neocoretechs_rknn4j_image_Instance_getRGARGB
  (JNIEnv *env, jobject jobj, jbyteArray byteData, jint img_width, jint img_height, jint channel, jint width, jint height) {
		memset(&src_rect, 0, sizeof(src_rect));
		memset(&dst_rect, 0, sizeof(dst_rect));
		memset(&src, 0, sizeof(src));
		memset(&dst, 0, sizeof(dst));
		jbyte* _b_data= env->GetByteArrayElements(byteData, 0);
		cv::Mat mdata(img_height, img_width, CV_8UC3, (unsigned char *)_b_data);
		cv::Mat orig_img = imdecode(mdata,1);
		cv::Mat img;
		cv::cvtColor(orig_img, img, cv::COLOR_BGR2RGB);
		void* resize_buf = nullptr;
		if (img_width != width || img_height != height) {
			//printf("resize %d %d to %d %d\n", img.cols, img.rows, width, height);
			/* must be aligned to stride 16 for RGA resize */
			/*printf("resize with RGA!\n");*/
			resize_buf = malloc(height * width * channel);
			memset(resize_buf, 0x00, height * width * channel);

			src = wrapbuffer_virtualaddr((void*)img.data, img_width, img_height, RK_FORMAT_RGB_888);
			dst = wrapbuffer_virtualaddr((void*)resize_buf, width, height, RK_FORMAT_RGB_888);
			int ret = imcheck(src, dst, src_rect, dst_rect);
			if (IM_STATUS_NOERROR != ret) {
				printf("%d, check error! %s", __LINE__, imStrError((IM_STATUS)ret));
				return NULL;
			}
			IM_STATUS STATUS = imresize(src, dst);

			// for debug
			//cv::Mat resize_img(cv::Size(width, height), CV_8UC3, resize_buf);
			//cv::imwrite("resize_input.jpg", resize_buf);
			env->ReleaseByteArrayElements(byteData, _b_data, 0);
			// return resize_img as java byte array
			jbyteArray retArray = as_byte_array(env, (unsigned char*)resize_buf, height*width*channel);
			free(resize_buf);
			img.release();
			orig_img.release();
			mdata.release();
			return retArray;
		}
		env->ReleaseByteArrayElements(byteData, _b_data, 0);
		// return img as java byte array
		jbyteArray retArray = as_byte_array(env, img.data, height*width*channel);
		img.release();
		orig_img.release();
		mdata.release();
		return retArray;
}

/*
 * Class:     com_neocoretechs_rknn4j_image_Instance
 * Method:    getCapture
 * Signature: (I)[B

JNIEXPORT jbyteArray JNICALL Java_com_neocoretechs_rknn4j_image_Instance_getCapture
  (JNIEnv *env, jobject jobj, jint camdevice) {
	cv::VideoCapture capture;
	switch(camdevice) {
	  case -1:
		if(!bcapture0)
			capture0.release();
		if(!bcapture1)
			capture1.release();
		if(!bcapture2)
			capture2.release();
		if(!bcapture3)
			capture3.release();
		return NULL;
	  case 0:
		capture = capture0;
		if(bcapture0) {
			 capture.open(0);
			 bcapture0 = 0;
		}
		break;
	  case 1:
		capture = capture1;
		if(bcapture1) {
			 capture.open(1);
			 bcapture1 = 0;
		}
		break;
	  case 2:
		capture = capture2;
		if(bcapture2) {
			 capture.open(2);
			 bcapture2 = 0;
		}
		break;
	  case 3:
		capture = capture3;
		if(bcapture3) {
			 capture.open(3);
			 bcapture3 = 0;
		}
		break;
	  default:
		printf("Unknown camera device %d\n",camdevice);
		return NULL;
	}
	if (!capture.isOpened()) {
		printf("Failed to open webcam %d!",camdevice);
		return NULL;
	}

	cv::Mat frame;
	cv::Size size(640, 480); // set desired frame size

	capture.read(frame);
	//cv::resize(frame, frame, size); // resize the frame
	jbyteArray jbytes = as_byte_array(env, frame.data, 640*480*3);
	frame.release();
	return jbytes;
}
*/
