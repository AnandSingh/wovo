package com.anand.embivid.wovo;





import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;

public class WovoActivity extends Activity {

	SharedPreferences app_pref = null;
	private Button btnRst;
	private Button btnYes;
	private Button btnNo;
	private Button btnMain;
	private Button btnMem;
	private Button btnDft;
	private Button btnLrn;
	private Button btnNxt;
	private Button btnBck;
	private int Def0User1 = 0;
	private int flip = 1;

	private TextView tvWrd;
	private TextView tvDef;
	private TextView tvDebug;

	private int intLastLine = 0;
	private String line = null;


	public boolean InitWovo(boolean View)
	{
		//get the last time used line number 
		if(true  == View)
		{
			Def0User1 = 1;
			intLastLine = app_pref.getInt("user_Line", -1);
			Log.d("wovo", "user ......" + intLastLine);
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
			Log.d("wovo", "default ......" + intLastLine);
		}
		Log.d("wovo", "LastWord line number : " + intLastLine );

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
			//tvWrd.setText("");
			//tvDef.setText("Fail to Load database");
			//Log.d("wovo", "Default Fail...");
			return false;
		}
	}

	@Override 
	public void onSaveInstanceState(Bundle outState) 
	{
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
		Log.v("wovo", " +++ onPause +++");
		Lists.getInstance().readWriteLrnWordList(1);
		//Lists.getInstance().readWriteLrnWordList(0);
	}
	@Override
	public void onResume() {
		super.onResume();
		Log.v("wovo", " +++ onResume +++");
		Lists.getInstance().readWriteLrnWordList(0);
	}
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		Intent intent = getIntent();

		// from click on search results
		Lists.getInstance().ensureLoaded(getResources());
		//    String word = intent.getDataString();

		Log.d("wovo", intent.toString());
		if (intent.getExtras() != null) {
			Log.d("wovo", intent.getExtras().keySet().toString());
		}

		// Get the app's shared preferences
		app_pref = PreferenceManager.getDefaultSharedPreferences(this);

		btnRst = (Button) findViewById(R.id.button_reset);
		btnYes = (Button) findViewById(R.id.button_yes);
		btnNo = (Button) findViewById(R.id.button_no);
		btnMem = (Button) findViewById(R.id.button_mem);
		btnMain = (Button) findViewById(R.id.button_main);
		btnDft = (Button) findViewById(R.id.button_default);
		btnLrn = (Button) findViewById(R.id.button_learned);
		btnNxt = (Button) findViewById(R.id.button1);
		btnBck = (Button) findViewById(R.id.button2);
		tvWrd = (TextView) findViewById(R.id.word);
		tvDef = (TextView) findViewById(R.id.definition);
		tvDebug = (TextView) findViewById(R.id.num);


		tvDebug.setText("");


		// wiat till it load or checks the data base
		while(Lists.getInstance().isLoaded() == false);




		// for default screen
		btnDft.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				// Get the ViewFlipper from the layout
				if(Lists.getInstance().getTotalCount() == 0)
				{
					CharSequence text = " No words Found in Main List ";
					int duration = Toast.LENGTH_SHORT;
					Toast toast = Toast.makeText(view.getContext(), text, duration);
					toast.show();
					ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
					//flip = 1;
					// Set an animation from res/animation: I pick push left in
					//vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
					//vf.showPrevious();
					return;
					
				}
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				btnMem.setText(" I have memorized this word ! ");
				Lists.getInstance().setView(false);
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
				}

			}
		});



		btnLrn.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				// Get the ViewFlipper from the layout
				if(Lists.getInstance().getTotalCount() == 0)
				{
					CharSequence text = " No words found in revision list ";
					int duration = Toast.LENGTH_SHORT;
					Toast toast = Toast.makeText(view.getContext(), text, duration);
					toast.show();
					ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
					//flip = 1;
					// Set an animation from res/animation: I pick push left in
					//vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
					//vf.showPrevious();
					return;
					
				}
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				Lists.getInstance().setView(true);
				btnMem.setText(" I hadn't memorized this word ! ");
				if(true == InitWovo(true))
				{
					flip = 2;
					// Set an animation from res/animation: I pick push left in
					vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.push_left_in));
					vf.showNext();
				}else
				{
					Context context = getApplicationContext();
					CharSequence text = " No words found in revision list ";
					int duration = Toast.LENGTH_SHORT;

					Toast toast = Toast.makeText(context, text, duration);
					toast.show();
				}

			}
		});


		btnNxt.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {

				String line = Lists.getInstance().Next_list();

				if(line != null)
				{
					int curr_line;
					Lists.getInstance().splitText(line);
					tvWrd.setText(Lists.getInstance().getWord());
					tvDef.setText(Lists.getInstance().getDefine());
					curr_line = Lists.getInstance().getLineCount();
					int total_line = Lists.getInstance().getTotalCount();

					tvDebug.setText(String.valueOf(curr_line) + " / " + String.valueOf(total_line));

					// Save the last line
					SharedPreferences.Editor editor = app_pref.edit();
					if(Def0User1 == 0)
					{
						editor.putInt("def_Line", curr_line);
					}
					else
					{
						editor.putInt("user_Line", curr_line);
					}
					editor.commit();
				}else
				{
					Log.d("wovo", "Next Fail...");
				}
			}
		});

		btnBck.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {

				String line = Lists.getInstance().Back_list();
				if(line != null)
				{
					Lists.getInstance().splitText(line);
					tvWrd.setText(Lists.getInstance().getWord());

					tvDef.setText(Lists.getInstance().getDefine());
					int curr_line = Lists.getInstance().getLineCount();
					int total_line = Lists.getInstance().getTotalCount();

					tvDebug.setText(String.valueOf(curr_line) + " / " + String.valueOf(total_line));


					// Save the last line
					SharedPreferences.Editor editor = app_pref.edit();
					if(Def0User1 == 0)
					{
						editor.putInt("def_Line", curr_line);
					}
					else
					{
						editor.putInt("user_Line", curr_line);
					}
					editor.commit();
				}else
				{
					Log.d("wovo", "Back Fail...");
				}
			}
		});

		btnMain.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
				flip = 1;
				// Set an animation from res/animation: I pick push left in
				vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
				vf.showPrevious();
			}
		});

		btnMem.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				Context context = getApplicationContext();
				if(Def0User1 == 0)
				{
					
					CharSequence text = null;
					if(true == Lists.getInstance().setLearnWord())
					{
						text =  " \"" + Lists.getInstance().getWord() + "\" added to memorized word list ";
					}else
					{
						text = " \"" + Lists.getInstance().getWord() + "\" is already added ";
					}
					int duration = Toast.LENGTH_SHORT;
					Toast toast = Toast.makeText(context, text, duration);
					toast.show();
					//updating the current line / total lines
					//int curr_line = Lists.getInstance().getLineCount();
					//int total_line = Lists.getInstance().getTotalCount();
                   
					//tvDebug.setText(String.valueOf(curr_line) + " / " + String.valueOf(total_line));
					//this is for testing
					//Lists.getInstance().saveLrnWordList();
				}
				if(Def0User1 == 1)
				{
					CharSequence text = null;
					if(true == Lists.getInstance().removeLearnWord())
					{
						text =  " \"" + Lists.getInstance().getWord() + "\" removed from memorized word list ";
					}
					int duration = Toast.LENGTH_SHORT;
					Toast toast = Toast.makeText(context, text, duration);
					toast.show();
					
				}
				if(Lists.getInstance().getTotalCount() == 0)
				{
					CharSequence text = " No words in memorized List ";
					int duration = Toast.LENGTH_SHORT;
					Toast toast = Toast.makeText(context, text, duration);
					toast.show();
					ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
					flip = 1;
					// Set an animation from res/animation: I pick push left in
					vf.setAnimation(AnimationUtils.loadAnimation(view.getContext(), R.anim.fade));
					vf.showPrevious();
					return;
					
				}
				
				if(Lists.getInstance().getTotalCount() > 0)
				{
					// show the next avaiable word
					String line = Lists.getInstance().Next_list();

					if(line != null)
					{
						int curr_line;
						Lists.getInstance().splitText(line);
						tvWrd.setText(Lists.getInstance().getWord());
						tvDef.setText(Lists.getInstance().getDefine());
						curr_line = Lists.getInstance().getLineCount();
						int total_line = Lists.getInstance().getTotalCount();

						tvDebug.setText(String.valueOf(curr_line) + " / " + String.valueOf(total_line));

						// Save the last line
						SharedPreferences.Editor editor = app_pref.edit();
						if(Def0User1 == 0)
						{
							editor.putInt("def_Line", curr_line);
						}
						else
						{
							editor.putInt("user_Line", curr_line);
						}
						editor.commit();
					}else
					{
						Log.d("wovo", "Next Fail...");
					}
				}

				//Lists.getInstance().printIdx();
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