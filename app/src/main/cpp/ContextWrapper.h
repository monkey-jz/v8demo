//
// Created by JerryZhu on 2018/6/5.
//
#include "include/v8.h"

#ifndef V8DEMO_CONTEXTWRAPPER_H
#define V8DEMO_CONTEXTWRAPPER_H

#define OBJ_TEMPLATE(obj,isolate) Local<ObjectTemplate> obj = ObjectTemplate::New(isolate)
#define EXPOSE(obj,isolate, jsname, func) obj->Set(String::NewFromUtf8(isolate, jsname, NewStringType::kNormal).ToLocalChecked(),FunctionTemplate::New(isolate, func));

using namespace v8;

class ContextWrapper {

public:
    static Local<Context>  createContext1(Isolate *isolate);
    static Local<Context>  createContext2(Isolate *isolate);
    static Local<Context>  createContext3(Isolate *isolate);

};


#endif //V8DEMO_CONTEXTWRAPPER_H
