package win.gatewalker.game;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.PixelFormat;
import android.opengl.GLES10;
import android.opengl.GLES20;
import android.opengl.GLES32;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowInsets;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import win.gatewalker.lib.Native;

/*
    1. 기존 프로젝트  복붙한다.
        폴더(game, lib), AndroidManifest.xml, Activity_main.xml

    2. 구글플레이 서비스 적용
    - google id
    - leaderboard/achievement id
    - ads id

    3. keystore
    - 잃으버리면 안됨, 보관 잘해야함

    4. proguard *.pro 보호해야 할 클래스명, 함수명, 필드면
 */

public class App extends AppCompatActivity {

    public Object getAssetMgr()
    {
        return getAssets();
    }

    public static App app;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        app = this;

        RelativeLayout layout = new RelativeLayout(getApplicationContext());
        RelativeLayout.LayoutParams parms = new RelativeLayout.LayoutParams(
                RelativeLayout.LayoutParams.MATCH_PARENT,
                RelativeLayout.LayoutParams.MATCH_PARENT);
        layout.setLayoutParams(parms);

        GLView view = new GLView(getApplicationContext());
        layout.addView(view);

        setContentView(layout);

        // FullScreen
        //if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT)
        //{
        //getWindow().setFlags(//FLAG_LAYOUT_NO_LIMITS, FLAG_FULLSCREEN
        //        WindowManager.LayoutParams.FIRST_SYSTEM_WINDOW,
        //        WindowManager.LayoutParams.FIRST_SYSTEM_WINDOW);
        view.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE   |       // 키 적용되게 해줌
                View.SYSTEM_UI_FLAG_FULLSCREEN  |
                View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);   // 키, 터치 막음
    }

    SoundP sp = new SoundP();
    static void loadBgmSound(String path) { app.sp.bgmLoad(path); }
    static void loadSfxSound(String path) { app.sp.sfxLoad(path); }
    static void playBgmSound(int index) { app.sp.bgmPlay(index); }
    static void playSfxSound(int index) { app.sp.sfxPlay(index); }
    static void stopBgmSound(int index) { app.sp.bgmStop(index); }
    static void stopSfxSound(int index) { app.sp.sfxStop(index); }
    static void volumeBgmSound(float volume) { app.sp.bgmVolume(volume); }
    static void volumeSfxSound(float volume) { app.sp.sfxVolume(volume); }
    static void pauseBgmSound(boolean pause) { app.sp.bgmPause(pause);}

    MyHandler myHandler = new MyHandler(this);
    //int test = 0;
    public void sendMessage(int what, String data)
    {
        Message msg = new Message();
        msg.what = what;
        msg.obj = new String(data);
        myHandler.sendMessage(msg);
    }

    public void eventMessage(Message msg)
    {
        Log.i("ssm", "what = " + msg.what);
        if( msg.what==Msg_Login )
        {
            String strID = (String)msg.obj;
        }
        else if( msg.what==Msg_Logout )
        {

        }
    }

    public final static int Msg_Login = 0;
    public final static int Msg_Logout = 1;
    public final static int Msg_Cloud_Write = 2;
    public final static int Msg_Cloud_Read = 3;

    static class MyHandler extends Handler
    {

        //약하게 참조 : App이 삭제되더라도 Myhandler는 게속 상주
        WeakReference<App> handler;

        MyHandler(App app)
        {
            handler = new WeakReference<App>(app);
        }

        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);

            App a = handler.get();
            if( a!=null )
                a.eventMessage(msg);
        }
    }

    @Override
    protected void onStart()
    {
        super.onStart();
        // do something
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        Native.freeGame();
    }

    @Override
    protected void onStop()
    {
        super.onStop();
        // do something
    }

    @Override
    protected void onPause()
    {
        super.onPause();

        Native.pauseGame(true);
    }

    @Override
    protected void onResume()
    {
        super.onResume();

        Native.pauseGame(false);
    }
}

class GLView extends GLSurfaceView  // 화면 제어
{
    public GLView(Context context)
    {
        super(context);

        // OpenGl 설정
        setEGLContextClientVersion(2);
        setEGLConfigChooser(8, 8, 8, 8, 8,0);
        getHolder().setFormat(PixelFormat.RGBA_8888);
        setRenderer(new Render(context));
        // 일관되게 계속 뿌려야 하게 때문에 아래는 주석 처리
        //setRendererMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
        //setRendererMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY)

        enableMulti = false;
        mi = new int[20];
        mx = new float[20];
        my = new float[20];
    }

    boolean enableMulti;
    int mi[];
    float mx[];
    float my[];

    public void setEnableMulti(boolean enable)
    {
        enableMulti = enable;
    }

    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        // 터치가 하나일때 single
        int state;
        switch(e.getAction())
        {
            case MotionEvent.ACTION_DOWN:   state = 0;  break;
            case MotionEvent.ACTION_MOVE:   state = 1;  break;
            case MotionEvent.ACTION_UP:     state = 2;  break;
            default:                        state = e.getAction();
        }
        Native.keyGame(state, e.getX(), e.getY());

        if( !enableMulti )
            return true;

        // 터치가 여러개일때 multi
        int i, j, num = e.getPointerCount(), upIndex = 0xFF;

        switch(e.getAction() & MotionEvent.ACTION_MASK)
        {
            case MotionEvent.ACTION_CANCEL:
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                upIndex = (e.getAction() & MotionEvent.ACTION_POINTER_ID_MASK) >> MotionEvent.ACTION_POINTER_ID_SHIFT;
                break;
        }

        for (i = 0, j = 0; i < num; i++)
        {
            if(i == upIndex) continue;

            mi[j] = e.getPointerId(i);
            mx[j] = e.getX(i);
            my[j] = e.getY(i);
            j++;
        }
        Native.multiKeyGame(state, j, mx, my);

        return true;
    }
}

class Render implements GLSurfaceView.Renderer    // 그리기
{
    Context context;

    public Render(Context context)
    {
        this.context = context;
    }

    @Override
    public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig)
    {
        String apkFilePath = null;
        ApplicationInfo appInfo = null;
        PackageManager packageManager = context.getPackageManager();

        try {
            String packageName = App.app.getApplication().getPackageName();
            appInfo = packageManager.getApplicationInfo(packageName, 0);
        } catch(Exception e) {
            e.printStackTrace();
            throw new RuntimeException("경로 찾다가, 프로그램 종료되었습니다.");
        }
        apkFilePath = appInfo.sourceDir;

        String ioPath = context.getFilesDir().getAbsolutePath();

        Native.loadGame(apkFilePath, ioPath);
    }

    @Override
    public void onSurfaceChanged(GL10 gl10, int w, int h) { Native.resizeGame(w, h); }

    @Override
    public void onDrawFrame(GL10 gl10) { Native.drawGame(); }
}

