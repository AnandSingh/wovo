package com.anand.embvid.sudocam;

import android.os.Bundle;
import android.preference.PreferenceActivity;
public class Settings extends PreferenceActivity {
@Override
protected void onCreate(Bundle savedInstanceState) {
super.onCreate(savedInstanceState);
addPreferencesFromResource(R.xml.settings);
}
}
