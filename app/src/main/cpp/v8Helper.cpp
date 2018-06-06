//
// Created by JerryZhu on 2018/5/25.
//

#include <sys/types.h>
#include <stdlib.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include <assert.h>
#include "include/libplatform/libplatform.h"
#include "v8Helper.h"
#include "log.h"
#include "ContextWrapper.h"
#include "LocalCFunction.h"
#include "Person.h"

Local<v8::Context> context;
Global<Context> context_;
Global<Function> function_;
static Global<ObjectTemplate> person_template_;

Global<Context>* v8Helper::getGlobalContext()
{
    return &context_;
}
Global<Function>* v8Helper::getGlobalFunction()
{
    return &function_;
}

 bool v8Helper::ExecuteString(Isolate *isolate, Local<String> source, bool print_result,
                              bool report_exceptions)
 {
    LOGI("execute js source ....");
     HandleScope handle_scope(isolate);
     TryCatch try_catch(isolate);

     Local<Context> context(isolate->GetCurrentContext());
     Local<Script> script;
     if (!Script::Compile(context, source).ToLocal(&script)) {
         // Print errors that happened during compilation.
         if (report_exceptions)
             v8Helper::ReportException(isolate, &try_catch);
         return false;
     } else {
         Local<Value> result;
         if (!script->Run(context).ToLocal(&result)) {
             assert(try_catch.HasCaught());
             // Print errors that happened during execution.
             if (report_exceptions)
                 v8Helper::ReportException(isolate, &try_catch);
             return false;
         } else {
             assert(!try_catch.HasCaught());
             if (print_result && !result->IsUndefined()) {
                 // If all went well and the result wasn't undefined then print
                 // the returned value.
                 String::Utf8Value str(isolate, result);
                 LOGI("execute js source result %s\n", *str);
             }
             return true;
         }
     }
 }
void v8Helper::ReportException(Isolate *isolate, TryCatch *try_catch)
{
    HandleScope handle_scope(isolate);
    String::Utf8Value exception(isolate, try_catch->Exception());
//    const char *exception_string = ToCString(exception);
    Local<Message> message = try_catch->Message();
    if (message.IsEmpty()) {
        // V8 didn't provide any extra information about this error; just
        // print the exception.
        LOGI("exception_string ; %s\n", *exception);
    } else {
        // Print (filename):(line number): (message).
        String::Utf8Value filename(isolate,
                                       message->GetScriptOrigin().ResourceName());
        Local<Context> context(isolate->GetCurrentContext());
        int linenum = message->GetLineNumber(context).FromJust();
        LOGI("exception_string : %s:%i: %s\n",  *filename, linenum, *exception);
        // Print line of source code.
        String::Utf8Value sourceline(
                isolate, message->GetSourceLine(context).ToLocalChecked());
        LOGI("stderr :%s\n",*sourceline);
        // Print wavy underline (GetUnderline is deprecated).
        int start = message->GetStartColumn(context).FromJust();
        for (int i = 0; i < start; i++) {
            fprintf(stderr, " ");
        }
        int end = message->GetEndColumn(context).FromJust();
        for (int i = start; i < end; i++) {
            fprintf(stderr, "^");
        }
        fprintf(stderr, "\n");
        Local<Value> stack_trace_string;
        if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
            stack_trace_string->IsString() &&
            Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
            String::Utf8Value stack_trace(isolate, stack_trace_string);
            LOGI("exception_string : %s\n\n",*stack_trace);
        }
    }

}

Isolate::CreateParams create_params;
Isolate *isolate;

namespace  v8::internal
{
    void ReadNatives(){}
    void DisposeNatives(){}
    void SetNativesFromFile(v8::StartupData *s){}
    void SetSnapshotFromFile(v8::StartupData *s){}
}

void v8Helper::Initialize()
{
    LOGI("Initialize ...");
    Platform *platform = platform::CreateDefaultPlatform();
    V8::InitializePlatform(platform);
    V8::Initialize();
    create_params.array_buffer_allocator =
            ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate =Isolate::New(create_params);

}
Isolate* v8Helper::GetIsolate()
{
    return isolate;
}

