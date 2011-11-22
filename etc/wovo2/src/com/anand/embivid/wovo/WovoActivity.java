package com.anand.embivid.wovo;



import com.google.ads.*;
import android.app.Activity;
import android.app.SearchManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
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
	//private AdView adView;

	

	


	

	@Override
	public void onBackPressed() {
	
    //Log.d(this.getClass().getName(), "back button pressed");
	// do something on back.
	ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
	if(flip == 1)
	{
		 super.onBackPressed();
	}
    if(flip == 2)
    {
    	
    	flip = 1;
		// Set an animation from res/animation: I pick push left in
		vf.setAnimation(AnimationUtils.loadAnimation(this.getApplicationContext(), R.anim.fade));
		vf.showPrevious();    	
    }
    if(flip == 3)
    {
    	flip = 1;
    	vf.showPrevious();   
    	vf.showPrevious();   
    }
   
	return;
	}
	
	@Override 
	public void onSaveInstanceState(Bundle outState) 
	{
		//adView.destroy();
		//---save whatever you need to persist—
		outState.putInt("view", Def0User1 );
		outState.putInt("flip", flip);
		super.onSaveInstanceState(outState); 
		//Lists.getInstance().saveLrnWordList();
	}
	
	@Override
	public void onRestoreInstanceState(Bundle savedInstanceState) 
	{
		super.onRestoreInstanceState(savedInstanceState);
		//---retrieve the information persisted earlier---
		//adView.loadAd(new AdRequest());
		Def0User1 = savedInstanceState.getInt("view");   
		flip = savedInstanceState.getInt("flip"); 
		if(flip == 2)
		{   
			// Get the ViewFlipper from the layout
			ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
			boolean view = false;
			if(Def0User1 == 0)
			{
				btnMem.setText(" I have memorized this word ! ");
				view = false;
			}else
			{
				btnMem.setText(" I hadn't memorized this word ! ");
				view = true;
			}
			
			//Lists.getInstance().setView(view);

			//if( true == InitWovo(view))
			{
				flip = 2;
				// Set an animation from res/animation:
					// vf.setAnimation(AnimationUtils.loadAnimation(this.getApplicationContext(), R.anim.push_left_in));
				vf.showNext();

			}
			//else
			{
				Context context = getApplicationContext();
				CharSequence text = " Fail to Load Database ";
				int duration = Toast.LENGTH_SHORT;

				Toast toast = Toast.makeText(context, text, duration);
				toast.show();
			}
		}
		if(flip == 3)
		{
			ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
			vf.showNext();
			vf.showNext();
		}
	}

	@Override
	public void onPause() {
		super.onPause();
		//Log.v("wovo", " +++ onPause +++");
//		Lists.getInstance().readWriteLrnWordList(1);
		//Lists.getInstance().readWriteLrnWordList(0);
	}
	
	@Override
	public void onResume() {
		super.onResume();
		//Log.v("wovo", " +++ onResume +++");
	//	Lists.getInstance().readWriteLrnWordList(0);
	}
	
	 @Override
	 public void onDestroy() {
	    //adView.destroy();
	    super.onDestroy();
	  }

   public void LoadSearchAcitvity(boolean view)
   {
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
   
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		Intent intent = getIntent();

		// from click on search results
		//Lists.getInstance().ensureLoaded(getResources());
		//    String word = intent.getDataString();

		//Log.d("wovo", intent.toString());
		if (intent.getExtras() != null) {
		//	Log.d("wovo", intent.getExtras().keySet().toString());
		}

		
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

		
		//mDictionary = new WordDatabase(getApplicationContext());

		//while(mDictionary.isLoaded() == false);
		// wiat till it load or checks the data base
		//while(Lists.getInstance().isLoaded() == false);


        ///for creating ads
	    // Create the adView
	    //adView = new AdView(this, AdSize.BANNER, "a14eb39eac9a049");

	    // Lookup your LinearLayout assuming it’s been given
	    // the attribute android:id="@+id/mainLayout"
	    //LinearLayout layout = (LinearLayout)findViewById(R.id.linearLayout1);

	    // Add the adView to it
	    //layout.addView(adView);
		//adView.loadAd(new AdRequest());
		
	
		
	    // for default screen
		btnDft.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				// Get the ViewFlipper from the layout
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
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				flip = 3;
				// Set an animation from res/animation: I pick push left in
				//vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
				vf.showNext();//showPrevious();
				vf.showNext();
				
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