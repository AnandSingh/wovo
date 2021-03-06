package com.anand.embvid.wovo;

import android.app.Application;
import android.app.SearchManager;
import android.content.ContentValues;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteException;
import android.database.sqlite.SQLiteOpenHelper;
import android.database.sqlite.SQLiteQueryBuilder;
import android.preference.PreferenceManager;
import android.provider.BaseColumns;
import android.text.TextUtils;
import android.util.Log;

import java.io.BufferedReader;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.HashMap;


/**
 * Contains logic to return specific words from the dictionary, and
 * load the dictionary table when it needs to be created.
 */
public class WordDatabase {
	
	//private static boolean mLoaded = false;
	private static final String IDENTITY_1 = "1";
	private static final String IDENTITY_2 = "2";
	private static final int MAX_WORDS = 4757;

    private static final String TAG = "222 >>>>>>>>>";

    //The columns we'll include in the dictionary table
    public static final String KEY_ID = "_id";
    public static final String KEY_WORD = SearchManager.SUGGEST_COLUMN_TEXT_1;
    public static final String KEY_DEFINITION = SearchManager.SUGGEST_COLUMN_TEXT_2;
    public static final String KEY_IDENTITY = "Identifier";
    
    
    private static final String DB_PATH = "/data/data/com.anand.embvid.wovo/databases/";
    private static final String DB_NAME = "wovodefdb";
    private static SQLiteDatabase mDatabase;

    private static final String FTS_VIRTUAL_TABLE = "FTSwovoVT";
    private static final int DATABASE_VERSION = 2;

    private final DictionaryOpenHelper mDatabaseOpenHelper;
    private static final HashMap<String,String> mColumnMap = buildColumnMap();
    
    
    private static Context app_context = null;
    /**
     * Constructor
     * @param context The Context within which to work, used to create the DB
     */
    public WordDatabase(Context context) {
    	
    //   	Log.e(TAG, "context: "+ context);
    	app_context = context;
    	mDatabaseOpenHelper = new DictionaryOpenHelper(context);
    	
    }


    /**
     * Builds a map for all columns that may be requested, which will be given to the 
     * SQLiteQueryBuilder. This is a good way to define aliases for column names, but must include 
     * all columns, even if the value is the key. This allows the ContentProvider to request
     * columns w/o the need to know real column names and create the alias itself.
     */
    private static HashMap<String,String> buildColumnMap() {
        HashMap<String,String> map = new HashMap<String,String>();
        map.put(KEY_WORD, KEY_WORD);
        map.put(KEY_DEFINITION, KEY_DEFINITION);
        map.put(KEY_IDENTITY, KEY_IDENTITY);
        map.put(BaseColumns._ID, "rowid AS " +
                BaseColumns._ID);
        map.put(SearchManager.SUGGEST_COLUMN_INTENT_DATA_ID, "rowid AS " +
                SearchManager.SUGGEST_COLUMN_INTENT_DATA_ID);
        map.put(SearchManager.SUGGEST_COLUMN_SHORTCUT_ID, "rowid AS " +
                SearchManager.SUGGEST_COLUMN_SHORTCUT_ID);
        return map;
    }

    /**
     * Returns a Cursor positioned at the word specified by rowId
     *
     * @param rowId id of word to retrieve
     * @param columns The columns to include, if null then all are included
     * @return Cursor positioned to matching word, or null if not found.
     */
 
    public int updateIndenty(String value, String[] selectionArgs) {
    	
    	ContentValues initialValues = new ContentValues();
        initialValues.put(KEY_IDENTITY, value);

        SQLiteDatabase db = mDatabaseOpenHelper.getWritableDatabase();
    	return db.update(FTS_VIRTUAL_TABLE, initialValues, KEY_WORD+"= ?", new String []{selectionArgs[0]});
        //return db.update(FTS_VIRTUAL_TABLE, initialValues, KEY_WORD+"= ?" + " OR " + "rowid = ?", selectionArgs);
    	
    }

