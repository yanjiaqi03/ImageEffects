package com.xiaohuai.hellondk;

import com.xiaohuai.ndk.ConvertBitmap;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;

public class MainActivity extends Activity {
	private ImageView ivOut1, ivOut2, ivOut3, ivOut4, ivOut5, ivOut6;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.activity_main);

		Bitmap dst1 = BitmapFactory.decodeResource(getResources(), R.drawable.demo);
		ConvertBitmap.convertBitmap(1, dst1);
		ivOut1 = (ImageView) findViewById(R.id.ivout1);
		ivOut1.setImageBitmap(dst1);

		Bitmap dst2 = BitmapFactory.decodeResource(getResources(), R.drawable.demo);
		ConvertBitmap.convertBitmap(2, dst2);
		ivOut2 = (ImageView) findViewById(R.id.ivout2);
		ivOut2.setImageBitmap(dst2);

		Bitmap dst3 = BitmapFactory.decodeResource(getResources(), R.drawable.demo);
		ConvertBitmap.convertBitmap(3, dst3);
		ivOut3 = (ImageView) findViewById(R.id.ivout3);
		ivOut3.setImageBitmap(dst3);

		Bitmap dst4 = BitmapFactory.decodeResource(getResources(), R.drawable.demo);
		ConvertBitmap.convertBitmap(0, dst4);
		ivOut4 = (ImageView) findViewById(R.id.ivout4);
		ivOut4.setImageBitmap(dst4);

		Bitmap src = BitmapFactory.decodeResource(getResources(), R.drawable.demo);
		Config config = Bitmap.Config.ARGB_8888;
		Bitmap dst5 = Bitmap.createBitmap(src.getWidth(), src.getHeight(), config);
		ConvertBitmap.blurBitmap(src, dst5, 11);
		ivOut5 = (ImageView) findViewById(R.id.ivout5);
		ivOut5.setImageBitmap(dst5);

		Bitmap dst6 = Bitmap.createBitmap(src.getWidth(), src.getHeight(), config);
		ConvertBitmap.zoomBitmap(src, dst6, 2, Math.min(dst6.getWidth(), dst6.getHeight()) / 2,
				Math.min(dst6.getWidth(), dst6.getHeight()) / 4, Math.min(dst6.getWidth(), dst6.getHeight()) / 4);
		ivOut6 = (ImageView) findViewById(R.id.ivout6);
		ivOut6.setImageBitmap(dst6);
	}

	static {
		System.loadLibrary("HelloNDK");
	}
}
