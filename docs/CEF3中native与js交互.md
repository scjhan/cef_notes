[TOC]

### 一、Cef中C++与JavaScript的三种交互方式

Cef提供了3种方式来实现C++与JavaScript的交互，分别是直接执行JavaScript代码、窗口绑定(Window Binding)和拓展(Extension)。其中窗口绑定和拓展还提供更高级的`V8Handler`的实现方式。

#### 1.1 直接执行JavaScript

通过`CefFrame::ExecuteJavaScript()`函数来实现，可以在C++的任意位置调用JavaScript代码。函数原型如下：

```c++
// Define at include/cef_frame.h
class CefFrame : public virtual CefBaseRefCounted {
public:
  // Execute a string of JavaScript code in this frame. The |script_url|
  // parameter is the URL where the script in question can be found, if any.
  // The renderer may request this URL to show the developer the source of the
  // error.  The |start_line| parameter is the base line number to use for error
  // reporting.
  ///
  /*--cef(optional_param=script_url)--*/
  virtual void ExecuteJavaScript(const CefString& code,
                                 const CefString& script_url,
                                 int start_line) = 0;
}
```

下面是一个简单的实现：

```c++
static void ExecuteJsCode(CefRefPtr<CefBrowser> browser) {
    bool open(false);
    if (open) {
      CefRefPtr<CefFrame> frame = browser->GetMainFrame();
      CefString js_code = "alert('ExecuteJavaScript works!');";
      // 执行js代码，将会弹出窗口
      frame->ExecuteJavaScript(js_code, frame->GetURL(), 0);
    }
  }
```

#### 1.2 窗口绑定(Window Binding) 

##### a. 窗口绑定的一般用法

窗口绑定通过用于允许获取frame的`window`对象的一个值。例如可以在C++代码中设置frame的`window`对象的的某个域`field`的值，然后在JavaScript代码中通过`window.field`来获取。

**窗口绑定的执行必须要在`CefRenderProcessHandler::OnContextCreated()`函数里面实现。**

下面是一个简单的实现

```c++
void MyRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  CefRefPtr<CefV8Value> object = context->GetGlobal();

  CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");

  // 这一步将js window对象的'myval'域设置为str的值
  object->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);
}
```

然后在JavaScript中可以这样子调用：

```javascript
<script language="JavaScript">
alert(window.myval); // My Value!
</script>
```

##### b. 窗口绑定高级用法

窗口绑定除了可以绑定基本数据类型之外，还可以绑定C++函数，这样就能实现JavaScript对C++的回调。这个用法是：创建一个JS函数对象，JS函数对象通过V8Handler绑定到C++函数。在`SetValue`时传递这个JS函数对象。

下面是一个简单的实现

```c++
// 实现CefV8Handler接口
class V8Handler : public CefV8Handler {
  public:
  virtual bool Execute(const CefString& name,
                       CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval,
                       CefString& exception) {
    // 要绑定的函数名称
    if (name == "myfunc") {
      retval = CefV8Value::CreateString("My Value!");
      return true;
    }
    return false;
  }
  IMPLEMENT_REFCOUNTING(V8Handler);
};

void MyRenderProcessHandler::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  CefRefPtr<CefV8Value> object = context->GetGlobal();

  CefRefPtr<CefV8Handler> handler(new V8Handler);
  CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", handler);
  
  // 这一步将js window对象'native_func'绑定到func对应的C++函数上
  object->SetValue("native_func", func, V8_PROPERTY_ATTRIBUTE_NONE);
}
```

然后在JavaScript中可以这样子调用：

```javascript
<script language="JavaScript">
alert(window.native_func()); // 执行native的native_func函数
</script>
```

#### 1.3 拓展(Extension)

拓展类似于窗口绑定，区别是frame加载context之后他就不能再修改。拓展使用`CefRegisterExtension`函数注册一段JavaScript代码到frame中，函数原型如下：

```c++
bool CefRegisterExtension(const CefString& extension_name,	// js通过这个名称来调用，格式"V8/name"
                          const CefString& javascript_code,	// js代码
                          CefRefPtr<CefV8Handler> handler);
```

**拓展的执行必须要在`CefRenderProcessHandler::OnWebKitInitialized()`函数里面实现。**

##### a. 拓展一般用法

