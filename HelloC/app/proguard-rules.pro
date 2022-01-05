# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.
#
# For more details, see
#   http://developer.android.com/guide/developing/tools/proguard.html

# If your project uses WebView with JS, uncomment the following
# and specify the fully qualified class name to the JavaScript interface
# class:
#-keepclassmembers class fqcn.of.javascript.interface.for.webview {
#   public *;
#}

#-keepclassmembers class win.gatewalker.helloc.MainActivity {
    #public *;# MainActivity의 모든 함수를 유지 시킴(난독화 안함)
    #public static void test();# 등록된 함수는 난독화 안됨
#}
-keep public class * extends android.app.Activity

-keepclassmembers class win.gatewalker.game.App {
    #public *;
    win.gatewalker.game.App app;
    public void sendMessage*(...);

    static void loadBgmSound(...);
    static void loadSfxSound(...);
    static void playBgmSound(...);
    static void playSfxSound(...);
    static void stopBgmSound(...);
    static void stopSfxSound(...);
    static void volumeBgmSound(...);
    static void volumeSfxSound(...);
    static void pauseBgmSound(...);
}


-keepclassmembers class win.gatewalker.lib.Native {
    public *;#C에서 불러올 코드는 난독화를 하지 않아야 함
}

# Uncomment this to preserve the line number information for
# debugging stack traces.
#-keepattributes SourceFile,LineNumberTable

# If you keep the line number information, uncomment this to
# hide the original source file name.
#-renamesourcefileattribute SourceFile