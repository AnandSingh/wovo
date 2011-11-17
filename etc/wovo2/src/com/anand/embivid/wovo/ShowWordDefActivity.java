package com.anand.embivid.wovo;

import android.app.Activity;
import android.app.ActionBar;
import android.app.SearchManager;
import android.content.ContentUris;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.SearchView;
import android.widget.TextView;

/**
 * Displays a word and its definition.
 */
public class ShowWordDefActivity extends Activity {
	boolean mLoaded = false;
	private WordDatabase mDictionary;
	private TextView word;
	private TextView definition;
	private int curr_line_count = 0;
	Cursor cursor = null;
	Button btnNxt;
	Button btnBck;
	Button btnMem;
	private static final int TOTAL_LINES =  175;//4759;//1024;
	
	public void dsipalyLine(int line_num, int type)
	{
    	Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
                String.valueOf(line_num));
        Log.e(TAG, "uri: " + uri);
		
    	cursor = managedQuery(uri, null, null, null, null);

		//Log.e(TAG, "managedQuery -->");
	    //cursor = managedQuery(WordDefinationProvider.CONTENT_URI, null,  String.valueOf(line_num),
	    //		new String[] {"wovo"}, null);
		cursor.moveToFirst();
		
		//
		/*if(type == 0)
		 {
	      
		 }else if(type == 1)
		 {
			 cursor.moveToNext();
		 }
		 else if(type == 2)
		 {
			 cursor.moveToPrevious();
		 }*/
		Log.e(TAG, "-----> "+cursor +  ": " + WordDefinationProvider.currentRowId );
		//CONTENT_URI;

 		int wIndex = cursor.getColumnIndexOrThrow(WordDatabase.KEY_WORD);
 		int dIndex = cursor.getColumnIndexOrThrow(WordDatabase.KEY_DEFINITION);
 		
 		Log.e(TAG, "wIndex: " + wIndex + ", dTndex: " + dIndex);

 		word.setText(cursor.getString(wIndex));
 		definition.setText(cursor.getString(dIndex));
	}
	public void updateLine()
	{
		Log.e(TAG,"getContentResolver().update() --->");
		getContentResolver().update(WordDefinationProvider.CONTENT_URI, null, null, null);
	}
	
    private static final String TAG = "ShowWordDefActivity";//"DictionaryDatabase";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.word);

        Log.e(TAG, "onCreate +++");
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
        
        word = (TextView) findViewById(R.id.word);
		definition = (TextView) findViewById(R.id.definition);
		btnNxt = (Button) findViewById(R.id.button1);
		btnBck = (Button) findViewById(R.id.button2);
		btnMem = (Button) findViewById(R.id.button_mem);
        
    	Bundle bundle = getIntent().getExtras();
		Boolean search1default0 = bundle.getBoolean("SEARCH");
         
		Log.e(TAG, " view :" + search1default0);

		
    	
		if(search1default0 == true)
		{

			Uri uri = getIntent().getData();

			Log.e(TAG, "uri: " + uri);
			
	    	cursor = managedQuery(uri, null, null, null, null);
	    	
	    	Log.e(TAG, "cursor: " + cursor);
			
        	if (cursor == null) {
        		Log.e(TAG, "cursor == null" + uri);
        		finish();
        	} else {

        		Log.e(TAG, "cursor != null" );
        		Log.e(TAG, "uri: " + uri);
        		Log.e(TAG, "cursor: " + cursor);
    		
        		cursor.moveToFirst();

        		int wIndex = cursor.getColumnIndexOrThrow(WordDatabase.KEY_WORD);
        		int dIndex = cursor.getColumnIndexOrThrow(WordDatabase.KEY_DEFINITION);

        		
        		Log.e(TAG, "wIndex: " + wIndex + ", dTndex: " + dIndex + ", " + WordDefinationProvider.currentRowId  + ", " + cursor.getCount());
        		curr_line_count = WordDefinationProvider.currentRowId;
       		
        		word.setText(cursor.getString(wIndex));
        		definition.setText(cursor.getString(dIndex));
        	}
        }else
        {
        	if(curr_line_count == 0)
        	{
        		curr_line_count = 1;
        	}
        	dsipalyLine(curr_line_count,0);
       }
		
		 // for next word
		btnMem.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				updateLine();
			}
		});
	
	    // for next word
		btnNxt.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				// Get the ViewFlipper from the layout
				curr_line_count++;
				if(curr_line_count > TOTAL_LINES)
				{
					curr_line_count = 1;
				}
				dsipalyLine(curr_line_count,1);
				

			}
		});
		// for back word
		btnBck.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				// Get the ViewFlipper from the layout
				curr_line_count--;
				if(curr_line_count <= 0)
				{
					curr_line_count = TOTAL_LINES;
				}
				dsipalyLine(curr_line_count,2);
				

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
