package com.anand.embvid.wovo;



//import java.io.DataInputStream;
//import java.io.DataOutputStream;
//import java.io.EOFException;
import java.io.File;
import java.io.IOException;
//import java.io.FileInputStream;
//import java.io.IOException;

//import com.google.ads.*;
import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.app.SearchManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.SearchView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;

public class WovoActivity extends Activity {
    private static final String TAG = "WovoActivity";//"DictionaryDatabase";
    private static final String IDENTITY_1 = "1";
    private static final int MAX_WORDS = 4757;
	SharedPreferences app_pref = null;
	private Button btnRst;
	private Button btnYes;
	private Button btnNo;
	//private Button btnMain;
	private Button btnMem;
	private Button btnDft;
	private Button btnLrn;
    private ProgressBar progBar;

	private int Def0User1 = 0;
	private int flip = 1;

	private TextView tvWrd;
	private TextView tvDef;
	private TextView tvDebug;

	private int intLastLine = 0;
	private String line = null;
	private WordDatabase mDictionary;
    private Boolean databaseLoaded = false;
    
    private static final String LRN_FILE = "/data/data/com.anand.embvid.wovo/lrn.dat";
    
    ProgressThread progThread;
    ProgressDialog progDialog;
    
    int delay = 40;                  // Milliseconds of delay in the update loop
    int maxBarValue = 200;           // Maximum value of horizontal progress bar
   
   public void LoadSearchAcitvity(boolean view)
   {
	   databaseLoaded = app_pref.getBoolean("DB_LOADED", false);
	   if(databaseLoaded == false)
	   {
		   CharSequence text = "Loading Database ...";
		   Toast toast = Toast.makeText(this, text, Toast.LENGTH_SHORT);
		   toast.show();
	   }
	   else
	   {
		   CharSequence text = null;
		   boolean flag = false;
		
		   if(view == false)
		   {
			   if(app_pref.getInt("DEF_LINE_CNT", 0) == 0)
			   {
				   text = " No words found in main list ";
			   }
		   }else if(view == true)
		   {
			   if(app_pref.getInt("USR_LINE_CNT", 0) == 0)
			   {
				   text = " No words found in revision list ";
				   flag = true;
			   }
		   }

		   if(flag == false)
		   {
			   SharedPreferences.Editor editor = app_pref.edit();
			   editor.putBoolean("SCREEN_VIEW", view);
			   editor.commit();
			   Intent searchIntent = new Intent(WovoActivity.this, MainSearchWordActivity.class);
			   searchIntent.setData(getIntent().getData());
			   searchIntent.putExtra("VIEW", view);
			   WovoActivity.this.startActivity(searchIntent);
			   //finish();
		   }else if (flag == true)
		   {
			   Toast toast = Toast.makeText(this, text, Toast.LENGTH_SHORT);
			   toast.show();
		   }
	   }

   }

