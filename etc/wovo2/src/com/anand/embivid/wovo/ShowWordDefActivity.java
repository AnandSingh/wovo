package com.anand.embivid.wovo;

import android.app.Activity;
import android.app.ActionBar;
import android.app.SearchManager;
import android.content.ContentUris;
import android.content.ContentValues;
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
import android.widget.Button;
import android.widget.SearchView;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Displays a word and its definition.
 */
public class ShowWordDefActivity extends Activity {
	private static final String IDENTITY_1 = "1";
	private static final String IDENTITY_2 = "2";
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
	//private String currentWord = null;
	private boolean memory1def0 = false;
	private boolean search1default0 = false;
	//private static final int TOTAL_LINES =  175;
	
	private static final String TAG = "ShowWordDefActivity";
	
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
		
    	int wIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_WORD);
    	int dIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_DEFINITION);
    	//int iIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_IDENTITY);

    	int idx = curr_cursor.getPosition() + 1;
    	num.setText(""+idx+ " / " + curr_cursor.getCount());
    	word.setText(curr_cursor.getString(wIndex));
    	definition.setText(curr_cursor.getString(dIndex));
    	
    	Log.e("TAG","  >>> " + curr_cursor.getPosition());
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

    
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.word);

        Log.e(TAG, "onCreate +++");
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        
     // Get the app's shared preferences
     	app_pref = PreferenceManager.getDefaultSharedPreferences(this);
        
     	num = (TextView) findViewById(R.id.num);
        word = (TextView) findViewById(R.id.word);
		definition = (TextView) findViewById(R.id.definition);
		btnNxt = (Button) findViewById(R.id.button1);
		btnBck = (Button) findViewById(R.id.button2);
		btnMem = (Button) findViewById(R.id.button_mem);
        
		
    	Bundle bundle = getIntent().getExtras();
    	

    	memory1def0 = bundle.getBoolean("VIEW");
    	search1default0 = bundle.getBoolean("SEARCH");
     
    	if(search1default0 == true)
		{
			Uri uri = getIntent().getData();

			Log.e(TAG, "uri: " + uri);
			if(memory1def0 == false)
			{
				String[] selectionArgs = new String[] {uri.getLastPathSegment(), "1"};
				cursor = managedQuery(uri, null, null, selectionArgs, null);
			}else
			{
				String[] selectionArgs = new String[] {uri.getLastPathSegment(), "2"};
		    	cursor = managedQuery(uri, null, null, selectionArgs, null);
			}
	    	
        	if (cursor == null) {
           		finish();
        	} else {

        		Log.e(TAG, "cursor != null" );
        
        		cursor.moveToFirst();

        		int wIndex = cursor.getColumnIndexOrThrow(WordDatabase.KEY_WORD);
        		int dIndex = cursor.getColumnIndexOrThrow(WordDatabase.KEY_DEFINITION);
                 
        		
        		// curr_line_count = Integer.valueOf(cursor.getString(rIndex));
        	    // int idx = Integer.valueOf(curr_cursor.getPosition()) + 1;
        		
        		// num.setText(""+ idx + " / " + curr_cursor.getCount());
        		word.setText(cursor.getString(wIndex));
        		definition.setText(cursor.getString(dIndex));
    
        		
        		SharedPreferences.Editor editor = app_pref.edit();
        		if(memory1def0 == false)
    			{
    				editor.putInt("def_Line", curr_line_count);
    			}
    			if(memory1def0 == true)
    			{
    				editor.putInt("user_Line", curr_line_count);
    			}
    			editor.commit();
        	}
        }else
        {
    	
        	if(memory1def0 == false)
        	{
        		def_total_cont = app_pref.getInt("DEF_LINE_CNT", 0);
        		curr_line_count = app_pref.getInt("def_Line", 1);
        	}
        	if(memory1def0 == true)
        	{
        		usr_total_cont = app_pref.getInt("USR_LINE_CNT", 0);
        		curr_line_count = app_pref.getInt("user_Line", 1);
        	}

        	Log.e(TAG, "+++++++++++++++++++++++++");
           	Log.e(TAG, "def: " + def_total_cont);
           	Log.e(TAG, "usr: " + usr_total_cont);
           	Log.e(TAG, "+++++++++++++++++++++++++");
        	// get the cursor for whole memorized word or default word 
    		Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
                    String.valueOf(1));
            //Log.e(TAG, "uri: " + uri);
    		
        	//String[] selectionArgs = new String[] {uri.getLastPathSegment(), "1"};
        	//Cursor def_cursor = managedQuery(uri, null, null, selectionArgs, null);
		
        	//String[] selectionArgs1 = new String[] {uri.getLastPathSegment(), "2"};
        	//Cursor usr_cursor = managedQuery(uri, null, null, selectionArgs1, null);
		    	
    		
        	
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
        
            Log.e(TAG, "+++++++++++++++++++++++++");
           	Log.e(TAG, "def: " + def_total_cont);
           	Log.e(TAG, "usr: " + usr_total_cont);
           	Log.e(TAG, "+++++++++++++++++++++++++");
            
            if(curr_cursor != null)
            {
                Log.e("TAG"," Total count >>> " + curr_cursor.getCount());
                Log.e("TAG"," curr count >>> " + curr_line_count);
                
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
				Log.e("TAG"," Total count >>> " + curr_cursor.getCount());
					
				dsipalyLine(curr_line_count,1);
     		}
		});
		// for back word
		btnBck.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				// Get the ViewFlipper from the layout
				Log.e("TAG"," Total count >>> " + curr_cursor.getCount());
				
				dsipalyLine(curr_line_count,0);
				}
		});
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	Log.e(TAG,"onCreateOptionsMenu +++");
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_item, menu);

        SearchManager searchManager = (SearchManager) getSystemService(Context.SEARCH_SERVICE);
        SearchView searchView = (SearchView) menu.findItem(R.id.search).getActionView();
        searchView.setSearchableInfo(searchManager.getSearchableInfo(getComponentName()));
        searchView.setIconifiedByDefault(false);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	Log.e(TAG,"onOptionsItemSelected +++");
        switch (item.getItemId()) {
            case R.id.search:
            	Log.e(TAG,"R.id.search:");
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
