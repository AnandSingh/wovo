package com.anand.embivid.wovo;





import android.app.Activity;
import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class WovoActivity extends Activity {
    

	private Button btnNxt;
	private Button btnBck;
    private TextView tvWrd;
    private TextView tvDef;
    private TextView tvDebug;
    private int intLastLine = 0;
    String line = null;
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
        
        
        
        btnNxt = (Button) findViewById(R.id.button1);
        btnBck = (Button) findViewById(R.id.button2);
        tvWrd = (TextView) findViewById(R.id.word);
        tvDef = (TextView) findViewById(R.id.definition);
        tvDebug = (TextView) findViewById(R.id.num);
        
  
        tvDebug.setText("");

       
        // wiat till it load or checks the data base
        while(Lists.getInstance().isLoaded() == false);
        
        // set the default text based on the last prefrence value
        intLastLine = 4758;
        line = Lists.getInstance().setLineCount(intLastLine);
        if(line != null)
        {
           Lists.getInstance().splitText(line);
           tvWrd.setText(Lists.getInstance().getWord());
           tvDef.setText(Lists.getInstance().getDefine());
           tvDebug.setText(Lists.getInstance().getLineCount());
        }else
        {
        	 tvWrd.setText("");
             tvDef.setText("Fail to Load database");
      	  Log.d("wovo", "Default Fail...");
        }
        
       
       
       btnNxt.setOnClickListener(new View.OnClickListener() {
              public void onClick(View view) {
                  String line = Lists.getInstance().Next_list();
                  
                  if(line != null)
                  {
                	  Lists.getInstance().splitText(line);
                      tvWrd.setText(Lists.getInstance().getWord());
                      tvDef.setText(Lists.getInstance().getDefine());
                      tvDebug.setText(Lists.getInstance().getLineCount());
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
                   tvDebug.setText(Lists.getInstance().getLineCount());
               }else
               {
            	   Log.d("wovo", "Back Fail...");
               }
           }
       });

    }
	
	
}