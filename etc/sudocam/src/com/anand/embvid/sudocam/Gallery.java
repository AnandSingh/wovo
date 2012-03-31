package com.anand.embvid.sudocam;


import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteOrder;


import android.app.Activity;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.hardware.Camera.ShutterCallback;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore.Images;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;

public class Gallery extends Activity implements OnClickListener{
	
	
	private static final int ACTIVITY_SELECT_IMAGE = 1;
	private String mCurrentImagePath = null;
	Bitmap bitmap = null;
	private DetectSudo sd = new DetectSudo();
	private int byteOrder = -1;
	private int[][] sudoku = new int[9][9];
	
	private static final String TAG = "CameraDemo";

	Button buttonClick;
	
	private static final int CAMERA_PIC_REQUEST = 1337;  
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.gallery);
		Bundle bundle = getIntent().getExtras();
		if (bundle.getBoolean("VIEW") == true)
		{
			// start gallery
			Intent cameraIntent = new Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE);  
	        startActivityForResult(cameraIntent, CAMERA_PIC_REQUEST); 
		}else
		{
			// start camera
			Intent galleryIntent = new Intent(Intent.ACTION_PICK, Images.Media.INTERNAL_CONTENT_URI);
		    startActivityForResult(galleryIntent, ACTIVITY_SELECT_IMAGE);
	
		}

	    View detectButton = this.findViewById(R.id.grab_button);
	    detectButton.setOnClickListener(this);
        View rotateButton = this.findViewById(R.id.rotate_button);
        rotateButton.setOnClickListener(this);
        
                
		//buttonClick = (Button) findViewById(R.id.grab_button);
	
  /*	buttonClick.setOnClickListener( new OnClickListener() {
			public void onClick(View v) {
				
				if(bitmap!=null){
					int width = bitmap.getWidth();
					int height = bitmap.getHeight();
					
					int newWidth = 384;
					int newHeight = 640;
					
					float scaleWidth = ((float) newWidth) / width;
			        float scaleHeight = ((float) newHeight) / height;
					
			        
		
			    
					Log.e(" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> >", "W: " + scaleWidth + ", H: "+scaleHeight );
					
					// createa matrix for the manipulation
			        Matrix matrix = new Matrix();
			        // resize the bit map
			        matrix.postScale(scaleWidth, scaleHeight);
			        // rotate the Bitmap
			        matrix.postRotate(45);
			        
			        // recreate the new Bitmap
			        Bitmap resizedBitmap = Bitmap.createBitmap(bitmap, 0, 0, 
			                          width, height, matrix, true); 
					
					
					
					
					
					Log.e(" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>", "W: " + resizedBitmap.getWidth() + ", H: "+ resizedBitmap.getHeight() );
					
					ImageView imageView = (ImageView) findViewById(R.id.sudoku_image);
					imageView.setImageBitmap(resizedBitmap);
					
					
				
			}
		}
				
		});*/

		Log.d(TAG, "onCreate'd");
	}
	
		
	@Override
	public void onClick(View v) {
		
		switch(v.getId()){
		case R.id.grab_button:
			if(bitmap!=null){
				int width = bitmap.getWidth();
				int height = bitmap.getHeight();
				
				int newWidth = 384;
				int newHeight = 640;
				
				float scaleWidth = ((float) newWidth) / width;
		        float scaleHeight = ((float) newHeight) / height;
					    
				Log.e(" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> >", "W: " + scaleWidth + ", H: "+scaleHeight );
				
				// createa matrix for the manipulation
		        Matrix matrix = new Matrix();
		        // resize the bit map
		        matrix.postScale(scaleWidth, scaleHeight);
		        // rotate the Bitmap
		        //matrix.postRotate(45);
		        
		        // recreate the new Bitmap
		        Bitmap resizedBitmap = Bitmap.createBitmap(bitmap, 0, 0, 
		                          width, height, matrix, true); 
				
		        if (bitmap != resizedBitmap) {
					bitmap.recycle();
					bitmap = resizedBitmap;
				}
				
				
				
				Log.e(" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>", "W: " + bitmap.getWidth() + ", H: "+ bitmap.getHeight() );
				
				ImageView imageView = (ImageView) findViewById(R.id.sudoku_image);
				imageView.setImageBitmap(bitmap);
				width = bitmap.getWidth();
				height = bitmap.getHeight();
				int[] pixels = new int[width * height];
				bitmap.getPixels(pixels, 0, width, 0, 0, width, height);
				sd.setSourceImage(pixels, width, height);
				getByteOrder();
				
				Log.e("ANAND", "+++++++++++++++++++++++++++++++++++++++++++");
				
				if(byteOrder != -1){
					sudoku = sd.detectSudoku(byteOrder);
									
					Intent intent = new Intent(Gallery.this, Game.class);
			        intent.putExtra(Game.KEY_PUZZLE, setPuzzle(sudoku));
			        startActivity(intent);
					//Intent i = new Intent(this,SudokuGrabber.class);
					//i.putExtra(SudokuGrabber.KEY_PUZZLE, setPuzzle(sudoku));
					//startActivity(i);
				}
			
		}
			break;
		case R.id.rotate_button:
			if(bitmap!=null){
				//bitmap = Utility.rotate(bitmap, 90);
				int width = bitmap.getWidth();
				int height = bitmap.getHeight();
				Matrix m = new Matrix();
				m.setRotate(90, (float) width / 2,
						(float) height / 2);
				try {
					Bitmap b2 = Bitmap.createBitmap(bitmap, 0, 0,width,
							height, m, true);
					if (bitmap != b2) {
						bitmap.recycle();
						bitmap = b2;
					}
				} catch (OutOfMemoryError ex) {
					// We have no memory to rotate. Return the original bitmap.
				}
				Log.e(" >>>>>>>>>>>>>>>>>>>>>>>>>>>>>", "W: " + bitmap.getWidth() + ", H: "+ bitmap.getHeight() );
				ImageView imageView = (ImageView) findViewById(R.id.sudoku_image);
				imageView.setImageBitmap(bitmap);
			}
			break;
		default:
			break;
		}
	}

	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		 if (requestCode == CAMERA_PIC_REQUEST) {  
	          	Log.e("TAG", "onActivityResult++++ ");
	           bitmap = (Bitmap) data.getExtras().get("data");  
	           ImageView image = (ImageView) findViewById(R.id.sudoku_image);  
	           image.setImageBitmap(bitmap);  
	          }  

		if (requestCode == ACTIVITY_SELECT_IMAGE && resultCode == RESULT_OK) {
			try {
				Uri currImageURI = data.getData();
				String[] proj = { Images.Media.DATA, Images.Media.ORIENTATION };
				Cursor cursor = managedQuery(currImageURI, proj, null, null, null);
				int columnIndex = cursor.getColumnIndex(proj[0]);
				cursor.moveToFirst();
				mCurrentImagePath = cursor.getString(columnIndex);
				bitmap = BitmapFactory.decodeFile(mCurrentImagePath);
				ImageView imageView = (ImageView) findViewById(R.id.sudoku_image);
				imageView.setImageBitmap(bitmap);
			} catch (Exception e) {
			}
		}
		
	}
	public int[] setPuzzle(int sudoku[][]){
		int puzzle[] = new int[81];
		for(int i=0;i<9;i++){
			for(int j=0; j<9; j++){
				if(sudoku[i][j]==-1){
					return null;
				}
				puzzle[i*9+j] = sudoku[i][j];
				Log.e("-----", " "+sudoku[i][j]);
			}
		}
		return puzzle;
	}
	private void getByteOrder(){
		String order = ByteOrder.nativeOrder().toString();
		if(order.equals(ByteOrder.BIG_ENDIAN.toString())){
			byteOrder = DetectSudo.BIG_ENDIAN;
		}else if(order.equals(ByteOrder.LITTLE_ENDIAN.toString())){
			byteOrder = DetectSudo.LITTLE_ENDIAN;
		}else{
			byteOrder = -1;
		}
	}

}
