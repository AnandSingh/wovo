package com.anand.embivid.wovo;



import java.io.IOException;

import com.google.ads.*;
import android.app.Activity;
import android.app.SearchManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.SearchView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;

public class WovoActivity extends Activity {
    private static final String TAG = "WovoActivity";//"DictionaryDatabase";
	
	SharedPreferences app_pref = null;
	private Button btnRst;
	private Button btnYes;
	private Button btnNo;
	//private Button btnMain;
	private Button btnMem;
	private Button btnDft;
	private Button btnLrn;

	private int Def0User1 = 0;
	private int flip = 1;

	private TextView tvWrd;
	private TextView tvDef;
	private TextView tvDebug;

	private int intLastLine = 0;
	private String line = null;
	private WordDatabase mDictionary;
    private Boolean databaseLoaded = false;
	//private AdView adView;

   public void LoadSearchAcitvity(boolean view)
   {
	   databaseLoaded = app_pref.getBoolean("DB_LOADED", false);
		if(databaseLoaded == false)
		{
			CharSequence text = "Loading Database ...";
			Toast toast = Toast.makeText(this, text, Toast.LENGTH_SHORT);
			toast.show();
		}else{
	   CharSequence text = null;
	   Log.e(TAG, "LoadSearchAcitvity ++");
	   boolean flag = false;
	   Log.e(TAG, "+++++++++++++++++++++++++");
	   Log.e(TAG, "def: " + app_pref.getInt("DEF_LINE_CNT", 0));
	   Log.e(TAG, "usr: " + app_pref.getInt("USR_LINE_CNT", 0));
	   Log.e(TAG, "+++++++++++++++++++++++++");
	   
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
		//adView = (AdView) findViewById(R.id.adView);

		tvDebug.setText("");
		databaseLoaded = app_pref.getBoolean("DB_LOADED", false);
	
		if(false == databaseLoaded)
		{
			Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
		            String.valueOf(1));
			String[] selectionArgs = new String[] {uri.getLastPathSegment(), "1"};
		 	managedQuery(uri, null, null, selectionArgs, null);
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
				SharedPreferences.Editor editor = app_pref.edit();
				editor.putInt("def_Line", 1);
				editor.putInt("user_Line", -1); 
				editor.commit();
				//Lists.getInstance().reset();
				Def0User1 = 0;
				flip = 1;
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				//flip = 1;
				// Set an animation from res/animation: I pick push left in
				//vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
				vf.showPrevious();
				vf.showPrevious();
			}
		});

		btnNo.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				flip = 1;
				// Set an animation from res/animation: I pick push left in
				//vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
				vf.showPrevious();
				vf.showPrevious();
			}
		});

	}
}