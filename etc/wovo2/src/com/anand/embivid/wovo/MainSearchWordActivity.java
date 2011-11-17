package com.anand.embivid.wovo;

import android.app.Activity;
import android.app.ActionBar;
import android.app.SearchManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
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
    private TextView mTextView;
    private ListView mListView;
	private Button btnNxt;
	private Button btnBck;
	private Button btnMem;
	private boolean mloaded = false;
	private int line_count = 0;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.e(TAG, "onCreate +++ ");
        
        setContentView(R.layout.search);

        mTextView = (TextView) findViewById(R.id.text);
        mListView = (ListView) findViewById(R.id.list);
    	//btnNxt = (Button) findViewById(R.id.button1);
		//btnBck = (Button) findViewById(R.id.button2);
		//btnMem = (Button) findViewById(R.id.button_mem);

		Bundle bundle = getIntent().getExtras();
		Boolean view = bundle.getBoolean("VIEW");
         
		Log.e(TAG, " view :" + view);
		
        handleIntent(getIntent(), mloaded);

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
    	//if(mloaded)
    	//{
    		if (Intent.ACTION_VIEW.equals(intent.getAction())) {
    			Log.e(TAG," --- ACTION_VIEW ---");
    			// handles a click on a search suggestion; launches activity to show word
    			Intent wordIntent = new Intent(this, ShowWordDefActivity.class);
    			wordIntent.setData(intent.getData());
    			 // this will tell that this is not from the search option
    			wordIntent.putExtra("SEARCH", true);
    			startActivity(wordIntent);
    			finish();
    	    } else if (Intent.ACTION_SEARCH.equals(intent.getAction())) {
    	    	// handles a search query
    	    	Log.e(TAG," --- ACTION_SEARCH ---");
    	    	String query = intent.getStringExtra(SearchManager.QUERY);
    	    	Log.e(TAG," query = "+query);
    	    	showResults(query);

    	    }else
    	    {
    	    	if(line_count == 0)
    	    	{
    	    		line_count = 1;
    	    	}
    	    	displayWordDef();
    	    	Intent wordIntent = new Intent(this, ShowWordDefActivity.class);
    			wordIntent.setData(intent.getData());
    			// this will tell that this is not from the search option
    	    	wordIntent.putExtra("SEARCH", false);
      	        startActivity(wordIntent);
    			finish();
      	    }
    //}
    //else
    //{
    	//Log.e(TAG, "First time loaded --");
    	//mloaded = true;
    	//Intent wordIntent = new Intent(this, ShowWordDefActivity.class);
		//wordIntent.setData(intent.getData());
		//startActivity(wordIntent);
		//finish();
    	
    //}
        Log.e(TAG, "handleIntent --");
    }

    /**
     * Searches the dictionary and displays results for the given query.
     * @param query The search query
     */
    private void displayWordDef()
    {
    	// Log.e(TAG, "displayWordDef +++");
         //Cursor cursor = managedQuery(WordDefinationProvider.CONTENT_URI, null,  String.valueOf(line_count),
         //        new String[] {"wovo"}, null);
    }
    
    private void showResults(String query) {
        Log.e(TAG, "showResults +++");
        Cursor cursor = managedQuery(WordDefinationProvider.CONTENT_URI, null, null,
                                new String[] {query}, null);

        if (cursor == null) {
            // There are no results
            mTextView.setText(getString(R.string.no_results, new Object[] {query}));
        } else {
            // Display the number of results
            int count = cursor.getCount();
            String countString = getResources().getQuantityString(R.plurals.search_results,
                                    count, new Object[] {count, query});
            mTextView.setText(countString);

            // Specify the columns we want to display in the result
            String[] from = new String[] { WordDatabase.KEY_WORD,
                                           WordDatabase.KEY_DEFINITION };

            // Specify the corresponding layout elements where we want the columns to go
            int[] to = new int[] { R.id.word,
                                   R.id.definition };

            // Create a simple cursor adapter for the definitions and apply them to the ListView
            SimpleCursorAdapter words = new SimpleCursorAdapter(this,
                                          R.layout.reesult, cursor, from, to);
            mListView.setAdapter(words);

            // Define the on-click listener for the list items
            mListView.setOnItemClickListener(new OnItemClickListener() {
                public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                    // Build the Intent used to open WordActivity with a specific word Uri
                    Intent wordIntent = new Intent(getApplicationContext(), ShowWordDefActivity.class);
                    Uri data = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
                                                    String.valueOf(id));
                    wordIntent.setData(data);
                    startActivity(wordIntent);
                }
            });
        }
    }
    

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
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
        switch (item.getItemId()) {
            case R.id.search:
                onSearchRequested();
                return true;
            default:
                return false;
        }
    }
}
