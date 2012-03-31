package com.anand.embvid.sudocam;


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.sql.Date;
import java.text.SimpleDateFormat;
import java.util.List;


import android.app.Activity;
import android.content.Intent;
//import android.graphics.Camera;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PictureCallback;
//import android.hardware.Camera.PictureCallback;
//import android.hardware.Camera.ShutterCallback;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Surface;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;

public class StartCamera extends Activity {
	
	private static final String TAG = "CameraDemo";
	
	private Camera mCamera;
	private CameraPreview mPreview;

	//Preview preview;
	Button buttonClick;
	

    private static final int CAMERA_PIC_REQUEST = 1337;  
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.gallery);
		
		Intent cameraIntent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE);  
        startActivityForResult(cameraIntent, CAMERA_PIC_REQUEST); 
    	
		//this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		/*
		int rotation = getWindowManager().getDefaultDisplay()
	             .getRotation();
	     int degrees = 0;
	     switch (rotation) {
	         case Surface.ROTATION_0: degrees = 0; break;
	         case Surface.ROTATION_90: degrees = 90; break;
	         case Surface.ROTATION_180: degrees = 180; break;
	         case Surface.ROTATION_270: degrees = 270; break;
	     }

		Log.e("sudocam::::", "    " + degrees);
		
		result = (info.orientation - degrees + 360) % 360;

        mCamera.setDisplayOrientation(result);*/

	     
		// Create an instance of Camera
        /*
        mCamera = Camera.open();
        
		Parameters param = mCamera.getParameters();
		Log.d("sudoCam >>>>>: ", " H: " +
		param.getPictureSize().height + " , " +
		" W: " +
		param.getPictureSize().width +
		param.getPreviewSize().toString() + " , " +
		param.getPictureFormat());
		param.setPictureSize(384, 640);
		param.setPreviewSize(384, 640);
		param.setRotation(90);
		param.setPictureFormat(ImageFormat.JPEG);
		
		Log.d("sudoCam >>>>>: ", " H: " +
				param.getPictureSize().height + " , " +
				" W: " +
				param.getPictureSize().width + " , " + 
				param.getPreviewSize().toString() + " , " +
				param.getPictureFormat() + " , " +
				param.getSupportedPreviewSizes());
		List ls = param.getSupportedPreviewSizes();
		for(int i =0; i< ls.size(); i++)
		{
			Log.e("SUDOCAM : Preview SIZED  ", "" + ls.get(i).toString());
		}
		//param.setPictureSize(384, 640);
		//param.setPreviewSize(384, 640);
		mCamera.setParameters(param);
		//mCamera.setDisplayOrientation(90);
		//preview = new Preview(this);
		//((FrameLayout) findViewById(R.id.preview)).addView(preview);
        
     // Create our Preview view and set it as the content of our activity.
        mPreview = new CameraPreview(this, mCamera);
      ((FrameLayout) findViewById(R.id.preview)).addView(mPreview);

		buttonClick = (Button) findViewById(R.id.buttonClick);
		buttonClick.setOnClickListener( new OnClickListener() {
			public void onClick(View v) {
				//preview.camera.takePicture(shutterCallback, rawCallback, jpegCallback);
				 // get an image from the camera
	            mCamera.takePicture(null, null, jpegCallback);

			}
		});
		Log.d(TAG, "onCreate'd");
		*/
	}
	
	   protected void onActivityResult(int requestCode, int resultCode, Intent data) {  
           if (requestCode == CAMERA_PIC_REQUEST) {  
           	Log.e("TAG", "onActivityResult++++ ");
            Bitmap thumbnail = (Bitmap) data.getExtras().get("data");  
            ImageView image = (ImageView) findViewById(R.id.sudoku_image);  
            image.setImageBitmap(thumbnail);  
               // do something  
           }  
       } 
	
	   @Override
	    protected void onPause() {
	        super.onPause();
	        if (mCamera != null){
	            mCamera.release();        // release the camera for other applications
	            mCamera = null;
	        }

	    }

	
	/*
	ShutterCallback shutterCallback = new ShutterCallback() {
		public void onShutter() {
			Log.d(TAG, "onShutter'd");
		}
	};
*/

	
	// Handles data for raw picture 
	/*PictureCallback rawCallback = new PictureCallback() {
		public void onPictureTaken(byte[] data, Camera camera) {
			Log.d(TAG, "onPictureTaken - raw");
		}
	};*/
	
	/** Create a File for saving an image or video */
	private static File getOutputMediaFile( ){
	    // To be safe, you should check that the SDCard is mounted
	    // using Environment.getExternalStorageState() before doing this.
		Log.e("sudocam :  ", "++++++++++++++++++++++++++++++++++++++++++++++++");
        
		
		Log.e("sudocam :  ", Environment.getExternalStorageState());
		File rootsd = Environment.getExternalStorageDirectory();
		//File dcim = new File(rootsd.getAbsolutePath() + "/DCIM");

		Log.e("sudocam:+++++++++++++ ", "" + rootsd.toString() + " ::  " + rootsd.getPath() + " :: " + rootsd.getAbsolutePath());
	    //File mediaStorageDir = new File(Environment.getExternalStoragePublicDirectory(
	            //  Environment.DIRECTORY_PICTURES), "Train");
	    // This location works best if you want the created images to be shared
	    // between applications and persist after your app has been uninstalled.

	    // Create the storage directory if it does not exist
	  
		/*if (! mediaStorageDir.exists()){
	        if (! mediaStorageDir.mkdirs()){
	            Log.d("sudocam", "failed to create directory");
	            return null;
	        }
	    }*/
        
	    // Create a media file name
	    //Date d = new Date();
	    String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(System.currentTimeMillis());
	    Log.d("sudocam: >>>>>>", timeStamp);
	    File mediaFile;
	    //if (type == MEDIA_TYPE_IMAGE){
	        mediaFile = new File(rootsd.getAbsolutePath() + File.separator +/* String.format("/sdcard/Train/") +*/
	        "SUDO_"+ timeStamp + ".jpg");
	    //} else if(type == MEDIA_TYPE_VIDEO) {
	     //   mediaFile = new File(mediaStorageDir.getPath() + File.separator +
	     //   "VID_"+ timeStamp + ".mp4");
	    //} else {
	    //    return null;
	    //}

	    return mediaFile;
	}
	
	// Handles data for jpeg picture
	PictureCallback jpegCallback = new PictureCallback() {
		
		@Override
		public void onPictureTaken(byte[] data, Camera camera) {
			
			 File pictureFile = getOutputMediaFile();
		        if (pictureFile == null){
		            Log.d(TAG, "Error creating media file, check storage permissions: " /* +
		                e.getMessage()*/);
		            return;
		        }

		        try {
		            FileOutputStream fos = new FileOutputStream(pictureFile);
		            fos.write(data);
		            fos.close();
		        } catch (FileNotFoundException e) {
		            Log.d(TAG, "File not found: " + e.getMessage());
		        } catch (IOException e) {
		            Log.d(TAG, "Error accessing file: " + e.getMessage());
		        }

			
			/*FileOutputStream outStream = null;
			try {
				// write to local sandbox file system
//				outStream = CameraDemo.this.openFileOutput(String.format("%d.jpg", System.currentTimeMillis()), 0);	
				// Or write to sdcard
				outStream = new FileOutputStream(String.format("/sdcard/Train/%d.jpg", System.currentTimeMillis()));	
				outStream.write(data);
				outStream.close();
				Log.d(TAG, "onPictureTaken - wrote bytes: " + data.length);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} finally {
			}
			Log.d(TAG, "onPictureTaken - jpeg");*/
		}
	};
	
}