void v8Helper::ShutDown()
{
    GetIsolate()->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete create_params.array_buffer_allocator;
}
//JS调用C++全局方法
bool v8Helper::js_call_c_method(const char *jsSource) {

    Isolate::Scope isolate_scope(GetIsolate());
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(GetIsolate());
    // Create a new context.
    Local<Context> context =ContextWrapper::createContext1(GetIsolate());
    if (context.IsEmpty())
    {
        LOGI("Error creating context\n");
    }
    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);
    // Create a string containing the JavaScript source code.
    bool result = ExecuteString(context->GetIsolate(),
                           String::NewFromUtf8(context->GetIsolate(), jsSource,
                                               NewStringType::kNormal).ToLocalChecked(),true, true);
    LOGI("JS Script Execute Result :%d", result);

    return result;
}
//在JS中创建C++对象，访问对象属性和方法
bool v8Helper::js_create_c_object(const char *jsSource) {
    Isolate::Scope isolate_scope(GetIsolate());
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(GetIsolate());
    // Create a new context.
    Local<Context> context =ContextWrapper::createContext2(GetIsolate());

    if (context.IsEmpty())
    {
        LOGI("Error creating context\n");
    }
    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);
    // Create a string containing the JavaScript source code.
    bool result = ExecuteString(context->GetIsolate(),
                                String::NewFromUtf8(context->GetIsolate(), jsSource,
                                                    NewStringType::kNormal).ToLocalChecked(),true, true);
    LOGI("JS Script Execute Result :%d", result);

    return result;
}
//C++调用JS方法
bool v8Helper::c_call_js_method(const char *jsSource) {

    Isolate::Scope isolate_scope(GetIsolate());
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(GetIsolate());
    // Create a new context.
    Local<Context> context =ContextWrapper::createContext3(GetIsolate());
    //store context
    context_.Reset(v8Helper::GetIsolate(),context);
    if (context.IsEmpty())
    {
        LOGI("Error creating context\n");
    }
    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);
    // Create a string containing the JavaScript source code.
    bool result = ExecuteString(context->GetIsolate(),
                                String::NewFromUtf8(context->GetIsolate(), jsSource,
                                                    NewStringType::kNormal).ToLocalChecked(),true, true);
    LOGI("JS Script Execute Result :%d", result);

    //调用js方法
    Local<String> function_name =
            String::NewFromUtf8(v8Helper::GetIsolate(), "jsFunction", NewStringType::kNormal)
                    .ToLocalChecked();
    Local<Value> jsFunction_val;
    // If there is no Process function, or if it is not a function,
    if (!context->Global()->Get(context, function_name).ToLocal(&jsFunction_val) ||
        !jsFunction_val->IsFunction()) {
        LOGI("jsFunction is not a function\n");
    }
    // It is a function; cast it to a Function
    Local<Function> jsFunction_fun = Local<Function>::Cast(jsFunction_val);
    function_.Reset(v8Helper::GetIsolate(),jsFunction_fun);

    //可以用context_和function_在以后的任何时候调用js方法，这里就直接调用了
    // Create a handle scope to keep the temporary object references.
//    HandleScope handle_scope(v8Helper::GetIsolate());

//    v8::Local<v8::Context> context =
//            v8::Local<v8::Context>::New(v8Helper::GetIsolate(), context_);
    TryCatch try_catch(v8Helper::GetIsolate());
    const int argc = 0;
    Local<Value> argv[argc] = {};
//    v8::Local<v8::Function> process =
//            v8::Local<v8::Function>::New(v8Helper::GetIsolate(), process_);
    Local<Value> call_result;
    if (!jsFunction_fun -> Call(context, context->Global(), argc, argv).ToLocal(&call_result)) {
        String::Utf8Value error(v8Helper::GetIsolate(), try_catch.Exception());
        LOGI("call js function error:%s",*error);
    }
    return result;
}

