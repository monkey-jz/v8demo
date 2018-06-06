
# v8demo



### js调用C++的方法：

```c++
v8Helper::Initialize();
Isolate::Scope isolate_scope(v8Helper::GetIsolate());
// Create a stack-allocated handle scope.
HandleScope handle_scope(v8Helper::GetIsolate());
// Create a new context.
Local<Context> context =creatContext(v8Helper::GetIsolate());
context_.Reset(v8Helper::GetIsolate(),context);
if (context.IsEmpty())
{
   LOGI("Error creating context\n");
}
// Enter the context for compiling and running the hello world script.
Context::Scope context_scope(context);
// Create a string containing the JavaScript source code.
bool result =   v8Helper::ExecuteString(context->GetIsolate(),
String::NewFromUtf8(context->GetIsolate(), jsSource,NewStringType::kNormal).ToLocalChecked(),true, true);
                                                               
LOGI("JS Script Execute Result :%d", result);

```
```c++
// Creates a new execution environment containing the built-in functions.
v8::Local<v8::Context> CreateContext(v8::Isolate* isolate) {

    // Create a template for the global object.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
    // Bind the global 'print' function to the C++ Print callback.
    global->Set(v8::String::NewFromUtf8(isolate, "print", v8::NewStringType::kNormal)
    v8::FunctionTemplate::New(isolate, print))
    // Bind the global 'read' function to the C++ Read callback.
    global->Set(v8::String::NewFromUtf8(isolate, "add", v8::NewStringType::kNormal).ToLocalChecked(),v8::FunctionTemplate::New(isolate, add));

    return Context::New(isolate,0,global);
    
}
```
```c++
void LocalCFunction::add(const FunctionCallbackInfo <Value> &args)
{
    int a = args[0]->Uint32Value();
    int b = args[1]->Uint32Value();
    args.GetReturnValue().Set(
            Integer::New(args.GetIsolate(), a+b));
    LOGI("add is called");

}
```
```c++
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
```
```JavaScript
var y = add(20, 30);
print(y);
      
```
不管是js调用C++的方法,还是C++调用js的方法，对这些方法的包装都应该是v8初始化完成后最先操作的。在 ``Local<Context> context =creatContext(v8Helper::GetIsolate());`` 之前的操作都是v8初始化常规代码我做了一些简单的封装，首先创建一个全局对象的模板然后绑定C++的方法到字符串上，js就可以用此字符串来调用绑定的方法。应该注意到这些绑定在执行js字符串之前就已经进行了。

### C++调用js方法：

```c++
v8Helper::Initialize();
Isolate::Scope isolate_scope(v8Helper::GetIsolate());
// Create a stack-allocated handle scope.
HandleScope handle_scope(v8Helper::GetIsolate());
// Create a new context.
Local<Context> context =creatContext(v8Helper::GetIsolate());
context_.Reset(v8Helper::GetIsolate(),context);
if (context.IsEmpty())
{
 LOGI("Error creating context\n");
}
// Enter the context for compiling and running the hello world script.
Context::Scope context_scope(context);
// Create a string containing the JavaScript source code.
bool result =   v8Helper::ExecuteString(context->GetIsolate(),
           String::NewFromUtf8(context->GetIsolate(), jsSource,
           NewStringType::kNormal).ToLocalChecked(),true, true);                                                              
LOGI("JS Script Execute Result :%d", result);
//调用js方法

Local<String> process_name =String::NewFromUtf8(v8Helper::GetIsolate(), "jsFunction", NewStringType::kNormal)
                    .ToLocalChecked();
Local<Value> process_val;
// If there is no Process function, or if it is not a function,
if (!context->Global()->Get(context, process_name).ToLocal(&process_val) ||!process_val->IsFunction()) {
   LOGI("initView is not a function\n");
}
// It is a function; cast it to a Function
Local<Function> process_fun = Local<Function>::Cast(process_val);
process_.Reset(v8Helper::GetIsolate(),process_fun);
          
```
```JavaScript

var x = [7, 8, 'jsarray', 7 * 8];
print(x);
function jsFunction(){
    var y = add(20, 30);
    print(y)
}
```

注意 ``context_.Reset(GetIsolate(), context);`` 这一句是用来保存当前上下文句柄的， ``process_.Reset(GetIsolate(), process_fun);`` 是用来保存在js找出来的方法的句柄的，这两句是我们在以后的任何时候都可以调用js方法的关键。中间的一些代码都很常规，就是找出js中的方法名然后转成方法。应该注意到找出js方法的操作是在脚本加载并且执行完之后进行的，这是因为如果在加载js脚本之前找js的方法是肯定找不到的。context_是Global<Context>类型，process_是Global<Function>类型，这两个全局类型的对象其实是用来保存当前的上下文环境和需要以后来执行的方法的句柄的。以后我们我们可以通过这两个句柄，进入到相应上下文环境中执行相应的方法。接下来看一下，我们是怎么在C++中调用在js中找出来的这个方法的：

