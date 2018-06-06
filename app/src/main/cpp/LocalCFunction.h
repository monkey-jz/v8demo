//
// Created by JerryZhu on 2018/5/28.
//
#include "v8.h"
#include "log.h"
#ifndef V8DEMO_LOCALCFUNCTION_H
#define V8DEMO_LOCALCFUNCTION_H
using namespace v8;

class LocalCFunction {

public:
    static void add(const FunctionCallbackInfo<Value>& args);
    static void print(const FunctionCallbackInfo<Value>& args);
    static void PersonConstructor(const FunctionCallbackInfo<Value>& args);
    static void getName(Local<String> name, const PropertyCallbackInfo<Value>& info);
    static void getAge(Local<String> name, const PropertyCallbackInfo<Value>& info);
    static void setName(const FunctionCallbackInfo <Value> &args);
    static void setAge(const FunctionCallbackInfo <Value> &args);
    static std::string getCallbackInfo();

};


#endif //V8DEMO_LOCALCFUNCTION_H