   public Cursor getWord(String[] query, String[] columns) {
    	
	   Log.e("<< Database >>", ""+ "getWord : " +query[0] + " " + query[1] + " "+ query[2] );
	  
   	   if(query[1] == null)
   	   {
        String selection = "rowid >= ?" + " AND " + KEY_IDENTITY + "=?" ;
        String[] selectionArgs = new String[] {query[0], query[2]};
        return query(selection, selectionArgs, columns);
   	   }
   	   else
   	   {
   		  String selection = "rowid >= ?" + " AND " + "rowid <= ?" +" AND " + KEY_IDENTITY + "=?" ;
          String[] selectionArgs = new String[] {query[0], query[1], query[2]};
          return query(selection, selectionArgs, columns);
   	   
     	     
   	   }
        

        

        /* This builds a query that looks like:
         *     SELECT <columns> FROM <table> WHERE rowid = <rowId>
         */
    }
    /**
     * Returns a Cursor over all words that match the given query
     *
     * @param query The string to search for
     * @param columns The columns to include, if null then all are included
     * @return Cursor over all words that match, or null if none found.
     */
    public Cursor getWordMatches(String[] query, String[] columns) {
    	
        String selection = KEY_WORD + " MATCH ?" + " AND " + KEY_IDENTITY + "=?" ;
        String[] selectionArgs = new String[] {query[0]+"*", query[1]};

        return query(selection, selectionArgs, columns);
        

        /* This builds a query that looks like:
         *     SELECT <columns> FROM <table> WHERE <KEY_WORD> MATCH 'query*'
         * which is an FTS3 search for the query text (plus a wildcard) inside the word column.
         *
         * - "rowid" is the unique id for all rows but we need this value for the "_id" column in
         *    order for the Adapters to work, so the columns need to make "_id" an alias for "rowid"
         * - "rowid" also needs to be used by the SUGGEST_COLUMN_INTENT_DATA alias in order
         *   for suggestions to carry the proper intent data.
         *   These aliases are defined in the DictionaryProvider when queries are made.
         * - This can be revised to also search the definition text with FTS3 by changing
         *   the selection clause to use FTS_VIRTUAL_TABLE instead of KEY_WORD (to search across
         *   the entire table, but sorting the relevance could be difficult.
         */
    }
    
     /**
     * Performs a database query.
     * @param selection The selection clause
     * @param selectionArgs Selection arguments for "?" components in the selection
     * @param columns The columns to return
     * @return A Cursor over all rows matching the query
     */
   
    private Cursor query(String selection, String[] selectionArgs, String[] columns) {
        /* The SQLiteBuilder provides a map for all possible columns requested to
         * actual columns in the database, creating a simple column alias mechanism
         * by which the ContentProvider does not need to know the real column names
         */
    //	Log.e(TAG, "query ++");
        SQLiteQueryBuilder builder = new SQLiteQueryBuilder();
        builder.setTables(FTS_VIRTUAL_TABLE);
        builder.setProjectionMap(mColumnMap);

        
        Cursor cursor = builder.query(mDatabaseOpenHelper.getReadableDatabase(),
                columns, selection, selectionArgs, null, null, null);
        
       // Log.e(TAG, "cursor :" + cursor);
       
        if (cursor == null) {
            return null;
        } else if (!cursor.moveToFirst()) {
        	//return cursor;
            cursor.close();
            return null;
        }
        return cursor;
    }


    /**
     * This creates/opens the database.
     */
    private static class DictionaryOpenHelper extends AssestHelper {

        private final Context mHelperContext;
             
        //private SQLiteDatabase myDataBase; 
        //private SQLiteDatabase mDatabase;

        /* Note that FTS3 does not support column constraints and thus, you cannot
         * declare a primary key. However, "rowid" is automatically used as a unique
         * identifier, so when making requests, we will use "_id" as an alias for "rowid"
         */
        private static final String FTS_TABLE_CREATE =
                    "CREATE VIRTUAL TABLE " + FTS_VIRTUAL_TABLE +
                    " USING fts3 (" +
                    KEY_WORD + ", " +
                    KEY_DEFINITION  + ", " + KEY_IDENTITY + ");";
        
