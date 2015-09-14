package com.xiaohuai.ndk;

public class ConvertBitmap {
	public static native int convertBitmap(int which,Object bmp);
	public static native int blurBitmap(Object src,Object bmp,int radius);
	public static native int zoomBitmap(Object src,Object bmp,int zoom,int radius,int centerX,int centerY);
}
