这个wiki页用来说明如何在应用程序中集成JavaScript。

[TOC]

# 介绍

Chromium和CEF use the [V8 JavaScript 引擎](http://code.google.com/p/v8/) 作为内部的 JavaScript (JS)实现。browser里面每一个frame拥有属于自己JS contex，为JS代码的执行提供一个有限和安全的环境(更多信息查看"使用Contexs"段落)。CEF为客户端提供可以集成的大量JS features。

CEF3 Blink(Webkit)和JS代码执行运行在独立的渲染进程中、渲染进程的的主线程标识符是TID_RENDERER，所有的V8执行都必须在这个线程。JS执行相关的回调通过CefRenderProcessHandler 接口暴露，这个接口可以在一个新的渲染进程初始化时通过CefApp::GetRenderProcessHandler() 获得。

浏览器进程和渲染进程的JS APIs必须被设计为异步的回调。更多相关信息查看[一般用法](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage.md)文档的"异步JavaScript 绑定"段落。

# 执行JavaScript

客户端程序执行JavaScript最简单方式是使用CefFrame::ExecuteJavaScript() 函数。这个函数在浏览器进程和渲染进程都是有效的并且可以在JS context之外安全的使用。

```c++
CefRefPtr<CefBrowser> browser = ...;
CefRefPtr<CefFrame> frame = browser->GetMainFrame();
frame->ExecuteJavaScript("alert('ExecuteJavaScript works!');",
    frame->GetURL(), 0);
```

上述例子将会在浏览器的main frame执行 `alert('ExecuteJavaScript works!');` 。

ExecuteJavaScript() 函数可以在JS context中用于与函数和变量交互、如果想要从JS中返回值到客户端程序可以使用窗口绑定或者拓展。

# 窗口绑定

窗口绑定允许客户端程序获得frame的`window`对象的一个值。窗口绑定通过使用CefRenderProcessHandler::OnContextCreated() 函数实现。

```c++
void MyRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  // Retrieve the context's window object.
  CefRefPtr<CefV8Value> object = context->GetGlobal();

  // Create a new V8 string value. See the "Basic JS Types" section below.
  CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");

  // Add the string to the window object as "window.myval". See the "JS Objects" section below.
  object->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);
}
```

之后frame中的JavaScript就能和窗口绑定进行交互。

```javascript
<script language="JavaScript">
alert(window.myval); // Shows an alert box with "My Value!"
</script>
```

每次frame重新加载时窗口绑定也会重新加载，以便于客户端应用在必要时改变绑定。例如，可以通过修改绑定到该窗口的窗口对象的值来给不同的frames访问客户端应用程序中的不同功能。

# 拓展

拓展类似与窗口绑定，区别是它在每一个frames的contex载入之后不可以修改。拓展被加载时DOM还未存在，这个时候尝试读取DOM将会导致crash。拓展通过CefRegisterExtension()函数进行注册，这个函数必须在CefRenderProcessHandler::OnWebKitInitialized() 中调用。

```c++
void MyRenderProcessHandler::OnWebKitInitialized() {
  // Define the extension contents.
  std::string extensionCode =
    "var test;"
    "if (!test)"
    "  test = {};"
    "(function() {"
    "  test.myval = 'My Value!';"
    "})();";

  // Register the extension.
  CefRegisterExtension("v8/test", extensionCode, NULL);
}
```

`extensionCode` 代表的字符串可以是任意有效的JS代码，frame中的JS可以与拓展代码交互。

```javascript
<script language="JavaScript">
alert(test.myval); // Shows an alert box with "My Value!"
</script>
```

# JS基础类型

CEF支持创建undefined, null, bool, int, double, date和string这些JS基本数据类型，这些类型通过一个名为CefV8Value::Create*()的静态函数创建。例如，使用CreateString创建一个JS string。 

```c++
CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");
```

基本数据类型可以在任意时刻被创建，而无需在指定的contex之前。(更多信息查"使用Context"段落)

使用Is*()可以函数测试一个值的类型。T

```c++
CefRefPtr<CefV8Value> val = ...;
if (val.IsString()) {
  // The value is a string.
}
```

使用Get*Value() 可以获取一个值

```c++c++
CefString strVal = val.GetStringValue();
```

# JS 数组

使用静态函数CefV8Value::CreateArray()创建一个数组，函数接受一个长度参数。数组只能在contex里面被创建和使用(更多信息查"使用Context"段落)

```c++
// Create an array that can contain two values.
CefRefPtr<CefV8Value> arr = CefV8Value::CreateArray(2);
```

使用SetValue()函数来为数组赋值，函数的第一个参数为一个index

```c++
// Add two values to the array.
arr->SetValue(0, CefV8Value::CreateString("My First String!"));
arr->SetValue(1, CefV8Value::CreateString("My Second String!"));
```

使用IsArray() 函数判断一个CefV8Value 是否是数组类型；使用GetArrayLength() 获取数组类型；使用GetValue()获取数组的值，第一个参数为index

# JS 对象

使用静态函数CefV8Value::CreateObject() 创建一个JS对象，函数包含一个可选的CefV8Accessor 参数。JS对象只能在contex里面被创建和使用(更多信息查"使用Context"段落)

```c++
CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(NULL);
```

使用SetValue()函数来获取一个对象的值，第一个参数是key

```c++
obj->SetValue("myval", CefV8Value::CreateString("My String!"));
```

## 对象存取器

对象可以拥有一个相关的CefV8Accessor，需要实现getting和setting

```c++
CefRefPtr<CefV8Accessor> accessor = …;
CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(accessor);
```

CefV8Accessor 的实现由客户端应用提供

```c++
class MyV8Accessor : public CefV8Accessor {
public:
  MyV8Accessor() {}

  virtual bool Get(const CefString& name,
                   const CefRefPtr<CefV8Value> object,
                   CefRefPtr<CefV8Value>& retval,
                   CefString& exception) OVERRIDE {
    if (name == "myval") {
      // Return the value.
      retval = CefV8Value::CreateString(myval_);
      return true;
    }

    // Value does not exist.
    return false;
  }

  virtual bool Set(const CefString& name,
                   const CefRefPtr<CefV8Value> object,
                   const CefRefPtr<CefV8Value> value,
                   CefString& exception) OVERRIDE {
    if (name == "myval") {
      if (value.IsString()) {
        // Store the value.
        myval_ = value.GetStringValue();
      } else {
        // Throw an exception.
        exception = "Invalid value type";
      }
      return true;
    }

    // Value does not exist.
    return false;
  }

  // Variable used for storing the value.
  CefString myval_;

  // Provide the reference counting implementation for this class.
  IMPLEMENT_REFCOUNTING(MyV8Accessor);
};
```

使用SetValue()函数的重载版本传递AccessControl和PropertyAttribute参数来设置一个accessor

```c++
obj->SetValue("myval", V8_ACCESS_CONTROL_DEFAULT, 
    V8_PROPERTY_ATTRIBUTE_NONE);
```

# JS 函数

CEF支持在native实现中创建一个JS函数。JS函数使用静态函数CefV8Value::CreateFunction() 创建，函数接受一个名称和CefV8Handler参数。函数只能在contex里面被创建和使用(更多信息查"使用Context"段落)

```c++
CefRefPtr<CefV8Handler> handler = …;
CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", handler);
```

CefV8Handler的实现由客户端应用提供

```c++
class MyV8Handler : public CefV8Handler {
public:
  MyV8Handler() {}

  virtual bool Execute(const CefString& name,
                       CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval,
                       CefString& exception) OVERRIDE {
    if (name == "myfunc") {
      // Return my string value.
      retval = CefV8Value::CreateString("My Value!");
      return true;
    }

    // Function does not exist.
    return false;
  }

  // Provide the reference counting implementation for this class.
  IMPLEMENT_REFCOUNTING(MyV8Handler);
};
```

## 窗口绑定中的函数

函数可以用与创建一个复杂的窗口绑定

```c++
void MyRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  // Retrieve the context's window object.
  CefRefPtr<CefV8Value> object = context->GetGlobal();

  // Create an instance of my CefV8Handler object.
  CefRefPtr<CefV8Handler> handler = new MyV8Handler();

  // Create the "myfunc" function.
  CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", handler);

  // Add the "myfunc" function to the "window" object.
  object->SetValue("myfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);
}
```

```js
<script language="JavaScript">
alert(window.myfunc()); // Shows an alert box with "My Value!"
</script>
```

## 拓展中的函数

使用函数可以创建一个复杂的拓展。注意使用拓展时native函数需要前置声明

```c++
void MyRenderProcessHandler::OnWebKitInitialized() {
  // Define the extension contents.
  std::string extensionCode =
    "var test;"
    "if (!test)"
    "  test = {};"
    "(function() {"
    "  test.myfunc = function() {"
    "    native function myfunc();"
    "    return myfunc();"
    "  };"
    "})();";

  // Create an instance of my CefV8Handler object.
  CefRefPtr<CefV8Handler> handler = new MyV8Handler();

  // Register the extension.
  CefRegisterExtension("v8/test", extensionCode, handler);
}
```

```javascript
<script language="JavaScript">
alert(test.myfunc()); // Shows an alert box with "My Value!"
</script>
```

# 使用Contexts

浏览器中的每一个frame都有自己的V8 context，context定义了frame中所有变量、对象和函数的范围。如果当前代码位置具有CefV8Handler, CefV8Accessor或者OnContextCreated()/OnContextReleased()回调在调用栈的更高处，则V8将会存在于context中。

OnContextCreated()和OnContextReleased()方法定义了frame相关联的V8 context的完整生命周期，在使用这两个方法时应当心并遵循以下两条规则：

1. 不要再context调用了OnContextReleased()之后去保存它的引用。
2. 所有V8对象的生命周期是未指定的(GC决定)。在自定义的内部实现对象内维持V8对象引用时应当心。在很多情况下，使用代理对象可能会更好，让应用程序与V8上下文相关联，并且可以在调用context的OnContextReleased()时“断开连接”（允许内部实现对象被释放）

如果V8不在context中，则需要获取并保存context的引用。可以使用CefV8Context的两个静态函数，GetCurrentContext() 返回frame当前执行JS的context；GetEnteredContext()返回frame开始执行JS时的context。例如，frame1在frame2中调用函数，则当前context为frame2，入口context为frame1。

数组，对象和函数只能在V8存在于context中时函数执行情况下被创建和修改。如果V8不存在于context综合馆，则应用程序需要通过调用Enter()来进入context，通过调用Exit()来退出context，这两个方法只能用于:

1. 创建V8对象时函数或者数组在存在的context之外。例如，在native菜单会调用创建一个JS对象。
2. 在当前context创建一个V8对象、函数或者数组时。例如，从frame1开始的调用需要修改frame2的context。

# 执行函数

native代码通过ExecuteFunction() 和ExecuteFunctionWithContext() 函数来执行JS函数。ExecuteFunction() 函数只能是"使用Contexts"段落描述的V8已经在context中的情况时调用。ExecuteFunctionWithContext() 允许客户端引用在执行JS函数时指定要进入的context

## 使用 JS 回调

当在native代码中注册一个JS函数回调时需要保存当前context和JS函数的引用。这个实现示例如下：

1. 在OnJSBinding()中创建一个叫做"register" 函数

```c++
void MyRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  // Retrieve the context's window object.
  CefRefPtr<CefV8Value> object = context->GetGlobal();

  CefRefPtr<CefV8Handler> handler = new MyV8Handler(this);
  object->SetValue("register",
                   CefV8Value::CreateFunction("register", handler),
                   V8_PROPERTY_ATTRIBUTE_NONE);
}
```

2. 在MyV8Handler::Execute()实现 "register" function函数，并保存context和function的引用 

```c++
bool MyV8Handler::Execute(const CefString& name,
                          CefRefPtr<CefV8Value> object,
                          const CefV8ValueList& arguments,
                          CefRefPtr<CefV8Value>& retval,
                          CefString& exception) {
  if (name == "register") {
    if (arguments.size() == 1 && arguments[0]->IsFunction()) {
      callback_func_ = arguments[0];
      callback_context_ = CefV8Context::GetCurrentContext();
      return true;
    }
  }

  return false;
}
```

3. 通过 JavaScript注册JS回调

```javascript
<script language="JavaScript">
function myFunc() {
  // do something in JS.
}
window.register(myFunc);
</script>
```

4. 执行 JS回调

```c++
CefV8ValueList args;
CefRefPtr<CefV8Value> retval;
CefRefPtr<CefV8Exception> exception;
if (callback_func_->ExecuteFunctionWithContext(callback_context_, NULL, args, retval, exception, false)) {
  if (exception.get()) {
    // Execution threw an exception.
  } else {
    // Execution succeeded.
  }
}
```

更多信息查看一般用法文档的 [异步JavaScript 绑定](https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-asynchronous-javascript-bindings) 段落。

## 重新抛出异常

如果在执行CefV8Value::ExecuteFunction*()之前调用了CefV8Value::SetRethrowExceptions(true)，则任何由V8在函数执行过程中产生的异常都会被立即重新抛出。如果异常被重新抛出，native需要立即返回。异常只有在JS调用处于更高的调用栈位置时才能被重新抛出。例如，以下的两个调用栈（JS表示JS函数，EF表示native的ExecuteFunction）：

Stack 1: JS1 -> EF1 -> JS2 -> EF2

Stack 2: Native Menu -> EF1 -> JS2 -> EF2

stack1在EF1和EF2均可以重新抛出异常，stack2只有EF2可以。

这个可以通过native代码中的两种call sites实现：

1. 只在V8 handler中调用。调用栈stack1包含EF1和EF2，stack2包含EF2，重新抛出将一直为真
2. 只在native中调用。调用栈stack2包含EF1，重新抛出将一直为假

当重新抛出异常时要非常当心。不正确的用法（例如，异常被重新抛出之后立即调用ExecuteFunction() ）将会导致应用程序crash或者出现非常难以调试的故障。