        DictionaryOpenHelper(Context context) {
            super(context, DB_NAME, null, DATABASE_VERSION);
            mHelperContext = context;
            SharedPreferences app_pref = PreferenceManager.getDefaultSharedPreferences(app_context);
          		SharedPreferences.Editor editor = app_pref.edit();
          		editor.putInt("DEF_LINE_CNT", MAX_WORDS);
          		editor.putInt("USR_LINE_CNT", 0);
      			editor.putBoolean("DB_LOADED", true);
          		editor.commit();
          	    Log.d(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>. DONE loading words.");
        }

  
      
        /*
        private void copyDataBase() throws IOException{
        	 Log.e("TAG", "copyDataBase +++");
        	//Open your local db as the input stream
        	InputStream myInput = app_context.getAssets().open(DB_NAME);
     
        	// Path to the just created empty db
        	String outFileName = DB_PATH + DB_NAME;
     
        	//Open the empty db as the output stream
        	OutputStream myOutput = new FileOutputStream(outFileName);
     
        	//transfer bytes from the inputfile to the outputfile
        	byte[] buffer = new byte[1024];
        	int length;
        	while ((length = myInput.read(buffer))>0){
        		myOutput.write(buffer, 0, length);
        	}
     
        	//Close the streams
        	myOutput.flush();
        	myOutput.close();
        	myInput.close();
     
        }

        private boolean checkDataBase(){
        	 
        	SQLiteDatabase checkDB = null;
     
        	try{
        		String myPath = DB_PATH + DB_NAME;
        		checkDB = SQLiteDatabase.openDatabase(myPath, null, SQLiteDatabase.OPEN_READONLY);
     
        	}catch(SQLiteException e){
                Log.e(TAG, "database does't exist yet.");
        		//database does't exist yet.
     
        	}
     
        	if(checkDB != null){
     
        		checkDB.close();
     
        	}
     
        	return checkDB != null ? true : false;
        }
        */
        /**
         * Starts a thread to load the database table with words
         */
        private void loadWords() throws IOException {
            Log.e(TAG, "Loading words.....");
            
            final Resources resources = mHelperContext.getResources();
            InputStream inputStream = resources.openRawResource(R.raw.list);
            BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream));
            try {
                String line;
                while ((line = reader.readLine()) != null) {
                    String[] strings = TextUtils.split(line, "-");
                    if (strings.length < 2) continue;
                    long id = addWord(strings[0].trim(), strings[1].trim(), IDENTITY_1);
                   
                    if (id < 0) {
                      //  Log.e(TAG, "unable to add word: " + strings[0].trim());
                    }
                    else{
                    	Log.e(TAG, "word added: " + strings[0].trim() + " : " + strings[1].trim());
                    }
                }
            } finally {
                reader.close();
            }
          
            SharedPreferences app_pref = PreferenceManager.getDefaultSharedPreferences(app_context);
    		SharedPreferences.Editor editor = app_pref.edit();
    		editor.putInt("DEF_LINE_CNT", MAX_WORDS);
    		editor.putInt("USR_LINE_CNT", 0);
			editor.putBoolean("DB_LOADED", true);
    		editor.commit();
    	    Log.d(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>. DONE loading words.");
        }
      
        /**
         * Add a word to the dictionary.
         * @return rowId or -1 if failed
         */
        public long addWord(String word, String definition, String identity) {
            ContentValues initialValues = new ContentValues();
            initialValues.put(KEY_WORD, word);
            initialValues.put(KEY_DEFINITION, definition);
            initialValues.put(KEY_IDENTITY, identity);
    
            return mDatabase.insert(FTS_VIRTUAL_TABLE, null, initialValues);
        }
        
        @Override
    	public synchronized void close() {
     
        	    //if(myDataBase != null)
        		//    myDataBase.close();
     
        	    super.close();
     
    	}
        
        @Override
        public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
            //db.execSQL("DROP TABLE IF EXISTS " + FTS_VIRTUAL_TABLE);
            //onCreate(db);
        }
    }

/*
	@Override
	public void onCreate(SQLiteDatabase arg0) {
		// TODO Auto-generated method stub
		
	}


	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
		// TODO Auto-generated method stub
		
	}
	*/
}
