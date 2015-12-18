#include <jni.h>
#include <string>
#include <stockfishcli.h>
#include <search.cpp>
#include <android/log.h>

#define LOGD(TAG,...) __android_log_print(ANDROID_LOG_DEBUG  , TAG,__VA_ARGS__)

extern "C" {
  JNIEXPORT void JNICALL Java_org_lichess_stockfish_CordovaPluginStockfish_jniInit(JNIEnv *env, jobject obj);
  JNIEXPORT void JNICALL Java_org_lichess_stockfish_CordovaPluginStockfish_jniExit(JNIEnv *env, jobject obj);
  JNIEXPORT void JNICALL Java_org_lichess_stockfish_CordovaPluginStockfish_jniCmd(JNIEnv *env, jobject obj, jstring jcmd);
};

bool run = false;

auto readstdout = []() {
  std::streambuf* out = std::cout.rdbuf();
  std::ostringstream lichout;
  std::cout.rdbuf(lichout.rdbuf());

  LOGD("stockfishcli", "##> UP");

  run = true;

  while(run) {
    std::string output = lichout.str();
    lichout.str("");

    std::istringstream lichin(output);
    char line[1024];
    lichin.getline(line, 1024);

    if(strlen(line) > 0) {
      LOGD("stockfishcli", "##> %s", line);
    }
  };

  std::cout.rdbuf(out);
};

std::thread reader (readstdout);

JNIEXPORT void JNICALL Java_org_lichess_stockfish_CordovaPluginStockfish_jniInit(JNIEnv *env, jobject obj) {
  UCI::init(Options);
  PSQT::init();
  Bitboards::init();
  Position::init();
  Bitbases::init();
  Search::init();
  Eval::init();
  Pawns::init();
  Threads.init();
  Tablebases::init(Options["SyzygyPath"]);
  TT.resize(Options["Hash"]);

  jclass jstockfish = env->FindClass("org/lichess/stockfish/CordovaPluginStockfish");
  jmethodID f = env->GetMethodID(jstockfish, "f", "(Ljava/lang/String;)V");
  jstring msg = env->NewStringUTF("Pong");
  env->CallVoidMethod(obj, f, msg);
}

JNIEXPORT void JNICALL Java_org_lichess_stockfish_CordovaPluginStockfish_jniExit(JNIEnv *env, jobject obj) {
  run = false;
}

JNIEXPORT void JNICALL Java_org_lichess_stockfish_CordovaPluginStockfish_jniCmd(JNIEnv *env, jobject obj, jstring jcmd) {
  const char *cmd = env->GetStringUTFChars(jcmd, (jboolean *)0);
  LOGD("stockfishcli", "cmd %s", cmd);
  stockfishcli::commandInit();
  stockfishcli::command(cmd);
  env->ReleaseStringUTFChars(jcmd, cmd);
}
