#include <jni.h>
#include "com_xiaohuai_ndk_GetString.h"
#include "com_xiaohuai_ndk_GetInt.h"
#include "com_xiaohuai_ndk_ConvertBitmap.h"
#include <android/bitmap.h>
#include <android/log.h>

#define isLog false

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
} rgba;

class Point {
private:
	int x, y;
public:
	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}
	int getX() {
		return this->x;
	}
	int getY() {
		return this->y;
	}
};

JNIEXPORT jstring JNICALL Java_com_xiaohuai_ndk_GetString_getStr(JNIEnv *env,
		jclass jc) {

	return env->NewStringUTF("HelloNDK");
}

JNIEXPORT jint JNICALL Java_com_xiaohuai_ndk_GetInt_getInt(JNIEnv *, jclass) {
	Point *p = new Point(2, 2);
	jint x = p->getX();
	delete p;

	return x;
}

/*黑白处理*/
void BlackWhite(AndroidBitmapInfo info, void* pixelinfo) {
	jint i;
	jint ave;
	rgba * point = (rgba *) pixelinfo;

	for (i = 0; i < info.width * info.height; i++, point++) {
		ave = (point->red + point->green + point->blue) / 3;
		if (ave >= 128) {
			point->red = 255;
			point->green = 255;
			point->blue = 255;
		} else {
			point->red = 0;
			point->green = 0;
			point->blue = 0;
		}
	}
}
/*灰度处理*/
void ToGray(AndroidBitmapInfo info, void* pixelinfo) {
	jint i;
	jint color;
	rgba * point = (rgba *) pixelinfo;

	for (i = 0; i < info.width * info.height; i++, point++) {
		color = (uint8_t) (point->red * 0.299 + point->green * 0.587
				+ point->blue * 0.114);
		point->red = color;
		point->green = color;
		point->blue = color;
	}
}
/*底片效果*/
void Negative(AndroidBitmapInfo info, void* pixelinfo) {
	jint i;
	rgba * point = (rgba *) pixelinfo;

	for (i = 0; i < info.width * info.height; i++, point++) {
		point->red = 255 - point->red;
		point->green = 255 - point->green;
		point->blue = 255 - point->blue;
	}
}
/*浮雕效果*/
void Relief(AndroidBitmapInfo info, void* pixelinfo) {
	jint i, j;
	jint color, r, g, b;
	rgba curr_color = { 0, 0, 0, 0 };
	rgba leftColor = { 0, 0, 0, 0 };
	rgba * point = (rgba *) pixelinfo;

	for (int i = 0; i < info.height; i++) {
		for (int j = 0; j < info.width; j++, point++) {
			curr_color = *point;
			if (j == 0) {
				if (info.width > 1) {
					leftColor = *(point + 1);
				}
			}
			r = curr_color.red - leftColor.red + 128;
			g = curr_color.green - leftColor.green + 128;
			b = curr_color.blue - leftColor.blue + 128;

			color = (uint8_t) (r * 0.299 + g * 0.587 + b * 0.114);
			leftColor = curr_color;
			point->red = color;
			point->green = color;
			point->blue = color;

		}
	}
}

/*高斯模糊*/
void Blur(AndroidBitmapInfo info, void* srcpixel, void* dstpixel, jint radius) {
	jint i, j, p, q, num;
	jint r, g, b;
	rgba * srcpoint = (rgba *) srcpixel;
	rgba * dstpoint = (rgba *) dstpixel;

	for (i = 0; i < info.height; i++) {
		for (j = 0; j < info.width; j++, dstpoint++) {
			for (num = 0, r = 0, g = 0, b = 0, p = i - radius / 2;
					p <= i + radius / 2; p++) {
				if (p < 0 || p >= info.height) {
					continue;
				}
				for (q = j - radius / 2; q <= j + radius / 2; q++) {
					if (q < 0 || q >= info.width || (p == i && q == j)) {
						continue;
					}
					r += ((srcpoint + p * info.width + q)->red);
					g += ((srcpoint + p * info.width + q)->green);
					b += ((srcpoint + p * info.width + q)->blue);

					num++;
				}
			}
			#if isLog
				__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "dstpoint->alpha = %d\n" ,dstpoint->alpha);
				__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "dstpoint->red = %d\n" , dstpoint->red);
			#endif
			if (num > 0) {
				dstpoint->alpha = srcpoint->alpha;
				dstpoint->red = r / num;
				dstpoint->green = g / num;
				dstpoint->blue = b / num;
			}
		}
	}
}

