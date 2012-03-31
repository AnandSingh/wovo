package com.anand.embvid.sudocam;

import java.io.IOException;
import java.util.List;

import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/** A basic Camera preview class */
public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {
    private SurfaceHolder mHolder;
    private Camera mCamera;

    public CameraPreview(Context context, Camera camera) {
        super(context);
        mCamera = camera;

        // Install a SurfaceHolder.Callback so we get notified when the
        // underlying surface is created and destroyed.
        mHolder = getHolder();
        mHolder.addCallback(this);
        // deprecated setting, but required on Android versions prior to 3.0
        mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }

    public void surfaceCreated(SurfaceHolder holder) {
        // The Surface has been created, now tell the camera where to draw the preview.
        try {
        	Parameters param = mCamera.getParameters();
    		Log.d("sudoCam >>>>>: ", " H: " +
    		param.getPictureSize().height + " , " +
    		" W: " +
    		param.getPictureSize().width +
    		param.getPreviewSize().toString() + " , " +
    		param.getPictureFormat());
    		param.setPictureSize(480, 640); //384, 640
    		param.setPreviewSize(144, 176);
    		param.setRotation(90);
    		param.setPictureFormat(ImageFormat.JPEG);
    		
    		Log.d("sudoCam >>>>>: ", " H: " +
    				param.getPictureSize().height + " , " +
    				" W: " +
    				param.getPictureSize().width + " , " + 
    				param.getPreviewSize().toString() + " , " +
    				param.getPictureFormat() + " , " +
    				param.getSupportedPreviewSizes());
    		
    		List<Camera.Size> ls = param.getSupportedPictureSizes();
    		
    		
    		for(int i =0; i< ls.size(); i++)
    		{
    			Camera.Size cs = ls.get(i);
    			Log.e("SUDOCAM : Preview SIZED  ", "w: " + cs.height  + "H: " + cs.width);
    		}
    		mCamera.setParameters(param);
    		
    	
    		
    		Parameters param1 = mCamera.getParameters();
    		
    		Log.d("sudoCam <<<<<<: ", " H: " +
    				param1.getPictureSize().height + " , " +
    				" W: " +
    				param1.getPictureSize().width + " , " + 
    				param1.getPreviewSize().toString() + " , " +
    				param1.getPictureFormat() + " , " +
    				param1.getSupportedPreviewSizes());
    		
        	mCamera.setPreviewDisplay(holder);
           
            mCamera.startPreview();
        } catch (IOException e) {
           Log.d("sudocam ", "Error setting camera preview: " + e.getMessage());
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        // empty. Take care of releasing the Camera preview in your activity.
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        // If your preview can change or rotate, take care of those events here.
        // Make sure to stop the preview before resizing or reformatting it.

        if (mHolder.getSurface() == null){
          // preview surface does not exist
          return;
        }

        // stop preview before making changes
        try {
            mCamera.stopPreview();
        } catch (Exception e){
          // ignore: tried to stop a non-existent preview
        }

        // set preview size and make any resize, rotate or
        // reformatting changes here

        // start preview with new settings
        try {
            mCamera.setPreviewDisplay(mHolder);
            mCamera.startPreview();

        } catch (Exception e){
            Log.d("sudocam ", "Error starting camera preview: " + e.getMessage());
        }
    }
}