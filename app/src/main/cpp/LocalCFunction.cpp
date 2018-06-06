//
// Created by JerryZhu on 2018/5/28.
//


#include <string>
#include "LocalCFunction.h"
#include "v8Helper.h"
#include "Person.h"

Person* UnwrapPerson(Local<Object> obj);

void LocalCFunction::add(const FunctionCallbackInfo <Value> &args)
{
    int a = args[0]->Uint32Value();
    int b = args[1]->Uint32Value();
    args.GetReturnValue().Set(
            Integer::New(args.GetIsolate(), a+b));
    LOGI("add is called");

}


std::string callbackInfo;

void LocalCFunction::print(const FunctionCallbackInfo <Value> &args)
{
    callbackInfo ="";
    bool first = true;
    for (int i = 0; i < args.Length(); i++) {
        HandleScope handle_scope(args.GetIsolate());
        if (first) {
            first = false;
        } else {
            printf(" 1111");
        }
        String::Utf8Value str(args.GetIsolate(),args[i]);
        LOGI("print is called : %s",*str);
        std::string result(*str);
        callbackInfo += (result + ", ");
    }

    fflush(stdout);
}
void LocalCFunction::PersonConstructor(const FunctionCallbackInfo<Value> &args)
{
    LOGI("PersonConstructor is called");
    if (!args.IsConstructCall())
    {
        LOGI("args is not PersonConstructor call");
    }
    Handle<Object> object = args.This();
    HandleScope handle_scope(v8Helper::GetIsolate());
    String::Utf8Value str(args.GetIsolate(),args[0]);
    std::string name = *str;
    int age = args[1] -> Uint32Value();
    Person *person =  new Person(name,age);
    //Note that this index 0 is the internal field we created in the template!
    object -> SetInternalField(0,v8::External::New(v8Helper::GetIsolate(),person));

}

Person* UnwrapPerson(Local<Object> obj) {
    Local<External> field = Local<External>::Cast(obj->GetInternalField(0));
    void* ptr = field->Value();
    return static_cast<Person*>(ptr);
}

void LocalCFunction::getName(Local<String> name, const PropertyCallbackInfo<Value> &info)
{
    LOGI("getName is called ...");
    // Extract the C++ request object from the JavaScript wrapper.
    Person* person = UnwrapPerson(info.Holder());

    // Fetch the path.
    const std::string& cName = person -> getName();

    // Wrap the result in a JavaScript string and return it.
    info.GetReturnValue().Set(
            String::NewFromUtf8(info.GetIsolate(), cName.c_str(),
                                NewStringType::kNormal,
                                static_cast<int>(cName.length())).ToLocalChecked());
}
void LocalCFunction::setName(const FunctionCallbackInfo<Value> &args)
{
    LOGI("setName is called ...");
    Person* person = UnwrapPerson(args.Holder());
    String::Utf8Value str(args.GetIsolate(),args[0]);
    // Fetch the path.
    person ->setName(*str);
}
void LocalCFunction::getAge(Local<String> name, const PropertyCallbackInfo<Value> &info)
{
    LOGI("getAge is called ...");
    // Extract the C++ request object from the JavaScript wrapper.
    Person* person = UnwrapPerson(info.Holder());

    // Fetch the path.
    const int cAge = person -> getAge();

    // Wrap the result in a JavaScript int and return it.
    info.GetReturnValue().Set(
            Integer::New(info.GetIsolate(), cAge));

}
void LocalCFunction::setAge(const FunctionCallbackInfo<Value> &args)
{
    LOGI("setAge is called");
    Person* person = UnwrapPerson(args.Holder());
    person -> setAge(args[0]->Uint32Value());
}

std::string LocalCFunction::getCallbackInfo()
{
    return callbackInfo;
}
