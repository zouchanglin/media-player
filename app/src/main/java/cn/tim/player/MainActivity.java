package cn.tim.player;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    // 申请权限请求码
    private static final int REQUEST_EXTERNAL_STORAGE = 1001;
    private SuperPlayer superPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        verifyStoragePermissions(this);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        superPlayer = new SuperPlayer();
        TextView versionTextView = findViewById(R.id.ffmpeg_version_tv);
        TextView errorTextView = findViewById(R.id.error_code_tv);
        versionTextView.setText(superPlayer.getFFmpegVersion());

        superPlayer.setDataSource(new File(Environment.getExternalStorageDirectory()
                + File.separator + "demo.mp4").getAbsolutePath());

        // 准备成功回调处 C++子线程调用
        superPlayer.setOnPreparedListener(() -> {
            runOnUiThread(()-> {
                errorTextView.setTextColor(Color.GREEN);
                errorTextView.setText("init success!");
            });
            // 调用C++层开始播放
            superPlayer.start();
        });

        superPlayer.setOnErrorListener(errorCode -> {
            runOnUiThread(()->{
                errorTextView.setTextColor(Color.RED);
                errorTextView.setText(errorCode);
            });
        });
    }

    // 触发准备工作
    @Override
    protected void onResume() {
        super.onResume();
        if(superPlayer != null) {
            superPlayer.prepare();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        if(superPlayer != null){
            superPlayer.stop();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if(superPlayer != null){
            superPlayer.release();
        }
    }

    // 检查权限
    public static void verifyStoragePermissions(Activity activity) {
        int writePermission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        int readPermission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.READ_EXTERNAL_STORAGE);

        if (writePermission != PackageManager.PERMISSION_GRANTED
                || readPermission != PackageManager.PERMISSION_GRANTED) {
            // 如果没有权限需要动态地去申请权限
            ActivityCompat.requestPermissions(
                    activity,
                    // 权限数组
                    new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    // 权限请求码
                    REQUEST_EXTERNAL_STORAGE
            );
        }
    }
}