/*放大镜效果*/
void Zoom(AndroidBitmapInfo info, void* srcpixel, void* dstpixel, jint zoom,
		jint radius, jint centerX, jint centerY) {
	jint i, j, distance;
	rgba * srcpoint = (rgba *) srcpixel;
	rgba * dstpoint = (rgba *) dstpixel;

	for (i = 0; i < info.height; i++) {
		for (j = 0; j < info.width; j++, dstpoint++) {
			dstpoint->alpha = (srcpoint+j+info.width*i)->alpha;
			dstpoint->red = (srcpoint+j+info.width*i)->red;
			dstpoint->green = (srcpoint+j+info.width*i)->green;
			dstpoint->blue = (srcpoint+j+info.width*i)->blue;

			distance = (j - centerX) * (j - centerX)
					+ (i - centerY) * (i - centerY);
			if (distance <= radius * radius && distance >= -radius * radius) {
				dstpoint->red = (srcpoint + centerX + (j - centerX) / zoom
						+ (centerY + (i - centerY) / zoom) * info.width)->red;
				dstpoint->green = (srcpoint + centerX + (j - centerX) / zoom
						+ (centerY + (i - centerY) / zoom) * info.width)->green;
				dstpoint->blue = (srcpoint + centerX + (j - centerX) / zoom
						+ (centerY + (i - centerY) / zoom) * info.width)->blue;
			}
		}
	}
}

JNIEXPORT jint JNICALL Java_com_xiaohuai_ndk_ConvertBitmap_convertBitmap(
		JNIEnv *env, jclass, jint which, jobject bmp) {
	AndroidBitmapInfo info;
	jint ret = AndroidBitmap_getInfo(env, bmp, &info);

	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "getInfo error");
		return ret;
	}

#if isLog
	__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "flags = %d\n", info.flags);
	__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "format = %d\n",
			info.format);
	__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "width = %d\n", info.width);
	__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "height = %d\n",
			info.height);
	__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "stride = %d\n",
			info.stride);
#endif

	void* pixelinfo;
	ret = AndroidBitmap_lockPixels(env, bmp, &pixelinfo);
	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "lockPixels error");
		return ret;
	}

	switch (which) {
	case 0:
		/*黑白处理*/
		BlackWhite(info, pixelinfo);
		break;
	case 1:
		/*灰度处理*/
		ToGray(info, pixelinfo);
		break;
	case 2:
		/*底片效果*/
		Negative(info, pixelinfo);
		break;
	case 3:
		/*浮雕效果*/
		Relief(info, pixelinfo);
		break;
	default:
		break;
	}
#if isLog
	__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "Convert Bmp Over!");
#endif

	AndroidBitmap_unlockPixels(env, bmp);

	return 1;
}

/*高斯模糊*/
JNIEXPORT jint JNICALL Java_com_xiaohuai_ndk_ConvertBitmap_blurBitmap(
		JNIEnv *env, jclass, jobject src, jobject dst, jint radius) {
	AndroidBitmapInfo src_info, dst_info;
	jint ret = AndroidBitmap_getInfo(env, src, &src_info);

	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "src getInfo error");
		return ret;
	}
	ret = AndroidBitmap_getInfo(env, dst, &dst_info);
	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "dst getInfo error");
		return ret;
	}
	void* src_pixelinfo;
	void* dst_pixelinfo;

	ret = AndroidBitmap_lockPixels(env, src, &src_pixelinfo);
	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "src lockPixels error");
		return ret;
	}

	ret = AndroidBitmap_lockPixels(env, dst, &dst_pixelinfo);
	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "dst lockPixels error");
		return ret;
	}
	/*高斯模糊*/
	Blur(src_info, src_pixelinfo, dst_pixelinfo, radius);
#if isLog
	__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "Convert Bmp Over!");
#endif
	AndroidBitmap_unlockPixels(env, src);
	AndroidBitmap_unlockPixels(env, dst);

	return 1;
}

/*放大镜效果*/
JNIEXPORT jint JNICALL Java_com_xiaohuai_ndk_ConvertBitmap_zoomBitmap(
		JNIEnv *env, jclass, jobject src, jobject dst, jint zoom, jint radius,
		jint centerX, jint centerY) {
	AndroidBitmapInfo src_info, dst_info;
	jint ret = AndroidBitmap_getInfo(env, src, &src_info);
	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "src getInfo error");
		return ret;
	}
	ret = AndroidBitmap_getInfo(env, dst, &dst_info);
	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "dst getInfo error");
		return ret;
	}
	void* src_pixelinfo;
	void* dst_pixelinfo;

	ret = AndroidBitmap_lockPixels(env, src, &src_pixelinfo);
	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "src lockPixels error");
		return ret;
	}

	ret = AndroidBitmap_lockPixels(env, dst, &dst_pixelinfo);
	if (ret < 0) {
		__android_log_print(ANDROID_LOG_ERROR, "Jiaqi", "dst lockPixels error");
		return ret;
	}
	/*放大镜效果*/
	Zoom(src_info, src_pixelinfo, dst_pixelinfo, zoom, radius, centerX,
			centerY);
#if isLog
	__android_log_print(ANDROID_LOG_INFO, "Jiaqi", "Convert Bmp Over!");
#endif

	AndroidBitmap_unlockPixels(env, src);
	AndroidBitmap_unlockPixels(env, dst);

	return 1;
}
