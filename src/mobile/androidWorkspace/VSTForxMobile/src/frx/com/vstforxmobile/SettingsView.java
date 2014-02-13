package frx.com.vstforxmobile;

import com.example.vstforxmobile.R;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

public class SettingsView extends BaseFragment {
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) 
	{
		View v = super.onCreateView(inflater, container, savedInstanceState);
		return v;
	}
	
	protected View getRootView(LayoutInflater inflater, ViewGroup container) {
		return inflater.inflate(R.layout.settings_view, container, false);
	}
}
