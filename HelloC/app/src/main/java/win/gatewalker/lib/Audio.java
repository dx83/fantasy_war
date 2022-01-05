package win.gatewalker.lib;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

import java.io.InputStream;

import win.gatewalker.game.App;

//App.java -> onSurfaceCreated -> line 290 : new Audio();
public class Audio implements Runnable
{
    AudioTrack audioTrack;
    boolean isPlaying;

    byte[] bgm;
    int offBgm;

    byte[] eff;
    int offEff;

    boolean toggle;

    int bufferSize;

    public Audio()
    {
        int sampleRate = 22050;//44100
        int channelCount = AudioFormat.CHANNEL_IN_STEREO;
        int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        bufferSize = AudioTrack.getMinBufferSize(sampleRate, channelCount, audioFormat);
        audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                                    sampleRate,
                                    channelCount,
                                    audioFormat,
                                    bufferSize,
                                    audioTrack.MODE_STREAM);
        isPlaying = true;
        toggle = false;

        bgm = getStream("00maintitle.wav");
        offBgm = 0;

        eff = getStream("01ingame.wav");
        offEff = 0;

        offBgm = 0;

        new Thread(this).start();
    }

    byte[] getStream(String fileName)
    {
        byte[] b = null;

        try {
            InputStream is = App.app.getAssets().open(fileName);
            int len = is.available();
            b = new byte[len];
            is.read(b);
            is.close();

        } catch (Exception e) {
            Log.e("ssm", "AUDIO ERROR");
            return null;
        }

        return b;
    }

    public void run()
    {
        audioTrack.play();

        while( isPlaying )
        {
            try {
                if( toggle )
                {
                    int len = audioTrack.write(bgm, offBgm, bufferSize);// 실제 재생
                    offBgm += len;
                    if( offBgm >= bgm.length )
                        offBgm = 0;
                }
                else
                {
                    int len = audioTrack.write(eff, offEff, bufferSize);// 실제 재생
                    offEff += len;
                    if( offEff >= eff.length )
                        offEff = 0;
                }
                toggle = !toggle;
            } catch (Exception e) {}
        }

        audioTrack.stop();
        audioTrack.release();
    }
}

