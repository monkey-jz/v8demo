//
// Created by JerryZhu on 2018/6/5.
//

#include "ContextWrapper.h"
#include "LocalCFunction.h"

Local<Context> ContextWrapper::createContext1(Isolate *isolate)
{
    // Create a template for the global object.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    // Bind the global 'print' function to the C++ Print callback.
    global->Set(
            v8::String::NewFromUtf8(isolate, "print", v8::NewStringType::kNormal)
                    .ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, LocalCFunction::print));
    global->Set(
            v8::String::NewFromUtf8(isolate, "add", v8::NewStringType::kNormal)
                    .ToLocalChecked(),
            v8::FunctionTemplate::New(isolate, LocalCFunction::add));

    return Context::New(isolate,0,global);
}
Local<Context> ContextWrapper::createContext2(Isolate *isolate)
{
    // Create a template for the global object.
    OBJ_TEMPLATE(global,isolate);
    // Bind the global 'print' function to the C++ Print callback.
    EXPOSE(global,isolate,"print",LocalCFunction::print);
    EXPOSE(global,isolate,"add",LocalCFunction::add);

    //Create the function template for the constructor, and point it to our constructor,
    Local<FunctionTemplate> person_template = FunctionTemplate::New(isolate,LocalCFunction::PersonConstructor);
    //We can tell the scripts what type to report. In this case, just Person will do.
    person_template->SetClassName(String::NewFromUtf8(isolate, "Person", NewStringType::kNormal)
                                          .ToLocalChecked());
    //This template is the unique properties of this type, we can set
    //functions, getters, setters, and values directly on each new Person() object.
    Local<ObjectTemplate> person = person_template -> InstanceTemplate();
    //Again, this is to store the c++ object for use in callbacks.
    person -> SetInternalFieldCount(1);
    person -> SetAccessor(
            String::NewFromUtf8(isolate, "name", NewStringType::kInternalized)
                    .ToLocalChecked(),
            LocalCFunction::getName);
    person -> SetAccessor(
            String::NewFromUtf8(isolate, "age", NewStringType::kInternalized)
                    .ToLocalChecked(),
            LocalCFunction::getAge);

    person -> Set(String::NewFromUtf8(isolate, "setName", NewStringType::kNormal)
                          .ToLocalChecked(),
                  FunctionTemplate::New(isolate, LocalCFunction::setName));
    person -> Set(String::NewFromUtf8(isolate, "setAge", NewStringType::kNormal)
                          .ToLocalChecked(),
                  FunctionTemplate::New(isolate, LocalCFunction::setAge));

    //Finally, we can tell the global scope that it now has a 'function' called Person,
    //and that this function returns the template object above.
    global -> Set(String::NewFromUtf8(isolate, "Person", NewStringType::kNormal)
                          .ToLocalChecked(),person_template);

    return Context::New(isolate,0,global);
}
Local<Context> ContextWrapper::createContext3(Isolate *isolate)
{
    // Create a template for the global object.
    OBJ_TEMPLATE(global,isolate);
    // Bind the global 'print' function to the C++ Print callback.
    EXPOSE(global,isolate,"print",LocalCFunction::print);
    EXPOSE(global,isolate,"add",LocalCFunction::add);

    return Context::New(isolate,0,global);
}