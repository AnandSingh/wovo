package com.anand.embvid.sudocam;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.res.Resources;


public class DetectSudo {
	public final static int BIG_ENDIAN = 0;
	public final static int LITTLE_ENDIAN = 1;
	static{
		System.loadLibrary("parsesudo");
	}
	
	
//	public native String stringFromJNI();
	public native boolean setSourceImage(int[] pixels, int width, int height);
	//public native void hello(int i);
	public native int[][] detectSudoku(int byteOrder);
	

}