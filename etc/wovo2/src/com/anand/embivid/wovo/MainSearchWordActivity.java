package com.anand.embivid.wovo;

import java.io.File;

import android.app.Activity;
import android.app.ActionBar;
import android.app.SearchManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.animation.AnimationUtils;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.SearchView;
import android.widget.SimpleCursorAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ViewFlipper;
import android.widget.AdapterView.OnItemClickListener;

/**
 * The main activity for the dictionary.
 * Displays search results triggered by the search dialog and handles
 * actions from search suggestions.
 */
public class MainSearchWordActivity extends Activity {
	private static final String TAG = "MainSearchWordActivity";
	private static final String IDENTITY_1 = "1";
	private static final String IDENTITY_2 = "2";
    private TextView mTextView;
    private ListView mListView;
	private boolean mloaded = false;
	private Boolean view = false;
	
	
	SharedPreferences app_pref = null;

	private TextView num;
	private TextView word;
	private TextView definition;
	private int curr_line_count = 0;
	private int def_total_cont = 0;
	private int usr_total_cont = 0;
	Cursor cursor = null;
	private Cursor curr_cursor = null;
	private Button btnNxt;
	private Button btnBck;
	private Button btnMem;
	private Button btnSearch;
	//private String currentWord = null;
	private boolean memory1def0 = false;
	private boolean search1default0 = false;
	//private static final int TOTAL_LINES =  175;
	
	private File lrn_file = null;
	private int[] lrn_lines = new int[175];//[4759];
	private int lrn_cnt = 0;
	private int curr_ridx = 0;