Local<ObjectTemplate> MakePersonTemplate(Isolate *isolate) {

        EscapableHandleScope handle_scope(isolate);

        Local<ObjectTemplate> result = ObjectTemplate::New(isolate);

        result->SetInternalFieldCount(1);

        // Add accessors for each of the fields of the request.
        result->SetAccessor(
                String::NewFromUtf8(isolate, "name", NewStringType::kInternalized)
                        .ToLocalChecked(),
                LocalCFunction::getName);
        result->SetAccessor(
                String::NewFromUtf8(isolate, "age", NewStringType::kInternalized)
                        .ToLocalChecked(),
                LocalCFunction::getAge);

        result->Set(String::NewFromUtf8(isolate, "setName", NewStringType::kNormal)
                            .ToLocalChecked(),
                    FunctionTemplate::New(isolate, LocalCFunction::setName));
        result->Set(String::NewFromUtf8(isolate, "setAge", NewStringType::kNormal)
                            .ToLocalChecked(),
                    FunctionTemplate::New(isolate, LocalCFunction::setAge));

        // Again, return the result through the current handle scope.
        return handle_scope.Escape(result);
}

/**
 * Utility function that wraps a C++ http person object in a
 * JavaScript object.
 */
Local<Object> WrapPerson(Person *person) {
    // Local scope for temporary handles.
    EscapableHandleScope handle_scope(v8Helper::GetIsolate());
    // Fetch the template for creating JavaScript person wrappers.
    // It only has to be created once, which we do on demand.
    if (person_template_.IsEmpty()) {
        Local<ObjectTemplate> raw_template = MakePersonTemplate(v8Helper::GetIsolate());
        person_template_.Reset(v8Helper::GetIsolate(), raw_template);
    }
    Local<ObjectTemplate> temp = Local<ObjectTemplate>::New(v8Helper::GetIsolate(),person_template_);
    // Create an empty  person wrapper.
    Local<Object> result = temp -> NewInstance(v8Helper::GetIsolate() -> GetCurrentContext()).ToLocalChecked();
    // Wrap the raw C++ pointer in an External so it can be referenced
    // from within JavaScript.
    Local<External> person_ptr = External::New(v8Helper::GetIsolate(),person);

    // Store the person pointer in the JavaScript wrapper.
    result->SetInternalField(0, person_ptr);
    // Return the result through the current handle scope.  Since each
    // of these handles will go away when the handle scope is deleted
    // we need to call Close to let one, the result, escape into the
    // outer handle scope.
    return handle_scope.Escape(result);
}

//C++传递对象给JS调用
bool v8Helper::c_pass_object_toJs(const char *jsSource) {
    Isolate::Scope isolate_scope(GetIsolate());
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(GetIsolate());
    // Create a new context.
    Local<Context> context =ContextWrapper::createContext3(GetIsolate());
    //store context
    context_.Reset(v8Helper::GetIsolate(),context);
    if (context.IsEmpty())
    {
        LOGI("Error creating context\n");
    }
    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);
    // Create a string containing the JavaScript source code.
    bool result = ExecuteString(context->GetIsolate(),
                                String::NewFromUtf8(context->GetIsolate(), jsSource,
                                                    NewStringType::kNormal).ToLocalChecked(),true, true);
    LOGI("JS Script Execute Result :%d", result);

    //调用js方法
    Local<String> function_name =
            String::NewFromUtf8(v8Helper::GetIsolate(), "jsFunction", NewStringType::kNormal)
                    .ToLocalChecked();
    Local<Value> jsFunction_val;
    // If there is no Process function, or if it is not a function,
    if (!context->Global()->Get(context, function_name).ToLocal(&jsFunction_val) ||
        !jsFunction_val->IsFunction()) {
        LOGI("jsFunction is not a function\n");
    }
    // It is a function; cast it to a Function
    Local<Function> jsFunction_fun = Local<Function>::Cast(jsFunction_val);
    function_.Reset(v8Helper::GetIsolate(),jsFunction_fun);

    //可以用context_和function_在以后的任何时候调用js方法，这里就直接调用了
    // Create a handle scope to keep the temporary object references.
