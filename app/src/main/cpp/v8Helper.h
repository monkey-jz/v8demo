//
// Created by JerryZhu on 2018/5/25.
//

#ifndef V8DEMO_V8HELPER_H
#define V8DEMO_V8HELPER_H
#define OBJ_TEMPLATE(obj,isolate) Local<ObjectTemplate> obj = ObjectTemplate::New(isolate)
#define EXPOSE(obj,isolate, jsname, func) obj->Set(String::NewFromUtf8(isolate, jsname, NewStringType::kNormal).ToLocalChecked(),FunctionTemplate::New(isolate, func));

#include <string>
#include <jni.h>
#include "include/v8.h"

using namespace v8;

class v8Helper {

public:

    static void ReportException(Isolate* isolate, TryCatch* try_catch);
    static bool ExecuteString(Isolate* isolate, Local<String> source,
                               bool print_result,
                               bool report_exceptions);

    static void Initialize();
    static Isolate* GetIsolate();
    static void ShutDown();
    static const char *loadScriptSource(JNIEnv *env, jobject assetManager, const char* name);
    static Global<Function>* getGlobalFunction();
    static Global<Context>*  getGlobalContext();
    static bool js_call_c_method(const char *soure);
    static bool js_create_c_object(const char *soure);
    static bool c_call_js_method(const char *soure);
    static bool c_pass_object_toJs(const char *soure);

    static bool c_call_js_object(const char *soure);
};


#endif //V8DEMO_V8HELPER_H