	/*
	@Override
	public void onBackPressed() {
	//	Log.e(TAG, "onBackPressed ++");
	
	 super.onBackPressed();
	}*/
	public int dsipalyLine(int line_num, int nxt1bck0)
	{
		if(curr_cursor == null)
		{
			return -1;
		}
		if(nxt1bck0 == 1)
		{
			if(curr_cursor.isLast())
			{
				curr_cursor.moveToFirst();
			}else
			{
				curr_cursor.moveToNext();
			}
		}
		if(nxt1bck0 == 0)
		{
			if(curr_cursor.isFirst())
			{
				curr_cursor.moveToLast();
			}else
			{
				curr_cursor.moveToPrevious();
			}
		}
		
		int ridx = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_ID);
    	int wIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_WORD);
    	int dIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_DEFINITION);
    	//int iIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_IDENTITY);

    	int idx = curr_cursor.getPosition() + 1;
    	num.setText(""+idx+ " / " + curr_cursor.getCount());
    	word.setText(curr_cursor.getString(wIndex));
    	definition.setText(curr_cursor.getString(dIndex));
    	curr_ridx = Integer.valueOf(curr_cursor.getString(ridx));
    	
    	Log.e("TAG","  >>> " + curr_cursor.getPosition());
    	Log.e("TAG","  >>> " + curr_ridx);
    	Log.e("TAG","  >>> " + word.getText());
    	Log.e("TAG","  >>> " + definition.getText());
    	
    	SharedPreferences.Editor editor = app_pref.edit();
		if(memory1def0 == false)
		{
			editor.putInt("def_Line", curr_cursor.getPosition());
		}
		if(memory1def0 == true)
		{
			editor.putInt("user_Line", curr_cursor.getPosition());
		}
		editor.commit();
		return 0;
	}

	
	
	public void displayWord(int rowNum)
	{
	  //Log.e(TAG, "" + 2);
	if(memory1def0 == false)
	{
		def_total_cont = app_pref.getInt("DEF_LINE_CNT", 0);
		//curr_line_count = app_pref.getInt("def_Line", 1);
	}
	if(memory1def0 == true)
	{
		usr_total_cont = app_pref.getInt("USR_LINE_CNT", 0);
		//curr_line_count = app_pref.getInt("user_Line", 1);
	}
	curr_line_count = rowNum;
	
	// get the cursor for whole memorized word or default word 
	Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
            String.valueOf(1));
    	
	Log.e(TAG, "+++++++++++++++++++++++++");
	Log.e(TAG, "memory1def0: " + memory1def0);
   	Log.e(TAG, "def: " + def_total_cont);
   	Log.e(TAG, "usr: " + usr_total_cont);
	Log.e(TAG, "curr_line_count:" + curr_line_count);
   	Log.e(TAG, "uri: " + uri);
   	Log.e(TAG, "+++++++++++++++++++++++++");
   	
    if(memory1def0 == false)
	{
    	String[] selectionArgs = new String[] {uri.getLastPathSegment(), "1"};
    	curr_cursor = managedQuery(uri, null, null, selectionArgs, null);
    	String[] selectionArgs1 = new String[] {uri.getLastPathSegment(), "2"};
    	Cursor usr_cursor = managedQuery(uri, null, null, selectionArgs1, null);
    	if(curr_cursor != null)
    	{
    		def_total_cont = curr_cursor.getCount();
    	}else
    	{
    		def_total_cont = 0;
    	}
    	if(usr_cursor != null)
    	{
    		usr_total_cont = usr_cursor.getCount();
    		usr_cursor.close();
    	}else
    	{
    		usr_total_cont = 0;
    	}
    	
    	
	}else
	{
		String[] selectionArgs = new String[] {uri.getLastPathSegment(), "2"};
    	curr_cursor = managedQuery(uri, null, null, selectionArgs, null);
    	String[] selectionArgs1 = new String[] {uri.getLastPathSegment(), "1"};
    	Cursor def_cursor = managedQuery(uri, null, null, selectionArgs1, null);
     	if(curr_cursor != null)
    	{
     		usr_total_cont = curr_cursor.getCount();
    	}else
    	{
    		usr_total_cont = 0;
    	}
    	if(def_cursor != null)
    	{
    		def_total_cont = def_cursor.getCount();
	    	def_cursor.close();
    	}else
    	{
    		def_total_cont = 0;
    	}
    	
    
    }

   if(curr_cursor != null)
    {
        Log.e(TAG," Total count >>> " + curr_cursor.getCount());
        Log.e(TAG," curr count >>> " + curr_line_count);
        
        Log.e(TAG, "+++++++++++++++++++++++++");
    	Log.e(TAG, "def: " + def_total_cont);
    	Log.e(TAG, "usr: " + usr_total_cont);
    	Log.e(TAG, "+++++++++++++++++++++++++");
    	
        
        SharedPreferences.Editor editor = app_pref.edit();
		editor.putInt("DEF_LINE_CNT", def_total_cont);
		editor.putInt("USR_LINE_CNT", usr_total_cont);
		editor.commit();
    	
        curr_cursor.moveToFirst();

        if(curr_cursor.moveToPosition(curr_line_count))
    	{
       		int wIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_WORD);
    		int dIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_DEFINITION);
    		//int iIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_IDENTITY);

    		int idx = curr_cursor.getPosition() + 1;
    		num.setText(""+idx+ " / " + curr_cursor.getCount());
    		word.setText(curr_cursor.getString(wIndex));
    		definition.setText(curr_cursor.getString(dIndex));
    		Log.e("TAG","  >>> " + word.getText());
    		Log.e("TAG","  >>> " + definition.getText());
    		Log.e("TAG","  >>> " + curr_cursor.getPosition());
       	}
    }

}

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.e(TAG, "onCreate +++ ");
        
        setContentView(R.layout.search);

        mTextView = (TextView) findViewById(R.id.text);
        mListView = (ListView) findViewById(R.id.list);
        
     	app_pref = PreferenceManager.getDefaultSharedPreferences(this);
        btnSearch = (Button) findViewById(R.id.button_search);
        num = (TextView) findViewById(R.id.num);
        word = (TextView) findViewById(R.id.word);
		definition = (TextView) findViewById(R.id.definition);
		btnNxt = (Button) findViewById(R.id.button1);
		btnBck = (Button) findViewById(R.id.button2);
		btnMem = (Button) findViewById(R.id.button_mem);

		Bundle bundle = getIntent().getExtras();
		view = bundle.getBoolean("VIEW");
		memory1def0 = view;
         
		Log.e(TAG, " view :" + view);
		
        handleIntent(getIntent(), mloaded);
        
        // Attach actions to buttons
        btnSearch.setOnClickListener(new OnClickListener() {
           	public void onClick(View v) {
           	    Log.e(TAG, "btnSearch +++ ");
           		onSearchRequested();
                }
            });
		
		 // for next word
		btnMem.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				
				CharSequence text = null;
				CharSequence text1 = null;
				//boolean finish_line = false;
				CharSequence currentWord = word.getText();
				int line = 0;
				
				Log.e(TAG,"getContentResolver().update() --->");
			   	Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
		                String.valueOf(curr_cursor.getPosition()+1));
			   	
			   	if(memory1def0 == false)
			   	{
			   	    String[] selectionArgs = new String[] {""+currentWord, IDENTITY_2};
			   		getContentResolver().update(WordDefinationProvider.CONTENT_URI, null, null, selectionArgs);
			   		text =  " \"" + currentWord + "\" added to memorized word list ";
			   		lrn_lines[curr_ridx] = 1;
			   		
			   		line = curr_cursor.getPosition();// + 1;
			   		curr_cursor.close();
			   	 	
			   		uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
			                String.valueOf(1));
			   		String[] selectionArgs1 = new String[] {uri.getLastPathSegment(), "1"};
	            	
			   		curr_cursor = managedQuery(uri, null, null, selectionArgs1, null);
			   		if(curr_cursor != null)
			   		{
			   			def_total_cont = curr_cursor.getCount();
			   		}else
			   		{
			   			def_total_cont = 0;
			   		}
	            	usr_total_cont++;
		 
			   	}
			   						   	
			   	if(memory1def0 == true)
			   	{
			   		String[] selectionArgs = new String[] {""+currentWord, IDENTITY_1};
			   		getContentResolver().update(WordDefinationProvider.CONTENT_URI, null, null, selectionArgs);
	    	   		text =  " \"" + currentWord + "\" removed from memorized word list ";
	    	   		lrn_lines[curr_ridx] = 0;
			   		line = curr_cursor.getPosition();// - 1;
			   		curr_cursor.close();
			   	 	
			   		uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
			                String.valueOf(1));
			   		String[] selectionArgs1 = new String[] {uri.getLastPathSegment(), "2"};
	    	    	curr_cursor = managedQuery(uri, null, null, selectionArgs1, null);
	    	    	if(curr_cursor != null)
	    	    	{
	    	    		usr_total_cont = curr_cursor.getCount();
	    	    	}else
	    	    	{
	    	    		usr_total_cont = 0;
	    	    	}
	    	    	def_total_cont++;
			
			   	}
				Toast toast = Toast.makeText(view.getContext(), text, Toast.LENGTH_SHORT);
				toast.show();
				
				Log.e(TAG, "+++++++++++++++++++++++++");
				Log.e(TAG, "def: " + def_total_cont);
				Log.e(TAG, "usr: " + usr_total_cont);
				Log.e(TAG, "+++++++++++++++++++++++++");
				SharedPreferences.Editor editor = app_pref.edit();
				editor.putInt("DEF_LINE_CNT", def_total_cont);
				editor.putInt("USR_LINE_CNT", usr_total_cont);
				editor.commit();
				
				if(curr_cursor == null)
				{
					Toast toast1 = Toast.makeText(view.getContext(), text1, Toast.LENGTH_SHORT);
					toast1.show();
					Intent intent = new Intent(view.getContext(), WovoActivity.class);
					intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
					startActivity(intent);
					finish();
					
				}else
				{
					//show the next line
					//curr_line_count++;
					Log.e(TAG, "line >>>> " + line);
					Log.e(TAG, "count >>>> " + curr_cursor.getCount());
					
					curr_cursor.moveToPosition(line-1);
					dsipalyLine(curr_line_count,1);
				}
				
    		}
		});
	
	    // for next word
		btnNxt.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				Log.e(TAG, " btnNxt +++" );
				Log.e(TAG," Total count >>> " + curr_cursor.getCount());
					
				dsipalyLine(curr_line_count,1);
     		}
		});
		// for back word
		btnBck.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				// Get the ViewFlipper from the layout
				Log.e(TAG, " btnBck +++" );
				Log.e(TAG," Total count >>> " + curr_cursor.getCount());
				
				dsipalyLine(curr_line_count,0);
				}
		});

    }

    @Override
    protected void onNewIntent(Intent intent) {
        // Because this activity has set launchMode="singleTop", the system calls this method
        // to deliver the intent if this actvity is currently the foreground activity when
        // invoked again (when the user executes a search from this activity, we don't create
        // a new instance of this activity, so the system delivers the search intent here)
    	Log.e(TAG, " +++ onNewIntent +++");
        handleIntent(intent, mloaded);
    }

    private void handleIntent(Intent intent, boolean mloaded) {
    	Log.e(TAG, "handleIntent +++ ");
    	Log.e(TAG, "intent.getAction() : " + intent.getAction());
    	ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
    	if (Intent.ACTION_VIEW.equals(intent.getAction())) {
    		
    			Log.e(TAG," --- ACTION_VIEW ---");
    			// handles a click on a search suggestion; launches activity to show word
    			//vf.setAnimation(AnimationUtils.loadAnimation(this.getApplicationContext(), R.anim.push_left_in));
    			//vf.showNext();
    			search1default0 = true;
 
    			//Uri uri = getIntent().getData();
    			//intent.getData();
       			//Log.e(TAG, "uri: " + intent.getData());
    			//String[] srowNum = new String[] {uri.getLastPathSegment()};
    			//Log.e(TAG,srowNum[0] + " " + Integer.valueOf(srowNum[0]));
    			
    			//rowNum =  Integer.valueOf(srowNum[0]) - 1;
    			
    			
    			//Intent wordIntent = new Intent(this, ShowWordDefActivity.class);
    			//wordIntent.setData(intent.getData());
    			//wordIntent.putExtra("VIEW", view);
    			//wordIntent.putExtra("SEARCH", true );
    			//startActivity(wordIntent);
    			//finish();
    	    } else if (Intent.ACTION_SEARCH.equals(intent.getAction())) {
    	    	// handles a search query
    	    	vf.showPrevious();
    	    	Log.e(TAG," --- ACTION_SEARCH ---");
    	    	String query = intent.getStringExtra(SearchManager.QUERY);
    	    	Log.e(TAG," query = "+query);
    	    	showResults(query);
    	    	//vf.setAnimation(AnimationUtils.loadAnimation(this.getApplicationContext(), R.anim.push_left_in));
    	    }else
    	    {
    	    	//Intent wordIntent = new Intent(this, ShowWordDefActivity.class);
    			//wordIntent.setData(intent.getData());
    			// this will tell that this is not from the search option
    			//wordIntent.putExtra("VIEW", view);
    			//wordIntent.putExtra("SEARCH", false );
      	        //startActivity(wordIntent);
    			//finish();
    	    	vf.setAnimation(AnimationUtils.loadAnimation(this.getApplicationContext(), R.anim.push_left_in));
    			vf.showNext();
    			search1default0 = false;
      	    }
    	
    	

    	Bundle bundle = getIntent().getExtras();
    	

    	memory1def0 = bundle.getBoolean("VIEW");
    	//search1default0 = bundle.getBoolean("SEARCH");
        int rowNum = 0;
  
    	if(search1default0 == true)
		{
			Uri uri = intent.getData();
		  	Log.e(TAG, "" + 1);
			Log.e(TAG, "uri: " + uri);
			String[] srowNum = new String[] {uri.getLastPathSegment()};
			Log.e(TAG,srowNum[0] + " " + Integer.valueOf(srowNum[0]));
			
			rowNum =  Integer.valueOf(srowNum[0]) - 1;
			
		}else
        {
        	if(memory1def0 == false)
        	{
        		//def_total_cont = app_pref.getInt("DEF_LINE_CNT", 0);
        		rowNum = app_pref.getInt("def_Line", 0);
        	}
        	if(memory1def0 == true)
        	{
        		//usr_total_cont = app_pref.getInt("USR_LINE_CNT", 0);
        		rowNum = app_pref.getInt("user_Line", 0);
        	}
        	
        }
    	displayWord(rowNum);
        Log.e(TAG, "handleIntent --");
    }

    /**
     * Searches the dictionary and displays results for the given query.
     * @param query The search query
     */
     private void showResults(String query) {
        Log.e(TAG, "+++++++++++++++++++showResults ++++++++++++++++++++");
        Cursor cursor = null;
        if(view == false)
        {
             cursor = managedQuery(WordDefinationProvider.CONTENT_URI, null, null,
                                new String[] {query, IDENTITY_1}, null);
        }
        if(view == true)
        {
        	cursor = managedQuery(WordDefinationProvider.CONTENT_URI, null, null,
                    new String[] {query, IDENTITY_2}, null);
	
        }

        if (cursor == null) {
            // There are no results
            mTextView.setText(getString(R.string.no_results, new Object[] {query}));
        } else {
        	//ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
        	//vf.showNext();
            // Display the number of results
            int count = cursor.getCount();
            String countString = getResources().getQuantityString(R.plurals.search_results,
                                    count, new Object[] {count, query});
            mTextView.setText(countString);

            // Specify the columns we want to display in the result
            String[] from = new String[] { WordDatabase.KEY_WORD,
                                           WordDatabase.KEY_DEFINITION };

            // Specify the corresponding layout elements where we want the columns to go
            int[] to = new int[] { R.id.word_res,
                                   R.id.definition_res };

            // Create a simple cursor adapter for the definitions and apply them to the ListView
            SimpleCursorAdapter words = new SimpleCursorAdapter(this,
                                          R.layout.reesult, cursor, from, to);
            
            mListView.setAdapter(words);

            // Define the on-click listener for the list items
            mListView.setOnItemClickListener(new OnItemClickListener() {
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    // Build the Intent used to open WordActivity with a specific word Uri
                    //Intent wordIntent = new Intent(getApplicationContext(), ShowWordDefActivity.class);
                    Uri data = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
                                                    String.valueOf(id));
                    Log.e(TAG, "" + data);
                   // setContentView(R.layout.search);
                   // ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
                 //	vf.showNext();
                    //wordIntent.setData(data);
                    //startActivity(wordIntent);
                }
            });
        }
    }
    
/*
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        
    	Log.e(TAG, "onCreateOptionsMenu ++");
    	
    	MenuInflater inflater = getMenuInflater();
       
        inflater.inflate(R.menu.menu_item, menu);

        SearchManager searchManager = (SearchManager) getSystemService(Context.SEARCH_SERVICE);
        SearchView searchView = (SearchView) menu.findItem(R.id.search).getActionView();
        searchView.setSearchableInfo(searchManager.getSearchableInfo(getComponentName()));
        searchView.setIconifiedByDefault(false);

        return true;
    }
*/
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	Log.e(TAG, "" + item.getItemId());
        switch (item.getItemId()) {
            case R.id.search:
                onSearchRequested();
                return true;
            case android.R.id.home:
             	Log.e(TAG,"R.id.home:");
                Intent intent = new Intent(this, WovoActivity.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
                startActivity(intent);
                finish();
                return true;
            default:
                return false;
        }
    }
 }