```c++
// Create a handle scope to keep the temporary object references.
HandleScope handle_scope(v8Helper::GetIsolate());

v8::Local<v8::Context> context = v8::Local<v8::Context>::New(v8Helper::GetIsolate(), context_);

// Enter this processor's context so all the remaining operations
// take place there
Context::Scope context_scope(context);

// Set up an exception handler before calling the Process function
TryCatch try_catch(v8Helper::GetIsolate());

const int argc = 0;
Local<Value> argv[argc] = {};
v8::Local<v8::Function> process =v8::Local<v8::Function>::New(v8Helper::GetIsolate(), process_);
Local<Value> result;
if (!process->Call(context, context->Global(), argc, argv).ToLocal(&result)) {
String::Utf8Value error(v8Helper::GetIsolate(), try_catch.Exception());
        LOGI("call js function error:%s",*error);
}
      
```

首先是创建一栈区域来保存当前临时对象引用。红框内的方法就是把之前保存``context_`` ，和 ``process_`` 对象中的句柄拿出来，先进入相应的上下文，然后在对应的上下文环境中拿到对应的方法来执行。基本C++调用js方法都是这个套路，这里argc为0是调用无参的js方法，调用有参的js方法，我还没试不过按照process.cc中的套路也应该没什么问题。

##js调C++类:
这里有两种情况：

1. 在C++中创建类的对象，然后把对象作为参数传递给js,在js中访问对象的属性和方法。
2. 在js中创建对象，在JS中访问对象的属性和方法。
 
第一种情况：
这种情况下与调用C++普通方法的区别是js在调用C++方法的时候多了一个对象参数，由于这个参数是个C++类的对象，所以我们需要把这个C++对象封装成js对象。

```c++
// Create a handle scope to keep the temporary object references.
HandleScope handle_scope(v8Helper::GetIsolate());
v8::Local<v8::Context> context =v8::Local<v8::Context>::New(v8Helper::GetIsolate(), context_);
// Enter this processor's context so all the remaining operations
// take place there
Context::Scope context_scope(context);
Person person("SexMonkey",18);
Local<Object> person_object = WrapPerson(&person);
// Set up an exception handler before calling the Process function
TryCatch try_catch(v8Helper::GetIsolate());
const int argc = 1;
Local<Value> argv[argc] = {person_object};
v8::Local<v8::Function> process = v8::Local<v8::Function>::New(v8Helper::GetIsolate(), process_);
Local<Value> result;
if (!process->Call(context, context->Global(), argc, argv).ToLocal(&result)) {
  String::Utf8Value error(v8Helper::GetIsolate(), try_catch.Exception());
  LOGI("call js function error:%s",*error);
}
```

```c++

Local<Object> WrapPerson(Person *person) {
//Local scope for temporary handles.
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
    ```

```c++

    Local<ObjectTemplate> MakePersonTemplate(Isolate *isolate) {

    EscapableHandleScope handle_scope(isolate);

    Local<ObjectTemplate> result = ObjectTemplate::New(isolate);
    result->SetInternalFieldCount(1);

    // Add accessors for each of the fields of the request.
    result->SetAccessor(
            String::NewFromUtf8(isolate, "name", NewStringType::kInternalized)
                    .ToLocalChecked(),
           GetName);
    result->SetAccessor(
            String::NewFromUtf8(isolate, "age", NewStringType::kInternalized)
                    .ToLocalChecked(),
            GetAge);

    result->Set(String::NewFromUtf8(isolate, "setName", NewStringType::kNormal)
                        .ToLocalChecked(),
                FunctionTemplate::New(isolate, SetName));
    result->Set(String::NewFromUtf8(isolate, "setAge", NewStringType::kNormal)
                        .ToLocalChecked(),
                FunctionTemplate::New(isolate, SetAge));

    // Again, return the result through the current handle scope.
    return handle_scope.Escape(result);
    }

    ```

```c++

    void GetName(Local<String> name, const PropertyCallbackInfo<Value>& info) {
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
    ```
```c++
    void SetAge(const FunctionCallbackInfo <Value> &args)
    {
    LOGI("setAge is called");
    Person* person = UnwrapPerson(args.Holder());
    person -> setAge(args[0]->Uint32Value());

    }
    ```

```c++

    Person* UnwrapPerson(Local<Object> obj) {
    Local<External> field = Local<External>::Cast(obj->GetInternalField(0));
    void* ptr = field->Value();
    return static_cast<Person*>(ptr);
    }
    ```

```JavaScript

   function jsFunction(person){
       print(person.name,person.age);
       person.setName("test");
       person.setAge(18);
       print(person.name,person.age);
   }
    ```