//    HandleScope handle_scope(v8Helper::GetIsolate());

//    v8::Local<v8::Context> context =
//            v8::Local<v8::Context>::New(v8Helper::GetIsolate(), context_);

    Person person("xiao ming",20);
    Local<Object> person_object = WrapPerson(&person);

    TryCatch try_catch(v8Helper::GetIsolate());
    const int argc = 1;
    Local<Value> argv[argc] = {person_object};
//    v8::Local<v8::Function> function =
//            v8::Local<v8::Function>::New(v8Helper::GetIsolate(), function_);
    Local<Value> call_result;
    if (!jsFunction_fun -> Call(context, context->Global(), argc, argv).ToLocal(&call_result)) {
        String::Utf8Value error(v8Helper::GetIsolate(), try_catch.Exception());
        LOGI("call js function error:%s",*error);

    }
        return result;
}
//C++调用JS类
bool v8Helper::c_call_js_object(const char *jsSource) {

    Isolate::Scope isolate_scope(GetIsolate());
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(GetIsolate());
    // Create a new context.
    Local<Context> context =ContextWrapper::createContext3(GetIsolate());

    if (context.IsEmpty())
    {
        LOGI("Error creating context\n");
    }
    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);
    // Create a string containing the JavaScript source code.
    bool result = ExecuteString(context->GetIsolate(),
                                String::NewFromUtf8(context->GetIsolate(), jsSource,
                                                    NewStringType::kNormal).ToLocalChecked(),true, true);
    LOGI("JS Script Execute Result :%d", result);

    //C++调用js类start
    Local<String> js_data = String::NewFromUtf8(v8Helper::GetIsolate(), "Point", NewStringType::kInternalized)
            .ToLocalChecked();
//    PersistentBase::
    Local<Value> js_data_value = context -> Global() -> Get(js_data);
    String::Utf8Value str(js_data_value);
    LOGI("Point = %s \n",*str);
    bool  isFunction = js_data_value -> IsFunction();
    LOGI("Point is function %d",isFunction);
    bool  isObject = js_data_value -> IsObject();
    LOGI("Point is object %d",isObject);
    Local<Object> js_data_object = Local<Object>::Cast(js_data_value);
    // var object = new Point(1,2);
    const int argc = 2;
    Local<Value> argv[argc] = {};
    argv[0] = Int32::New(v8Helper::GetIsolate(),8);
    argv[1] = Int32::New(v8Helper::GetIsolate(),9);
    Local<Value> newObject = js_data_object -> CallAsConstructor(context, argc, argv).ToLocalChecked();
    LOGI("Point is function %d \n",newObject -> IsFunction());
    LOGI("Point is object %d",newObject -> IsObject());
    // object.show();
    Local<Object> obj = Local<Object>::Cast(newObject);
    Local<String> js_func_name = String::NewFromUtf8(v8Helper::GetIsolate(), "show", NewStringType::kInternalized).ToLocalChecked();
    Local<Value>  js_func_ref = obj->Get(js_func_name);
    Local<Function> js_func = Local<Function>::Cast(js_func_ref);
    js_func->Call(obj, 0, NULL) ;

    return result;
}

const char* v8Helper::loadScriptSource(JNIEnv *env, jobject assetManager, const char *name)
{
    LOGI("loadScriptSource.................");
    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);
    if(mgr==NULL)
    {
        LOGI(" %s","AAssetManager==NULL");
        return 0;
    }
    AAsset* asset = AAssetManager_open(mgr, name,AASSET_MODE_UNKNOWN);
    if(asset==NULL)
    {
        return 0;
    }
    off_t bufferSize = AAsset_getLength(asset);
    char *buffer=(char *)malloc(bufferSize+1);
    buffer[bufferSize]=0;
    int numBytesRead = AAsset_read(asset, buffer, bufferSize);
//    free(buffer);
    AAsset_close(asset);
    return buffer;
}









