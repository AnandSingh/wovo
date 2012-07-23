package com.anand.embvid.wovo;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;

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
//import android.util.Log;
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
	private static final int MAX_WORDS = 4759;
	private static final int MAX_ROW = 4759;//25;
	private TextView mTextView;
	private ListView mListView;
	private boolean mloaded = false;
	private Boolean view = false;


	SharedPreferences app_pref = null;

	private TextView num;
	private TextView word;
	private TextView definition;
	private int curr_line_count = 0;
	
	//private int def_total_cont = 0;
	//private int usr_total_cont = 0;
	
	Cursor cursor = null;
	private Cursor curr_cursor = null;
	private Button btnNxt;
	private Button btnBck;
	private Button btnMem;
	private Button btnSearch;
	//private String currentWord = null;
	//private boolean memory1def0 = false;
	private boolean search1default0 = false;

	private File lrn_file = null;
	private int[] lrn_lines = new int[MAX_WORDS + 1];//[4759];

	//private int curr_ridx = 0;

	private Boolean search = false;
	private Boolean search_click = false;
	
	private WovoDbHeader wHead = new WovoDbHeader();

	private static final String LRN_FILE = "/data/data/com.anand.embvid.wovo/lrn.dat";

	@Override
	public void onBackPressed() {
		// Log.e(TAG, "onBackPressed ++");
		if(search == true)
		{
			search = false;
		}
		super.onBackPressed();
	}

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
		wHead.cur_row = Integer.valueOf(curr_cursor.getString(ridx));

		//Log.e("TAG","  >>> " + curr_cursor.getPosition());
		//Log.e("TAG","  >>> " + curr_ridx);
		//Log.e("TAG","  >>> " + word.getText());
		//Log.e("TAG","  >>> " + definition.getText());

		SharedPreferences.Editor editor = app_pref.edit();
		if(wHead.view == false)
		{
			editor.putInt("def_Line", wHead.cur_row);
		}
		if(wHead.view == true)
		{
			editor.putInt("user_Line", wHead.cur_row);
		}
		editor.commit();
		return 0;
	}

	
	public void displayWord(boolean view, int rowNum)
	{
		wHead.totalWord_1 = app_pref.getInt("DEF_LINE_CNT", 0);
		wHead.totalWord_2 = app_pref.getInt("USR_LINE_CNT", 0);
		wHead.cur_row = rowNum;
		
		Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
				String.valueOf(wHead.cur_row));
		int totalWord;
		
		//loop = def_total_cont / 25;
		//rem = def_total_cont % 25;
		//next_cnt = 0;
		
		int curr_loop = (wHead.cur_row / MAX_ROW) + 1;
		wHead.max_count = MAX_ROW;
		
		
		if(view == true)
		{   // for word-2
			int loop = wHead.totalWord_2 / MAX_ROW;
			if(curr_loop == loop)
			{
				wHead.max_count = wHead.totalWord_2 - wHead.cur_row;
			}
			int total = wHead.cur_row + wHead.max_count;
			String[] selectionArgs = new String[] {uri.getLastPathSegment(), String.valueOf(total),"2"};
			curr_cursor = managedQuery(uri, null, null, selectionArgs, null);
			totalWord = wHead.totalWord_2;
		}else
		{  // for word-1
			int loop = wHead.totalWord_1 / MAX_ROW;
			if(curr_loop == loop)
			{
				wHead.max_count = wHead.totalWord_1 - wHead.cur_row;
			}
			int total = wHead.cur_row + wHead.max_count;
			String[] selectionArgs = new String[] {uri.getLastPathSegment(), String.valueOf(total),"1"};
			curr_cursor = managedQuery(uri, null, null, selectionArgs, null);
			totalWord = wHead.totalWord_1;
		}
		wHead.count = 1;
		if(curr_cursor != null)
		{
			//SharedPreferences.Editor editor = app_pref.edit();
			//editor.putInt("DEF_LINE_CNT", def_total_cont);
			//editor.putInt("USR_LINE_CNT", usr_total_cont);
			//editor.commit();

			curr_cursor.moveToFirst();
			
			Log.e(">>>>", "curr_cursor.count " + curr_cursor.getCount());
			//int adjustIdx = ;

			//if(curr_cursor.moveToPosition(curr_line_count))
			//{
				int ridx = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_ID);
				int wIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_WORD);
				int dIndex = curr_cursor.getColumnIndexOrThrow(WordDatabase.KEY_DEFINITION);
				wHead.cur_row = Integer.valueOf(curr_cursor.getString(ridx));
				
				num.setText(""+ wHead.cur_row + " / " + totalWord);
				
				word.setText(curr_cursor.getString(wIndex));
				definition.setText(curr_cursor.getString(dIndex));
				//Log.e("TAG","  >>> " + curr_ridx + " " + wHead.cur_row);

		}

	}

	public synchronized void ensureLoaded() {

		//	if (mLoaded) return;

		new Thread(new Runnable() {
			public void run() {
				try {
					loadWords();
				} catch (IOException e) {
					throw new RuntimeException(e);
				}
			}
		}).start();
	}

	private synchronized void loadWords() throws IOException {
		//if (mLoaded) return;
		//Log.d("wovo", "loading words");
		try
		{
			if(lrn_file == null)
			{
				lrn_file = new File(LRN_FILE);
			}
			readWriteLrnWordList(0);
			//Log.d("wovo", "\"lrn.dat\" File Loaded, Size :" + lrn_file.length());
		}
		catch (Exception e)
		{
			//Log.d("wovo init", e.toString());
		}

		return;
	}

	public synchronized void readWriteLrnWordList(int read0write1)
	{
		try {
			if(lrn_file.exists() == false)
			{
				lrn_file = new File(LRN_FILE);
				//Log.e("wovo", "File Found .....");
			}
			Process process = Runtime.getRuntime().exec("sh");
			DataOutputStream os = new DataOutputStream(process.getOutputStream());
			os.writeBytes("chmod 755 "+LRN_FILE+"\n");

			os.writeBytes("exit\n");
			os.flush(); 

			if(read0write1 == 1)
			{
				//Log.d("WOVO", "=== Writing File === " + lrn_file);
				// Wrap the FileOutputStream with a DataOutputStream
				DataOutputStream data_out = new DataOutputStream (new FileOutputStream (lrn_file));
				// Write the data to the file in an integer/double pair

				for(int i = 0; i < (MAX_WORDS+1); i++)
				{
					data_out.writeInt (lrn_lines[i]);
					//		Log.d("wovo", "w* " + i + ". " + lrn_lines[i]);
				}
				data_out.close();
			}


			if(read0write1 == 0)
			{
				// Log.d("WOVO", "===Reading File=== " + lrn_file);
				//for(int i =0 ; i < (MAX_WORDS+1); i++)
				//{
				//	lrn_lines[i] = 0;
				//}

				// FileInputStream file_input = new FileInputStream (lrn_file);
				DataInputStream data_in    = new DataInputStream (new FileInputStream (lrn_file) );

				int lrn_cnt = 0;
				while (true) {
					try {
						lrn_lines[lrn_cnt] = data_in.readInt ();
						//	Log.d("wovo", "r*" + lrn_cnt + ". " + lrn_lines[lrn_cnt]);
						lrn_cnt++;
					}
					catch (EOFException eof) {
						//System.out.println ("End of File");

						break;
					}
				}
				data_in.close ();
			}

		} catch (Exception e) {
			//Log.e("wovo", "Exception");
			//Log.e("wovo", e.toString());
			return;
		}
	}

	@Override
	public boolean onSearchRequested()
	{
		search = true;
		return super.onSearchRequested();
	}


	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		//  Log.e(TAG, "onCreate +++ ");

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
		//view = bundle.getBoolean("VIEW");
		wHead.view = bundle.getBoolean("VIEW");
		//memory1def0 = view;

		//	Log.e(TAG, " view :" + view);

		ensureLoaded();
		//wHead.totalWord_1 = def_total_cont;
		//wHead.totalWord_2 = usr_total_cont;
		
		//Log.e("++++++","usr_total_cont : "+ usr_total_cont );
		//Log.e("++++++","def_total_cont : "+ def_total_cont );
		handleIntent(getIntent(), mloaded);

	    // Attach actions to buttons
		btnSearch.setOnClickListener(new OnClickListener() {
			public void onClick(View v) {
				//    Log.e(TAG, "btnSearch +++ ");
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

				//		Log.e(TAG,"getContentResolver().update() --->");
				//Uri uri = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
				//		String.valueOf(wHead.cur_row));
				String[] selectionArgs = new String[] {""+currentWord, String.valueOf(wHead.cur_row)};
				if(wHead.view == false)
				{
					getContentResolver().update(WordDefinationProvider.CONTENT_URI, null, IDENTITY_2, selectionArgs);
					text =  " \"" + currentWord + "\" added to memorized word list ";

					lrn_lines[wHead.cur_row - 1] = 1;

					//line = curr_cursor.getPosition();// + 1;
					wHead.totalWord_1--;
					wHead.totalWord_2++;
/*
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
*/
				}

				if(wHead.view == true)
				{
					//String[] selectionArgs = new String[] {""+currentWord, ""+wHead.cur_row/*, IDENTITY_1*/};
					getContentResolver().update(WordDefinationProvider.CONTENT_URI, null, IDENTITY_1, selectionArgs);
					text =  " \"" + currentWord + "\" removed from memorized word list ";

					lrn_lines[wHead.cur_row-1] = 0;
					
					wHead.totalWord_1++;
					wHead.totalWord_2--;

					//line = curr_cursor.getPosition();// - 1;
					curr_cursor.close();
/*
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
*/
				}
				Toast toast = Toast.makeText(view.getContext(), text, Toast.LENGTH_SHORT);
				toast.show();
				
				curr_cursor.close();
				
				displayWord(wHead.view, wHead.cur_row+1);
				
				//Log.e(TAG, "+++++++++++++++++++++++++");
				//Log.e(TAG, "def: " + def_total_cont);
				//Log.e(TAG, "usr: " + usr_total_cont);
				//Log.e(TAG, "+++++++++++++++++++++++++");
				SharedPreferences.Editor editor = app_pref.edit();
				editor.putInt("DEF_LINE_CNT", wHead.totalWord_1);
				editor.putInt("USR_LINE_CNT", wHead.totalWord_2);
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
					//	Log.e(TAG, "line >>>> " + line);
					//	Log.e(TAG, "count >>>> " + curr_cursor.getCount());

					curr_cursor.moveToPosition(line-1);
					dsipalyLine(curr_line_count,1);
				}

			}
		});

		// for next word
		btnNxt.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				//Log.e(TAG, " btnNxt +++" );
				//Log.e(TAG," Total count >>> " + curr_cursor.getCount());
				//Log.e("++++++","usr_total_cont : "+ usr_total_cont );
				//Log.e("++++++","def_total_cont : "+ def_total_cont );
				
				dsipalyLine(wHead.cur_row,1);
			}
		});
		// for back word
		btnBck.setOnClickListener(new View.OnClickListener() {
			public void onClick(View view) {
				// Get the ViewFlipper from the layout
				//	Log.e(TAG, " btnBck +++" );
				//	Log.e(TAG," Total count >>> " + curr_cursor.getCount());
				//Log.e("++++++","usr_total_cont : "+ usr_total_cont );
				//Log.e("++++++","def_total_cont : "+ def_total_cont );
				dsipalyLine(wHead.cur_row,0);
			}
		});

	}

	@Override
	protected void onNewIntent(Intent intent) {
		// Because this activity has set launchMode="singleTop", the system calls this method
		// to deliver the intent if this actvity is currently the foreground activity when
		// invoked again (when the user executes a search from this activity, we don't create
		// a new instance of this activity, so the system delivers the search intent here)
		//Log.e(TAG, " +++ onNewIntent +++");
		handleIntent(intent, mloaded);
	}

	private void handleIntent(Intent intent, boolean mloaded) {
		//Log.e(TAG, "handleIntent +++ ");
		//Log.e(TAG, "intent.getAction() : " + intent.getAction());
		ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
		if (Intent.ACTION_VIEW.equals(intent.getAction())) 
		{
			//	Log.e(TAG," --- ACTION_VIEW ---");
			// handles a click on a search suggestion; launches activity to show word
			//vf.setAnimation(AnimationUtils.loadAnimation(this.getApplicationContext(), R.anim.push_left_in));
			//vf.showNext();
			search1default0 = true;

		} else if (Intent.ACTION_SEARCH.equals(intent.getAction())) 
		{
			// handles a search query
			if(search_click == false)
			{
				vf.showPrevious();
				//Log.e(TAG," --- ACTION_SEARCH ---");
				String query = intent.getStringExtra(SearchManager.QUERY);
				//Log.e(TAG," query = "+query);
				showResults(query);
				search_click = true;
				search1default0 = false;
			}else {
				search_click = false;
				vf.setAnimation(AnimationUtils.loadAnimation(this.getApplicationContext(), R.anim.push_left_in));
				vf.showNext();
				search1default0 = true;

			}

		}else
		{
			vf.setAnimation(AnimationUtils.loadAnimation(this.getApplicationContext(), R.anim.push_left_in));
			vf.showNext();
			search1default0 = false;
		}
		
		//Bundle bundle = getIntent().getExtras();
		//memory1def0 = bundle.getBoolean("VIEW");
		//search1default0 = bundle.getBoolean("SEARCH");
		int rowNum = 0;
		//Log.e(TAG, "6666666666666666666 search1default0 " + search1default0);
		if(search1default0 == true)
		{
			Uri uri = intent.getData();
			//Log.e(TAG, "" + 1);
			//Log.e(TAG, "uri: " + uri);
			String[] srowNum = new String[] {uri.getLastPathSegment()};
			//Log.e(TAG,srowNum[0] + " " + Integer.valueOf(srowNum[0]));

			rowNum =  Integer.valueOf(srowNum[0]) - 1;
			int dec = 0;
			for(int i=0; i<=rowNum; i++)
			{
				if(wHead.view == false)
				{
					if(lrn_lines[i] == 1)
					{
						dec++;
					}
				}
				if(wHead.view == true)
				{
					if(lrn_lines[i] == 0)
					{
						dec++;
					}
				}
			}
			rowNum = rowNum - dec;
			//	Log.e(TAG, "''''''" + rowNum + " " + dec);

		}else
		{
			if(wHead.view == false)
			{
				//def_total_cont = app_pref.getInt("DEF_LINE_CNT", 0);
				rowNum = app_pref.getInt("def_Line", 0);
			}
			if(wHead.view == true)
			{
				//usr_total_cont = app_pref.getInt("USR_LINE_CNT", 0);
				rowNum = app_pref.getInt("user_Line", 0);
			}

		}
		displayWord(wHead.view, rowNum);
		//Log.e(TAG, "handleIntent --");
	}

	/**
	 * Searches the dictionary and displays results for the given query.
	 * @param query The search query
	 */
	private void showResults(String query) {
		// Log.e(TAG, "++++++++++showResults ++++++++++" + query);
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
			//Log.e(TAG, "2222222222222222222222" + count);

			String countString = getResources().getQuantityString(R.plurals.search_results,
					count, new Object[] {count, query});

			mTextView.setText(countString);

			//Log.e(TAG, "44444444444444444");
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

			//Log.e(TAG, "5555555555555555");
			// Define the on-click listener for the list items
			mListView.setOnItemClickListener(new OnItemClickListener() {
				public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
					// Build the Intent used to open WordActivity with a specific word Uri
					//Intent wordIntent = new Intent(getApplicationContext(), ShowWordDefActivity.class);
					Uri data = Uri.withAppendedPath(WordDefinationProvider.CONTENT_URI,
							String.valueOf(id));
					//         Log.e(TAG, ":  " + data);
					// setContentView(R.layout.search);
					// ViewFlipper vf = (ViewFlipper) findViewById(R.id.details);
					//	vf.showNext();
					//wordIntent.setData(data);


					Intent searchIntent = new Intent(getApplicationContext(), MainSearchWordActivity.class);
					searchIntent.setData(data);
					searchIntent.setAction("android.intent.action.SEARCH");
					//searchIntent.putExtra("VIEW", view);
					//startActivity(searchIntent);
					//	   Log.e(TAG,"Starting Activity");
					handleIntent(searchIntent,mloaded);

					//startActivity(wordIntent);
					// Log.e(TAG, " +++ onNewIntent +++");
					// handleIntent(intent, mloaded);
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
	public void onPause() {
		super.onPause();
		//Log.v("wovo", " +++ onPause +++");
		if(search == false)
		{
			readWriteLrnWordList(1);
		}
	}

	@Override
	public void onResume() {
		super.onResume();
		///Log.v("wovo", " +++ onResume +++");
		if(search ==false)
		{
			readWriteLrnWordList(0);
		}
		if(search == true)
		{
			search = false;
		}
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		//Log.e(TAG, "" + item.getItemId());
		switch (item.getItemId()) {
		case R.id.search:
			//   	Log.e(TAG,"R.id.search:");
			onSearchRequested();
			return true;
		case android.R.id.home:
			//     	Log.e(TAG,"R.id.home:");
			Intent intent = new Intent(this, WovoActivity.class);
			intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			startActivity(intent);
			finish();
			return true;
		default:
			return false;
		}
	}
	
	public class WovoDbHeader
	{

		private int totalWord_1;
		private int totalWord_2;
		private boolean view;
		private int curWrd_1;
		private int curWrd_2;
		private int cur_row;
		private int max_count;
		private int count;
		
		
	}
}