仔细看一下其实套路都是一样的，只不过是多了几步针对C++对象封装的专有步骤而已。主要的步骤就是先创建一个对象模板为这个对象模板绑定暴露给js访问C++对象的属性和方法的接口，注意对访问属性和访问方法的封装是不一样的，虽然这些套路都是固定的但也要注意其中的区别。

以我传递的person对象为例，当我在js中用person.name来访问person的name属性的时候，v8实际上就会调用 ``GetName(Local<String> name, const PropertyCallbackInfo<Value>& info)`` 方法，通过回调的info参数来解包装，转成你传递的对像类型的指针后，再用这个指针来访问对象的成员方法getName()来获取name的值，然后再设置成返回值其实也就是 ``person.name`` 的值。访问方法也一样，比如在js中访问 ``person.setName("123")`` ，v8会调用 ``SetName(const FunctionCallbackInfo <Value> &args)`` ;也是先解包装转换成你传递的对像类型的指针后再用对象的成员方法 ``setName(*str(args.GetIsolate(),args[0]));`` 通过v8回调给C++的参数来改变对象的属性值。注意以上 ``person.name`` ， ``person.setName("123")`` ,name和setName都是你绑定对象模板时暴露给js接口的字符串，person也是你自己在js中使用的一个字符而已，你也可以用teacher，teacher.name。在模板创建完成后又经过了几个步骤主要是对刚才创建的模板与C++对象的关联。这些步骤完成后，一个C++对象就封装成为js对象了，然后把这个对象当做参数传给js，js就可以访问之前创建的模板上绑定的方法了。

第二种情况：
在js中创建C++的对象去访问对象的属性和方法，重点是对构造函数的绑定，绑定的时机与一般函数即全局函数一样，在js文件加载之前就可绑定，注意是在``creatContext(Isolate *isolate)`` 这个方法中进行绑定。

```c++

    Local<Context> creatContext(Isolate *isolate) {
    Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
    //Create the function template for the constructor, and point it to our constructor,
    Handle<FunctionTemplate> person_template = FunctionTemplate::New(v8Helper::GetIsolate(),PersonConstructor);
    //We can tell the scripts what type to report. In this case, just Person will do.
    person_template->SetClassName(String::NewFromUtf8(v8Helper::GetIsolate(), "Person", NewStringType::kNormal)
                                           .ToLocalChecked());
    //This template is the unique properties of this type, we can set
    //functions, getters, setters, and values directly on each new Person() object.
    Handle<ObjectTemplate> person = person_template -> InstanceTemplate();
    //Again, this is to store the c++ object for use in callbacks.
    person -> SetInternalFieldCount(1);
    person -> SetAccessor(
            String::NewFromUtf8(isolate, "name", NewStringType::kInternalized)
                    .ToLocalChecked(),
            GetName);
    person -> SetAccessor(
            String::NewFromUtf8(isolate, "age", NewStringType::kInternalized)
                    .ToLocalChecked(),
            GetAge);

    person -> Set(String::NewFromUtf8(isolate, "setName", NewStringType::kNormal)
                        .ToLocalChecked(),
                FunctionTemplate::New(isolate, SetName));
    person -> Set(String::NewFromUtf8(isolate, "setAge", NewStringType::kNormal)
                        .ToLocalChecked(),
                FunctionTemplate::New(isolate, SetAge));

    //Finally, we can tell the global scope that it now has a 'function' called Person,
    //and that this function returns the template object above.
    global -> Set(String::NewFromUtf8(isolate, "Person", NewStringType::kNormal)
                          .ToLocalChecked(),person_template);

    return Context::New(isolate,0,global);
    }

    ```

```c++

    void  PersonConstructor(const FunctionCallbackInfo <Value>& args){
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
```

```JavaScript

    var per = new Person("JIMI",20);
    print(per.name,per.age);
    per.setName("test");
    per.setAge(18);
    print(per.name,per.age);

 ```

在 ``creatContext(Isolate *isolate)`` 中先是为构造函数创建函数模板，并将其指向我们的构造函数，SetClassName是告诉js脚本C++对象的类型，就是来区分普通字符串和C++类的，InstanceTemplate()是获取C++实例模板，接下来就是为js中创建C++对象访问其属性和方法绑定的C++接口,与我们传递C++对象给js函数然后用对象访问属性和方法的绑定过程是一样的,最后一步是设置person(name，age)这个全局函数给js调用；
构造函数的绑定需要注意一下，回调参数args数组索引对应着是初始化对象时的属性顺序，拿到属性值后，用这些属性值在C++中创建一个类的对象，然后再把对象指针设置到索引为0的地方,v8会在js中C++对象调用其属性和方法时从这个地方查询到真正的C++对象。

