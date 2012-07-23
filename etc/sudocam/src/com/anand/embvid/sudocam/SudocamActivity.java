package com.anand.embvid.sudocam;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore.Images;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ImageView;


public class SudocamActivity extends Activity implements OnClickListener {
    /** Called when the activity is first created. */

	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        // Set up click listeners for all the buttons
        View continueButton = this.findViewById(R.id.continue_button);
        continueButton.setOnClickListener(this);
        View newButton = this.findViewById(R.id.new_button);
        newButton.setOnClickListener(this);
        View aboutButton = this.findViewById(R.id.about_button);
        aboutButton.setOnClickListener(this);
        View exitButton = this.findViewById(R.id.exit_button);
        exitButton.setOnClickListener(this);
    }
        
        
        public void onClick(View v) {
            switch (v.getId()) {
            case R.id.about_button:
               Intent i = new Intent(this, About.class);
               startActivity(i);
               break;
            // More buttons go here (if any) ...
            case R.id.continue_button:
            	
            	//Intent cam = new Intent(this, StartCamera.class);
            	//cam.setData(getIntent().getData());
               	Intent gallery = new Intent(this, Gallery.class);
               	//gallery.setData(getIntent().getData());
                gallery.putExtra("VIEW", true);
                startActivity(gallery);
              
            	
                break;
            case R.id.new_button:
            	Intent gallery1 = new Intent(this, Gallery.class);
            	gallery1.putExtra("VIEW", false);
                startActivity(gallery1);
               //openNewGameDialog();
               break;
            
            
            case R.id.exit_button:
               finish();
               break;
     
            }
        }
        
     
    
                
        @Override
        public boolean onCreateOptionsMenu(Menu menu) {
        	super.onCreateOptionsMenu(menu);
        	MenuInflater inflater = getMenuInflater();
        	inflater.inflate(R.menu.menu, menu);
        	return true;
        }
        
        @Override
        public boolean onOptionsItemSelected(MenuItem item) {
        	switch (item.getItemId()) {
        	case R.id.settings:
        		startActivity(new Intent(this, Settings.class));
        		return true;
        		// More items go here (if any) ...
        	}
        return false;
        }

        private void openCameraPreview() 
        {
        	
        }
        /** Ask the user what difficulty level they want */
        private void openNewGameDialog() {
           new AlertDialog.Builder(this)
                .setTitle(R.string.new_game_title)
                .setItems(R.array.difficulty,
                 new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialoginterface,
                          int i) {
                       startGame(i);
                    }
                 })
                .show();
        }
        
        /** Start a new game with the given difficulty level */
        private void startGame(int i) {
           Log.d("TAG", "clicked on " + i);
           Log.d("TAG", "clicked on " + i);
           Intent intent = new Intent(SudocamActivity.this, Game.class);
           intent.putExtra(Game.KEY_DIFFICULTY, i);
           startActivity(intent);

           // Start game here...
        }
    
    
}