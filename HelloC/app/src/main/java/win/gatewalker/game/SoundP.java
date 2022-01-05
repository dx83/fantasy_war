package win.gatewalker.game;

import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.SoundPool;
import android.util.Log;

public class SoundP implements SoundPool.OnLoadCompleteListener
{
    @Override
    public void onLoadComplete(SoundPool soundPool, int sampleId, int status)
    {

    }

    SoundPool[] p;

    int[] bgmId;
    int bgmNum;
    int currBgmIndex;
    float bgmVolume;

    int[] sfxId;
    int sfxNum;
    float sfxVolume;

    public static final int bgmMaxStreams = 1;
    public static final int sfxMaxStreams = 3;
    public static final int SrcQuality = 0;

    SoundP()
    {
        p = new SoundPool[2];
        p[0] = new SoundPool(bgmMaxStreams, AudioManager.STREAM_MUSIC, SrcQuality);
        p[0].setOnLoadCompleteListener(this);
        p[1] = new SoundPool(sfxMaxStreams, AudioManager.STREAM_MUSIC, SrcQuality);
        p[1].setOnLoadCompleteListener(this);

        bgmId = new int[bgmMaxStreams];
        bgmNum = 0;
        currBgmIndex = 0;
        bgmVolume = 1.0f;

        sfxId = new int[sfxMaxStreams];
        sfxNum = 0;
        sfxVolume = 1.0f;
    }

    void bgmLoad(String path)
    {
        try{
            AssetFileDescriptor afd = App.app.getAssets().openFd(path);
            bgmId[bgmNum] = p[0].load(afd, 0);
            afd.close();
            bgmNum++;

        } catch (Exception e){};
    }

    void sfxLoad(String path)
    {
        try{
            AssetFileDescriptor afd = App.app.getAssets().openFd(path);
            sfxId[sfxNum] = p[1].load(afd, 0);
            afd.close();
            sfxNum++;
        } catch (Exception e){};
    }

    void bgmPlay(int index)
    {
        int priority = 0;
        int loop = -1;
        float rate = 1.0f;
        p[0].play(bgmId[index], bgmVolume, bgmVolume, priority, loop, rate);
        currBgmIndex = index;
    }

    void sfxPlay(int index)
    {
        int priority = 0;
        int loop = 0;
        float rate = 1.0f;
        p[1].play(sfxId[index], sfxVolume, sfxVolume, priority, loop, rate);
    }

    void bgmStop(int index)
    {
        p[0].stop(bgmId[index]);
    }

    void sfxStop(int index)
    {
        p[1].stop(sfxId[index]);
    }

    void bgmVolume(float volume)
    {
        bgmVolume = volume;
    }

    void sfxVolume(float volume)
    {
        sfxVolume = volume;

    }

    void bgmPause(boolean pause)
    {
        if( pause ) bgmStop(currBgmIndex);
        else bgmPlay(currBgmIndex);
    }
}
