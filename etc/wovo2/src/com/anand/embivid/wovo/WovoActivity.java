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

	

	


	public boolean InitWovo(boolean View)
	{
		//get the last time used line number 
		if(true  == View)
		{
			Def0User1 = 1;
			intLastLine = app_pref.getInt("user_Line", -1);
			//Log.d("wovo", "user ......" + intLastLine);
			// no previous saved length
			if(intLastLine == -1)
			{
				if(true == Lists.getInstance().isAnythingLrn())
				{
					//check if user has saved any new word 
					intLastLine = 1;
				}
				else
				{
					intLastLine = 0;
				}
			}

		}else
		{
			Def0User1 = 0; 
			intLastLine = app_pref.getInt("def_Line", 1);
			//Log.d("wovo", "default ......" + intLastLine);
		}
		//Log.d("wovo", "LastWord line number : " + intLastLine );

		if(Lists.getInstance().getTotalCount() == 0)
		{
			if(Def0User1 == 0)
			{
				
			}
			if(Def0User1 == 1)
			{
				
			}
			return false;
		}
		// set the default text based on the last prefrence value
		line = Lists.getInstance().setLineCount(intLastLine);

		
		if(line != null)
		{
			Lists.getInstance().splitText(line);
			tvWrd.setText(Lists.getInstance().getWord());
			tvDef.setText(Lists.getInstance().getDefine());
			int curr_line = Lists.getInstance().getLineCount();
			int total_line = Lists.getInstance().getTotalCount();

			tvDebug.setText(String.valueOf(curr_line) + " / " + String.valueOf(total_line));
			//tvDebug.setText(String.valueOf(curr_line));
			return true;
		}else
		{
			return false;
		}
	}

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
			
			Lists.getInstance().setView(view);

			if( true == InitWovo(view))
			{
				flip = 2;
				// Set an animation from res/animation:
					// vf.setAnimation(AnimationUtils.loadAnimation(this.getApplicationContext(), R.anim.push_left_in));
				vf.showNext();

			}
			else
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
		Lists.getInstance().readWriteLrnWordList(1);
		//Lists.getInstance().readWriteLrnWordList(0);
	}
	
	@Override
	public void onResume() {
		super.onResume();
		//Log.v("wovo", " +++ onResume +++");
		Lists.getInstance().readWriteLrnWordList(0);
	}
	
	 @Override
	 public void onDestroy() {
	    //adView.destroy();
	    super.onDestroy();
	  }

   public void LoadSearchAcitvity(boolean view)
   {
	   Log.e(TAG, "LoadSearchAcitvity ++");
	   Intent searchIntent = new Intent(WovoActivity.this, MainSearchWordActivity.class);
	   searchIntent.setData(getIntent().getData());
	   searchIntent.putExtra("VIEW", view);
	   //searchIntent.setAction(Intent.ACTION_VIEW);
	   //searchIntent.putExtra("some_other_key", "a value");
	   WovoActivity.this.startActivity(searchIntent);
	   //finish();
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
			
				WovoActivity.this.LoadSearchAcitvity(false);
				

			        
			/*	Lists.getInstance().setView(false);
				if(Lists.getInstance().getTotalCount() == 0)
				{
					CharSequence text = " No words Found in Main List ";
					int duration = Toast.LENGTH_SHORT;
					Toast toast = Toast.makeText(view.getContext(), text, duration);
					toast.show();
					//ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
					//flip = 1;
					// Set an animation from res/animation: I pick push left in
					//vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
					//vf.showPrevious();
					return;
					
				}
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				btnMem.setText(" I have memorized this word ! ");
				
				if( true == InitWovo(false))
				{
					flip = 2;
					// Set an animation from res/animation:
					vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.push_left_in));
					vf.showNext();
				}
				else
				{
					Context context = getApplicationContext();
					CharSequence text = " Fail to Load Database ";
					int duration = Toast.LENGTH_SHORT;

					Toast toast = Toast.makeText(context, text, duration);
					toast.show();
				}*/

			}
		});

		
		  
		btnLrn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				// Get the ViewFlipper from the layout
				// handles a click on a search suggestion; launches activity to show word
				//setIntent(getIntent());
				
				LoadSearchAcitvity(true);
				

				
				//Lists.getInstance().setView(true);
				/*
				if(Lists.getInstance().getTotalCount() == 0)
				{
					CharSequence text = " No words found in revision list ";
					int duration = Toast.LENGTH_SHORT;
					Toast toast = Toast.makeText(view.getContext(), text, duration);
					toast.show();
					//ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
					//flip = 1;
					// Set an animation from res/animation: I pick push left in
					//vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
					//vf.showPrevious();
					return;
					
				}
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				
				btnMem.setText(" I hadn't memorized this word ! ");
				if(true == InitWovo(true))
				{
					flip = 2;
					// Set an animation from res/animation: I pick push left in
					vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.push_left_in));
					vf.showNext();
					  // Initiate a generic request to load it with an ad
				    //adView.loadAd(new AdRequest());
				}else
				{
					Context context = getApplicationContext();
					CharSequence text = " No words found in revision list ";
					int duration = Toast.LENGTH_SHORT;

					Toast toast = Toast.makeText(context, text, duration);
					toast.show();
				}
*/
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
				Lists.getInstance().reset();
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