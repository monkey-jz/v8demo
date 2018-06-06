package com.boyaa.v8demo;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity implements View.OnClickListener{

    private static final String TAG = "MainActivity";

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public static native String native_js_call_c_method();
    public static native String native_js_create_c_object();
    public static native String native_c_call_js_method();
    public static native String native_c_pass_object_toJs();
    public static native String native_c_call_js_object();
    public static native void nativeInit(AssetManager assets);
    public static native void nativeDestory();

    private TextView mCallback_content;
    private Button mJs_call_c_method;
    private Button mJs_create_c_object;
    private Button mC_call_js_method;
    private Button mC_pass_object_toJs;
    private Button mC_call_js_object;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        mCallback_content = (TextView) findViewById(R.id.callback_content);
        mJs_call_c_method = (Button) findViewById(R.id.js_call_c_method);
        mJs_create_c_object = (Button) findViewById(R.id.js_create_c_object);
        mC_call_js_method = (Button) findViewById(R.id.c_call_js_method);
        mC_pass_object_toJs = (Button) findViewById(R.id.c_pass_object_toJs);
        mC_call_js_object = (Button) findViewById(R.id.c_call_js_object);
        mJs_call_c_method.setOnClickListener(this);
        mJs_create_c_object.setOnClickListener(this);
        mC_call_js_method.setOnClickListener(this);
        mC_pass_object_toJs.setOnClickListener(this);
        mC_call_js_object.setOnClickListener(this);
        nativeInit(getAssets());
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();


    @Override
    public void onClick(View v) {
        String result = null;
        switch (v.getId()){
            case R.id.js_call_c_method :
                   result = native_js_call_c_method();
                break;
            case R.id.js_create_c_object :
                result = native_js_create_c_object();
                break;
            case R.id.c_call_js_method :
                result = native_c_call_js_method();
                break;
            case R.id.c_pass_object_toJs :
                result = native_c_pass_object_toJs();
                break;
            case R.id.c_call_js_object :
                result = native_c_call_js_object();
                break;
            default:
                break;
        }
        mCallback_content.setText(result);
    
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        nativeDestory();
    }
}
