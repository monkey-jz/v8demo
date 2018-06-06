#include <jni.h>
#include <string>
#include "log.h"
#include "v8Helper.h"
#include "LocalCFunction.h"

jobject assetManager;

const char* js_call_c_method_soure;
const char* js_create_c_object_soure;
const char* c_call_js_method_soure;
const char* c_pass_object_toJs_soure;
const char* c_call_js_object_soure;

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_boyaa_v8demo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

static void _nativeInit(JNIEnv *env, jclass clazz,jobject assets)
{
    assetManager = assets;
    LOGI("native method _nativeInit ...");
    js_call_c_method_soure = v8Helper::loadScriptSource(env, assets, "script/js_call_c_method.js");
    js_create_c_object_soure = v8Helper::loadScriptSource(env, assets, "script/js_create_c_object.js");
    c_call_js_method_soure = v8Helper::loadScriptSource(env, assets, "script/c_call_js_method.js");
    c_pass_object_toJs_soure = v8Helper::loadScriptSource(env, assets, "script/c_pass_object_toJs.js");
    c_call_js_object_soure = v8Helper::loadScriptSource(env, assets, "script/c_call_js_object.js");
    v8Helper::Initialize();
}

static jstring _native_js_call_c_method(JNIEnv *env, jclass clazz)
{

    LOGI("native method _native_js_call_c_method ...");

    bool scriptExectueResult = v8Helper::js_call_c_method(js_call_c_method_soure);
    std::string result ;
    if(scriptExectueResult){
        result = "script exectue ... \nresult : success\ncallbackinfo :";
    }else{
        result = "script exectue failed:";
    }
    return env->NewStringUTF((result +LocalCFunction::getCallbackInfo()).c_str());
}
static jstring _native_js_create_c_object(JNIEnv *env, jclass clazz)
{
    LOGI("native method _native_js_create_c_object ...");
    bool scriptExectueResult = v8Helper::js_create_c_object(js_create_c_object_soure);
    std::string result ;
    if(scriptExectueResult){
        result = "script exectue ... \nresult : success\ncallbackinfo :";
    }else{
        result = "script exectue failed:";
    }

    return env->NewStringUTF((result +LocalCFunction::getCallbackInfo()).c_str());
}
static jstring _native_c_call_js_method(JNIEnv *env, jclass clazz)
{
    LOGI("native method _native_c_call_js_method ...");
    bool scriptExectueResult = v8Helper::c_call_js_method(c_call_js_method_soure);
    std::string result ;
    if(scriptExectueResult){
        result = "script exectue ... \nresult : success\ncallbackinfo :";
    }else{
        result = "script exectue failed:";
    }
    return env->NewStringUTF((result +LocalCFunction::getCallbackInfo()).c_str());
}
static jstring _native_c_pass_object_toJs(JNIEnv *env, jclass clazz)
{
    LOGI("native method _native_c_pass_object_toJs ...");
    bool scriptExectueResult = v8Helper::c_pass_object_toJs(c_pass_object_toJs_soure);
    std::string result ;
    if(scriptExectueResult){
        result = "script exectue ... \nresult : success\ncallbackinfo :";
    }else{
        result = "script exectue failed:";
    }
    return env->NewStringUTF((result +LocalCFunction::getCallbackInfo()).c_str());
}
static jstring _native_c_call_js_object(JNIEnv *env, jclass clazz)
{
    LOGI("native method _native_c_call_js_object ...");
    bool scriptExectueResult = v8Helper::c_call_js_object(c_call_js_object_soure);
    std::string result ;
    if(scriptExectueResult){
        result = "script exectue ... \nresult : success\ncallbackinfo :";
    }else{
        result = "script exectue failed:";
    }
    return env->NewStringUTF((result +LocalCFunction::getCallbackInfo()).c_str());

}
static void _nativeDestory(JNIEnv *env, jclass clazz)
{
    LOGI("native method _nativeDestory ...");
    v8Helper::ShutDown();
}

static JNINativeMethod _methodTable[] = {
        { "native_js_call_c_method", "()Ljava/lang/String;", (void*)_native_js_call_c_method },
        { "native_js_create_c_object", "()Ljava/lang/String;", (void*)_native_js_create_c_object },
        { "native_c_call_js_method", "()Ljava/lang/String;", (void*)_native_c_call_js_method},
        { "native_c_pass_object_toJs", "()Ljava/lang/String;", (void*)_native_c_pass_object_toJs },
        { "native_c_call_js_object", "()Ljava/lang/String;", (void*)_native_c_call_js_object },
        { "nativeInit", "(Landroid/content/res/AssetManager;)V", (void*)_nativeInit },
        { "nativeDestory", "()V", (void*)_nativeDestory },
};

int JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGI("JNI_OnLoad ...");
    JNIEnv* env = 0;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        return JNI_ERR;
    }
    jclass cls = env -> FindClass("com/boyaa/v8demo/MainActivity");

    if (cls == 0)
    {
        LOGI("JNI_OnLoad cls null");
        return JNI_ERR;
    }

    int len = sizeof(_methodTable) / sizeof(_methodTable[0]);
    if (JNI_OK != env->RegisterNatives(cls, _methodTable, len))
    {
        LOGE( "_registerNativeMethods: failed");
        if (env->ExceptionCheck())
        {
            env->ExceptionClear();
        }
    }

    return JNI_VERSION_1_4;
}