   private void prepareDatase() throws IOException {
	
		
		//wait if database loaded
		Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
	            String.valueOf(1));
		String[] selectionArgs = new String[] {uri.getLastPathSegment(), "1"};
	 	managedQuery(uri, null, null, selectionArgs, null);
	}
   
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		// Get the app's shared preferences
		app_pref = PreferenceManager.getDefaultSharedPreferences(this);

		btnRst = (Button) findViewById(R.id.button_reset);
		btnYes = (Button) findViewById(R.id.button_yes);
		btnNo = (Button) findViewById(R.id.button_no);
		
	    // btnMain = (Button) findViewById(R.id.button_main);
		btnDft = (Button) findViewById(R.id.button_default);
		btnLrn = (Button) findViewById(R.id.button_learned);
	
		tvWrd = (TextView) findViewById(R.id.word);
		tvDef = (TextView) findViewById(R.id.definition);
		tvDebug = (TextView) findViewById(R.id.num);
		progBar = (ProgressBar) findViewById(R.id.progressBar1);
	
		tvDebug.setText("");
		databaseLoaded = app_pref.getBoolean("DB_LOADED", false);
	
		Log.e(" >>>>>>>>>>>>>>", "Loading Data Base :" + databaseLoaded);
		if(false == databaseLoaded)
		{
			Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
		            String.valueOf(1));
			
			
			String[] selectionArgs = new String[] {uri.getLastPathSegment(), null ,"1"};
		 	managedQuery(uri, null, null, selectionArgs, null);
		 		
		 	showDialog(0);
			//app_context = this;
			/*CharSequence text = "Please wait .. \n First time preparing database";
			Toast toast = Toast.makeText(this, text, Toast.LENGTH_SHORT);
			toast.show();
	     	new Thread(new Runnable() {
            public void run() {
                try {
                    prepareDatase();
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
           }).start();
	     	while(false == app_pref.getBoolean("DB_LOADED", false));
		   toast = Toast.makeText(this, "Finish preparing database", Toast.LENGTH_SHORT);
			toast.show();*/
		}
		
		
		// for default screen
		btnDft.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				LoadSearchAcitvity(false);
			}
		});

		btnLrn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				LoadSearchAcitvity(true);
			}
		});

		/*private Handler handler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
		//	txt.setText("Processing Done");

			}
		};*/


		btnRst.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				
				databaseLoaded = app_pref.getBoolean("DB_LOADED", false);
				if(databaseLoaded == false)
				{
					
				}else{
				
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				flip = 3;
				// Set an animation from res/animation: I pick push left in
				//vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
				vf.showNext();//showPrevious();
				vf.showNext();
				}
				
			}
		});
		
		btnYes.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				/*progDailog = ProgressDialog.show(getApplicationContext(),
						"Reseting please wait....",
						true);*/
				//progBar.

				SharedPreferences.Editor editor = app_pref.edit();
				editor.putInt("DEF_LINE_CNT", MAX_WORDS);
				editor.putInt("USR_LINE_CNT", 0);
				editor.putInt("def_Line", 0);
				editor.putInt("user_Line", 0); 
				editor.commit();
					
				Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
			            String.valueOf(1));
				String[] selectionArgs = new String[] {uri.getLastPathSegment(), "2"};
				Cursor curr_cursor = managedQuery(uri, null, null, selectionArgs, null);
				
		    	//String[] selectionArgs1 = new String[] {uri.getLastPathSegment(), "1"};
		    	//Cursor def_cursor = managedQuery(uri, null, null, selectionArgs1, null);
		    	if(curr_cursor != null)
		    	{
		    		 curr_cursor.moveToFirst();
		    //		 Log.e(TAG, "" + curr_cursor.getCount());
		    		 //int i =0;
		    		 while(!curr_cursor.isAfterLast())
		    		 {
		    		  
		    			int wIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_WORD);
		    	
		    		    //i++;
		    		   	selectionArgs[0] = curr_cursor.getString(wIndex);
		    //		    Log.e(TAG, "Reset " + i + ". " + selectionArgs[0]);
				   		getContentResolver().update(WordDefinationProvider.CONTENT_URI, null, IDENTITY_1, selectionArgs);
		    		    curr_cursor.moveToNext();
		    		 }
		    		 curr_cursor.close();
		    		 
		    	}
		    	
		    	File lrn_file = new File(LRN_FILE);
				lrn_file.delete();
				Def0User1 = 0;
				flip = 1;
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				vf.showPrevious();
				vf.showPrevious();
			}
		});

		btnNo.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				flip = 1;
				// Set an animation from res/animation: I pick push left in
				vf.showPrevious();
				vf.showPrevious();
			}
		});

	}
	
    // Method to create a progress bar dialog of either spinner or horizontal type
    @Override
    protected Dialog onCreateDialog(int id) {
        //switch(id) {
        //case 0:                      // Spinner
            progDialog = new ProgressDialog(this);
            progDialog.setProgressStyle(ProgressDialog.STYLE_SPINNER);
            progDialog.setMessage("First Configration ...");
            progThread = new ProgressThread(handler);
            progThread.start();
            return progDialog;
        /*case 1:                      // Horizontal
            progDialog = new ProgressDialog(this);
            progDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
            progDialog.setMax(maxBarValue);
            progDialog.setMessage("Dollars in checking account:");
            progThread = new ProgressThread(handler);
            progThread.start();
            return progDialog;*/
        //default:
        //    return null;
        //}
    }
    
 // Handler on the main (UI) thread that will receive messages from the 
    // second thread and update the progress.
    
    final Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            // Get the current value of the variable total from the message data
            // and update the progress bar.
            int total = msg.getData().getInt("total");
            progDialog.setProgress(total);
            databaseLoaded = app_pref.getBoolean("DB_LOADED", false);
    		if ((total <= 0) || (databaseLoaded == true)){
                dismissDialog(0);
                progThread.setState(ProgressThread.DONE);
            }
        }
    };
    
    // Inner class that performs progress calculations on a second thread.  Implement
    // the thread by subclassing Thread and overriding its run() method.  Also provide
    // a setState(state) method to stop the thread gracefully.
    
    private class ProgressThread extends Thread {	
        
        // Class constants defining state of the thread
        final static int DONE = 0;
        final static int RUNNING = 1;
        
        Handler mHandler;
        int mState;
        int total;
    
        // Constructor with an argument that specifies Handler on main thread
        // to which messages will be sent by this thread.
        
        ProgressThread(Handler h) {
            mHandler = h;
        }
        
        // Override the run() method that will be invoked automatically when 
        // the Thread starts.  Do the work required to update the progress bar on this
        // thread but send a message to the Handler on the main UI thread to actually
        // change the visual representation of the progress. In this example we count
        // the index total down to zero, so the horizontal progress bar will start full and
        // count down.
        
        @Override
        public void run() {
            mState = RUNNING;   
            total = maxBarValue;
            while (mState == RUNNING) {
                // The method Thread.sleep throws an InterruptedException if Thread.interrupt() 
                // were to be issued while thread is sleeping; the exception must be caught.
                try {
                    // Control speed of update (but precision of delay not guaranteed)
                    Thread.sleep(delay);
                } catch (InterruptedException e) {
                   // Log.e("ERROR", "Thread was Interrupted");
                }
                
                // Send message (with current value of  total as data) to Handler on UI thread
                // so that it can update the progress bar.
                
                Message msg = mHandler.obtainMessage();
                Bundle b = new Bundle();
                b.putInt("total", total);
                msg.setData(b);
                mHandler.sendMessage(msg);
                
                total--;    // Count down
            }
        }
        
        // Set current state of thread (use state=ProgressThread.DONE to stop thread)
        public void setState(int state) {
            mState = state;
        }
    }
    
}