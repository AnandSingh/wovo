

package com.anand.embvid.embvid;

//import java.io.IOException;
import android.app.Activity;
import android.content.Context;
//import android.app.AlertDialog;
//import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
//import android.widget.Toast;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.TextView;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnVideoSizeChangedListener;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.animation.AnimationUtils;
import android.widget.ViewFlipper;

public class tv extends Activity implements
OnBufferingUpdateListener, OnCompletionListener,
OnPreparedListener, OnVideoSizeChangedListener, SurfaceHolder.Callback{
	
    private static final String TAG = "--EmbTv--";
    
    
    
    private ListView lv1;
	//private String lv_arr[]={"Android","iPhone","BlackBerry","AndroidPeople"};
	
	private static final String[] country = { "Iceland", "India", "Indonesia",
		 "Iran", "Iraq", "Ireland", "Israel", "Italy", "Laos", "Latvia",
		 "Lebanon", "Lesotho ", "Liberia", "Libya", "Lithuania",
		 "Luxembourg" };
	private static final String[] curr = { "ISK", "INR", "IDR", "IRR", "IQD",
			 "EUR", "ILS", "EUR", "LAK", "LVL", "LBP", "LSL ", "LRD", "LYD",
			 "LTL ", "EUR"
	 };
    
	// Widgets in the application
    Button btnPlay;
    ImageButton btnPause;
    Button btnScan;
    // private Button btnStop;
    tv m_tv;
    
    private int mVideoWidth;
    private int mVideoHeight;
    private MediaPlayer mMediaPlayer;
    private SurfaceView mPreview;
    private SurfaceHolder holder;
    private String path = "/data/sample.3gp";//"/data/sample_mpeg4.mp4";
    private Bundle extras;
    private boolean mIsVideoSizeKnown = false;
    private boolean mIsVideoReadyToBePlayed = false;
    
    private static class EfficientAdapter extends BaseAdapter {
		private LayoutInflater mInflater;
		public EfficientAdapter(Context context) {
			mInflater = LayoutInflater.from(context);
		}
			 
		public int getCount() {
			return country.length;
		}
			 
		public Object getItem(int position) {
			return position;
		}
			 
		public long getItemId(int position) {
			return position;
		}
			 
		public View getView(int position, View convertView, ViewGroup parent) {
			ViewHolder holder;
			if (convertView == null) {
				convertView = mInflater.inflate(R.layout.listview, null);
				holder = new ViewHolder();
				holder.text = (TextView) convertView.findViewById(R.id.TextView01);
				holder.text2 = (TextView) convertView.findViewById(R.id.TextView02);
			 
				convertView.setTag(holder);
			 } else {
				 holder = (ViewHolder) convertView.getTag();
			 }
			 
			holder.text.setText(curr[position]);
			holder.text2.setText(country[position]);
			
			return convertView;
		}
			 
		static class ViewHolder {
			TextView text;
			TextView text2;
		}
	 }
	
    
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        Log.d(TAG, "Creatinging Buttons:");

        btnScan = (Button) findViewById(R.id.ScanButton);
        btnPlay = (Button) findViewById(R.id.PlayButton);
        btnPause = (ImageButton) findViewById(R.id.PauseButton);
        lv1=(ListView)findViewById(R.id.ListView01);
       	// By using setAdpater method in listview we an add string array in list.
        //lv1.setAdapter(new ArrayAdapter<String>(this,android.R.layout.simple_list_item_1 , lv_arr));
        lv1.setAdapter(new EfficientAdapter(this));

        // On app load, the Pause button is disabled
      // btnPause.setEnabled(false);
       m_tv = this;

       //btnStop = (Button) findViewById(R.id.btnStop);
       /*
        * ClickListener for the Calculate and Reset buttons. Depending on the
        * button clicked, the corresponding method is called.
        */
       OnClickListener mClickListener = new OnClickListener() {
       @Override
	   public void onClick(View v) {
			// TODO Auto-generated method stub
    	 // Get the ViewFlipper from the layout
         	       	
    		Log.d(TAG, "Click event !!");
         
	        switch (v.getId()) {
	        case R.id.ScanButton:
	        {
	        	//Set an animation from
	        	Log.d(TAG, "Calling Scan Button");
	        ViewFlipper vf = (ViewFlipper) findViewById(R.id.firstview);
            vf.setAnimation(AnimationUtils.loadAnimation(v.getContext(), R.anim.slide_left));
            vf.showNext();
	        }
	         break;
	        case R.id.PlayButton:
	        {
	        	Log.d(TAG, "Clicked Play Button");
	            ViewFlipper vf = (ViewFlipper) findViewById(R.id.firstview);
	            //Set an animation from 
	            vf.setAnimation(AnimationUtils.loadAnimation(v.getContext(), R.anim.slide_left));
	            vf.showNext();
	         }
	         break;
	        case R.id.btn1:
	        {
	         Log.d(TAG, "Calling Pause Function");
	       	   	ViewFlipper vf = (ViewFlipper) findViewById(R.id.Lastview);
	            //Set an animation from 
	            vf.setAnimation(AnimationUtils.loadAnimation(v.getContext(), R.anim.slide_left));
	            vf.showNext();
	            Play();
	        }
	        break;
	        case R.id.PauseButton:
	        {
	         Log.d(TAG, "Calling Pause Function");
	       	   	ViewFlipper vf = (ViewFlipper) findViewById(R.id.Lastview);
	            //Set an animation from 
	            vf.setAnimation(AnimationUtils.loadAnimation(v.getContext(), R.anim.slide_left));
	            vf.showNext();
	            Pause();
	        }
	         break;
	        /*case R.id.btnStop:
	         Stop();
	         break;*/
	        }
       }

	private void Pause() {
		// TODO Auto-generated method stub
		Log.d(TAG, "Pause ------------");
		mMediaPlayer.pause();
		//mMediaPlayer.stop();
	}

	private void Play() {
		// TODO Auto-generated method stub
		Log.d(TAG, "Play --------------" );
		//mMediaPlayer.start();
		if (mIsVideoReadyToBePlayed && mIsVideoSizeKnown) {
	            startVideoPlayback();
	        }
					
	}
       };
        
       Log.d(TAG, "Watching for Click");
       /* Attach listener to the Calculate and Reset buttons */
       btnPlay.setOnClickListener(mClickListener);
       btnPause.setOnClickListener(mClickListener);
       btnScan.setOnClickListener(mClickListener);
      Log.d(TAG, "get mPreview..");
   	  mPreview = (SurfaceView) findViewById(R.id.surface);
   	  Log.d(TAG, "get holder..");
      holder = mPreview.getHolder();
      Log.d(TAG, "get addCallback..");
      holder.addCallback(m_tv);
      Log.d(TAG, "settype holder..");
	  holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
	  Log.d(TAG, "get extras.. ");
	  extras = getIntent().getExtras();
    }    
    
    private void playVideo() {
    	
        doCleanUp();
        Log.d(TAG, "playVideo ++++");
        try {

         /*   if (path == "") {
                // Tell the user to provide a media file URL.
                Toast
                        .makeText(
                                tv.this,
                                "Please edit MediaPlayerDemo_Video Activity, "
                                        + "and set the path variable to your media file path."
                                        + " Your media file must be stored on sdcard.",
                                Toast.LENGTH_LONG).show();

            }*/
            // Create a new media player and set the listeners
            mMediaPlayer = new MediaPlayer();
            Log.e(TAG, "File to play" + path);
            mMediaPlayer.setDataSource(path);
            mMediaPlayer.setDisplay(holder);
            mMediaPlayer.prepare();
            mMediaPlayer.setOnBufferingUpdateListener(this);
            mMediaPlayer.setOnCompletionListener(this);
            mMediaPlayer.setOnPreparedListener(this);
            mMediaPlayer.setOnVideoSizeChangedListener(this);
            mMediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            Log.e(TAG, "Height: " + mMediaPlayer.getVideoHeight() + " Width: " + mMediaPlayer.getVideoWidth() + " Duration: " + mMediaPlayer.getDuration());


        } catch (Exception e) {
            Log.e(TAG, "error: " + e.getMessage(), e);
        }
    }

    public void onBufferingUpdate(MediaPlayer arg0, int percent) {
        Log.d(TAG, "onBufferingUpdate percent:" + percent);

    }

    public void onCompletion(MediaPlayer arg0) {
        Log.d(TAG, "onCompletion called");
    }

    public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {
        Log.v(TAG, "onVideoSizeChanged called W:" + width + " H:" + height);
        if (width == 0 || height == 0) {
            Log.e(TAG, "invalid video width(" + width + ") or height(" + height + ")");
            return;
        }
        mIsVideoSizeKnown = true;
        mVideoWidth = width;
        mVideoHeight = height;
        if (mIsVideoReadyToBePlayed && mIsVideoSizeKnown) {
            startVideoPlayback();
        }
    }

    public void onPrepared(MediaPlayer mediaplayer) {
        Log.d(TAG, "onPrepared called");
        mIsVideoReadyToBePlayed = true;
        if (mIsVideoReadyToBePlayed && mIsVideoSizeKnown) {
            startVideoPlayback();
        }
    }

    public void surfaceChanged(SurfaceHolder surfaceholder, int i, int j, int k) {
        Log.d(TAG, "surfaceChanged called");

    }

    public void surfaceDestroyed(SurfaceHolder surfaceholder) {
        Log.d(TAG, "surfaceDestroyed called");
    }


    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated called");
        playVideo();
    }

    @Override
    protected void onPause() {
    	Log.d(TAG, "onPause ---");
        super.onPause();
        releaseMediaPlayer();
        doCleanUp();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        releaseMediaPlayer();
        doCleanUp();
    }

    private void releaseMediaPlayer() {
        if (mMediaPlayer != null) {
            mMediaPlayer.release();
            mMediaPlayer = null;
       }
    }

    private void doCleanUp() {
        mVideoWidth = 0;
        mVideoHeight = 0;
        mIsVideoReadyToBePlayed = false;
        mIsVideoSizeKnown = false;
    }

    private void startVideoPlayback() {
        Log.v(TAG, "startVideoPlayback " + mVideoWidth + " " + mVideoHeight);
        holder.setFixedSize(mVideoWidth, mVideoHeight);
        mMediaPlayer.start();
    }

    
}