类似于窗口绑定，可以通过注册一段JavaScript代码的形式设置一些JavaScript的变量，然后在JavaScript代码中访问到。

下面是一个简单的实现

```c++
void MyRenderProcessHandler::OnWebKitInitialized() {
  std::string extensionCode =
    "var test;"
    "if (!test)"
    "  test = {};"
    "(function() {"
    "  test.myval = 'My Value!';"
    "})();";

  // 注册这段代码，之后js可以通过test.xxx来访问设置的变量
  CefRegisterExtension("v8/test", extensionCode, NULL);
}
```

然后在JavaScript中可以这样子调用：

```javascript
<script language="JavaScript">
alert(test.myval); // "My Value!"
</script>
```

##### b. 拓展高级用法

类似于窗口绑定，拓展也可以使用V8Handler在所注册的代码中绑定C++的函数，实现JavaScript对C++的回调。

下面是一个简单的实现

```c++
void MyRenderProcessHandler::OnWebKitInitialized() {
  std::string extensionCode =
    "var test;"
    "if (!test)"
    "  test = {};"
    "(function() {"
    "  test.myfunc = function() {"
    "    native function myfunc();"	// 表示返回的是一个名为myfunc的native函数，V8Handler中获取
    "    return myfunc();"
    "  };"
    "})();";

  // v8handler.Execute函数实现了myfunc，执行拓展js代码时通过Handler找到这个函数
  CefRefPtr<CefV8Handler> handler = new MyV8Handler();
  // 注册这段js代码并绑定到Handler中。之后js可以通过test.xxx来获取这段拓展代码定义的东西
  CefRegisterExtension("v8/test", extensionCode, handler);
}
```

然后在JavaScript中可以这样子调用：

```javascript
<script language="JavaScript">
alert(test.myfunc()); // 执行拓展代码定义的myfunc，实际绑定到Handler中的native function myfunc()
</script>
```

### 二、C++回调JavaScript

前面说的三种交互方式其主导都是C++代码，也就是JavaScript回调C++的代码。实际上，也可以实现C++回调JavaScript的代码。实现方式是：

- JavaScript先回调C++代码，通过参数的形式传递一个JavaScript的函数进来，也即设置JavaScript回调
- C++代码中将这个JavaScript的回调函数保存起来，注意这个时候还必须将对应的context保存起来
- 在适当的时机C++代码调用这个JavaScript回调

下面是一个简单的实现

```c++
static CefRefPtr<CefV8Context> g_cb_context;
static CefRefPtr<CefV8Value> g_callback;

class JSBindingV8Handler : public CefV8Handler {
public:
  virtual bool Execute(const CefString& name,
                       CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval,
                       CefString& exception) {
    if (name == "register" && arguments.size() == 1 && arguments[0]->IsFunction()) {
      g_cb_context = CefV8Context::GetCurrentContext();
      g_callback = arguments[0];
      return true;
    }
    if (name == "runcb" && g_callback.get()) {
      CefV8ValueList arguments;
      g_callback->ExecuteFunctionWithContext(g_cb_context, g_callback, arguments);
    }
    return false;
private:
    IMPLEMENT_REFCOUNTING(JSBindingV8Handler);
}
  
static void RegisterJsCallback(CefRefPtr<CefV8Context> context) {
  CefRefPtr<CefV8Value> obj = context->GetGlobal();
  CefRefPtr<JSBindingV8Handler> handler(new JSBindingV8Handler);
  obj->SetValue("register", 
                CefV8Value::CreateFunction("register", handler), 
                V8_PROPERTY_ATTRIBUTE_NONE);
  obj->SetValue("runcb",
                CefV8Value::CreateFunction("runcb", handler),
                V8_PROPERTY_ATTRIBUTE_NONE);
}
  
void MyRenderProcessHandler::OnContextCreated(
   	CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
  RegisterJsCallback(context);
}
```

然后在JavaScript中可以这样子调用：

```html
<script language="JavaScript">
function JsCallback() { alert("JsCallback called"); }
function RegisterJsCallback() { window.register(JsCallback); }
function RunJsCall() { window.runcb(); }
</script>

<input type="button" onclick="RegisterJsCallback();" value="RegisterJsCallback">
<input type="button" onclick="RunJsCall();" value="RunJsCall">
```

上述js代码的意思是