### C++调js类:
C++调用调用js的类与C++调用js方法有些许类似，都是在脚本加载并运行之后进行的。看一下代码会发现调用过程有点复杂，但基本套路都是一样的。

```c++ 
    v8Helper::Initialize();
    Isolate::Scope isolate_scope(v8Helper::GetIsolate());
    // Create a stack-allocated handle scope.
    HandleScope handle_scope(v8Helper::GetIsolate());
    // Create a new context.
    Local<Context> context =creatTestContext(v8Helper::GetIsolate());
    context_.Reset(v8Helper::GetIsolate(),context);
    if (context.IsEmpty())
    {
        LOGI("Error creating context\n");
    }

    // Enter the context for compiling and running the hello world script.
    Context::Scope context_scope(context);
    // Create a string containing the JavaScript source code.
    bool result =   v8Helper::ExecuteString(context->GetIsolate(),
                                            String::NewFromUtf8(context->GetIsolate(), jsSource,NewStringType::kNormal).ToLocalChecked(),true, true);

    
    LOGI("JS Script Execute Result :%d", result);

    /*C++调用js类 start..*/
    Local<String> js_data = String::NewFromUtf8(v8Helper::GetIsolate(), "Point", NewStringType::kInternalized)
            .ToLocalChecked();
    Local<Value> js_data_value = context -> Global() -> Get(js_data);
    String::Utf8Value str(js_data_value);
    LOGI("Point = %s \n",*str);
    bool  isFunction = js_data_value -> IsFunction();
    LOGI("Point is function %d",isFunction);
    bool  isObject = js_data_value -> IsObject();
    LOGI("Point is object %d",isObject);
    Local<Object> js_data_object = Local<Object>::Cast(js_data_value);
    // var newObj = new Point(1,2);
    const int argc = 2;
    Local<Value> argv[argc] = {};
    argv[0] = Int32::New(v8Helper::GetIsolate(),7);
    argv[1] = Int32::New(v8Helper::GetIsolate(),8);
    Local<Value> newObject = js_data_object -> CallAsConstructor(context, argc, argv).ToLocalChecked();
    LOGI("Point is function %d \n",newObject -> IsFunction());
    LOGI("Point is object %d",newObject -> IsObject());
    // newObj.show();
    Local<Object> obj = Local<Object>::Cast(newObject);
    Local<String> js_func_name = String::NewFromUtf8(v8Helper::GetIsolate(), "show", NewStringType::kInternalized).ToLocalChecked();
    Local<Value>  js_func_ref = obj->Get(js_func_name);
    Local<Function> js_func = Local<Function>::Cast(js_func_ref);
    js_data_value = js_func->Call(obj, 0, NULL) ;
    
    String::Utf8Value str2(js_data_value);
    LOGI("Point = %s \n",*str2);

    //object.z;
    Local<String> js_pro_name = String::NewFromUtf8(v8Helper::GetIsolate(), "z", NewStringType::kInternalized).ToLocalChecked();
    Local<Value>  js_pro_ref = obj->Get(js_pro_name);
    String::Utf8Value pro(js_pro_ref);
    LOGI("js object prototype :%s",*pro);

    /*C++调用js类 end..*/

    //调用js方法
    Local<String> process_name =
            String::NewFromUtf8(v8Helper::GetIsolate(), "initView", NewStringType::kNormal)
                    .ToLocalChecked();
    Local<Value> process_val;
    // If there is no Process function, or if it is not a function,
    if (!context->Global()->Get(context, process_name).ToLocal(&process_val) ||
        !process_val->IsFunction()) {
        LOGI("initView is not a function\n");
    }
    // It is a function; cast it to a Function
    Local<Function> process_fun = Local<Function>::Cast(process_val);
    process_.Reset(v8Helper::GetIsolate(),process_fun);

        
    ```

```JavaScript

     function Point(x,y){
        this.x=x;
        this.y=y;
    }
    Point.prototype.show=function(){
       return '(x,y) = '+this.x+','+this.y;
    }

    Point.prototype.z = 1000;
 
    ```
首先将想要调用的js类的类名以字符串的形式转成v8能识别的字符串，然后``context -> Global() -> Get(js_data)`` , v8开始通过这个类名在js中找到相应的值,然后转通过相应的方法来先看看这个找出来的值是方法还是类，我们可以在脚本中看到这个值既是类又是方法,这其实是个构造方法,然后``Local<Object>::Cast(js_data_value);`` 将这个值转化成了js的类,接着``js_data_object -> CallAsConstructor(context, argc, argv).ToLocalChecked();`` ,是调用js类的构造方法并做一些属性的初始化操作,返回的就是js类的对象了,接下来就是用这个对象来调用js类的方法了,与之前C++调用js全局方法的方法是一样的。注意prototype是JavaScript类的一个关键字它可以指定类的属性，脚本中是把show()方法当做类的方法。可以用类的对象调用这个方法。


