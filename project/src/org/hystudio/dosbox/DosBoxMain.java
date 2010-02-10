package org.hystudio.dosbox;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.os.PowerManager;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;

public class DosBoxMain extends Activity {
	public static final String ApplicationName = "dosbox";
	private DemoGLSurfaceView mGLView;
	private PowerManager.WakeLock wakeLock;
	
	static {
		System.loadLibrary(ApplicationName);
	}

    /** Called when the activity is first created. */
	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
        mGLView = new DemoGLSurfaceView(this);
        setContentView(mGLView);
        // Receive keyboard events
        mGLView.setFocusableInTouchMode(true);
        mGLView.setFocusable(true);
        mGLView.requestFocus();
        PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
        wakeLock = pm.newWakeLock(PowerManager.SCREEN_DIM_WAKE_LOCK, ApplicationName);
        wakeLock.acquire(); 
    }
    
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}
}

class DemoRenderer implements GLSurfaceView.Renderer {

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
         nativeInit();
    }

    public void onSurfaceChanged(GL10 gl, int w, int h) {
        //gl.glViewport(0, 0, w, h);
        nativeResize(w, h);
    }

    public void onDrawFrame(GL10 gl) {
        nativeRender();
    }

    public void exitApp() {
         nativeDone();
    };

    private static native void nativeInit();
    private static native void nativeResize(int w, int h);
    private static native void nativeRender();
    private static native void nativeDone();

}

class DemoGLSurfaceView extends GLSurfaceView {
    public DemoGLSurfaceView(Activity context) {
        super(context);
        mParent = context;
        mRenderer = new DemoRenderer();
        setRenderer(mRenderer);
    }

    @Override
    public boolean onTouchEvent(final MotionEvent event) 
    {
        // TODO: add multitouch support (added in Android 2.0 SDK)
        int action = -1;
        if( event.getAction() == MotionEvent.ACTION_DOWN )
        	action = 0;
        if( event.getAction() == MotionEvent.ACTION_UP )
        	action = 1;
        if( event.getAction() == MotionEvent.ACTION_MOVE )
        	action = 2;
        if (  action >= 0 ) {
            nativeMouse( (int)event.getX(), (int)event.getY(), action );
        }
        return true;
    }

     public void exitApp() {
         mRenderer.exitApp();
     };

	@Override
	public boolean onKeyDown(int keyCode, final KeyEvent event) {
         nativeKey( keyCode, 1 );
         return true;
     }
	
	@Override
	public boolean onKeyUp(int keyCode, final KeyEvent event) {
         nativeKey( keyCode, 0 );
         return true;
     }

    DemoRenderer mRenderer;
    Activity mParent;

    public static native void nativeMouse( int x, int y, int action );
    public static native void nativeKey( int keyCode, int down );
}