- 提供一个注册按钮，点击则将`JsCallback`作为参数调用`window.register`传递到C++代码中
- 提供一个允许按钮，点击则C++代码回调先前注册的JavaScript回调函数，这个是用来测试的

### 三、异步Javascript绑定

#### 3.1 通用消息路由

Javascript运行在renderer进程，C++运行在browser进程，但是在实际工作中，JS绑定经常是和browser进行通信。CEF在两者之间提供了一个异步消息路由的通用实现。应用程序通过标准CEF C++回调(`OnBeforeBrowse`, `OnProcessMessageRecieved`, `OnContextCreated`等)传递数据来与router交互。renderer这边的router支持一般的Javascript回调注册、执行，browser这边的router通过一个或多个的应用程序提供的Handler实例来支持特定的应用程序逻辑。

##### a. render端路由

render端路由封装为`CefMessageRouterRendererSide`，定义在`inlcude/cef_message_router.h`中。

一般使用方式是：

1. render进程实例设置一个`CefMessageRouterRendererSide`类成员`render_side_router_`。

2. render进程初始化时，初始化`render_side_router_`。

   ```c++
   // config默认回调名如下，也可以自定义
   // config.js_query_function = "cefQuery";
   // config.js_cancel_function = "cefQueryCancel";
   CefMessageRouterConfig config;
   render_side_router_ = CefMessageRouterRendererSide::Create(config);
   ```

3. 在renderer进程的接口中调用`CefMessageRouterRendererSide`的同名方法：

   ```c++
   render_side_router_->OnContextCreated(...);
   render_side_router_->OnContextReleased(...);
   render_side_router_->OnProcessMessageReceived(...);
   // 等等
   ```

##### b. browser端路由

browser端路由封装为`CefMessageRouterBrowserSide`，定义在`inlcude/cef_message_router.h`中。

一般使用方式是：

1. **browser_client**实例设置一个`CefMessageRouterBrowserSide`类成员`browser_side_router_`。

2. browser_client初始化时初始化`browser_side_router_`，并为其设置一个自定义的handler。

   ```c++
   // 重写Handler的自定义操作
   class RouterHandler : public CefMessageRouterBrowserSide::Handler {
    public:
     virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          int64 query_id,
                          const CefString& request,
                          bool persistent,
                          CefRefPtr<Callback> callback) OVERRIDE {
       // do something
     }
     virtual void OnQueryCanceled(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  int64 query_id) OVERRIDE {
       // do something
     }
   };

   // on initialization
   CefMessageRouterConfig config;
   browser_side_router_ = CefMessageRouterRendererSide::Create(config);
   // 一般将Handler也作为一个类成员，即：
   // RouterHandler router_handler_;
   browser_side_router_->AddHandler(&router_handler_, true);
   ```

3. 在browser_client的接口中调用`CefMessageRouterBrowserSide`的同名方法：

   ```c++
   OnBeforeClose(...);
   OnRenderProcessTerminated(...);
   OnBeforeBrowse(...);
   OnProcessMessageReceived(...);
   // 等等
   ```

##### c. Javascript回调注册

```html
<script language="JavaScript">
  function sendMessage() {
    window.cefQuery({
      request: /* a string value */,
      onSuccess: function(response) { /* do something */ },
      onFailure: function(error_code, error_message) { /* do something */ }
    });
  }
</script>
```

#### 3.2 自定义实现

自定义的异步JS绑定实现过程类似于《二、C++回调JavaScript》，主要步骤如下：

1. render进程的JavaScript绑定传递一个回调函数。
2. render进程保存回调函数的引用。
3. render进程发送异步IPC消息给browser进程，请求执行该动作。
4. browser进程接受IPC消息并执行动作。
5. 动作完成之后browser进程发送一个异步IPC消息给render进程，并返回结果。
6. render进程接收到IPC消息并执行回调函数，返回结果。
7. 在`CefRenderProcessHandler::OnContextReleased()`中释放所有context相关的的V8引用。

```c++
void MyHandler::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) {
  // Remove any JavaScript callbacks registered for the context that has been released.
  if (!callback_map_.empty()) {
    CallbackMap::iterator it = callback_map_.begin();
    for (; it != callback_map_.end();) {
      if (it->second.first->IsSame(context))
        callback_map_.erase(it++);
      else
        ++it;
    }
  }
}
```

