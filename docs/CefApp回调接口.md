[TOC]

### 一、CefApp回调接口

#### 1. CefApp概念

`CefApp`对象用来传递到`CefInitialize()`和`CefExecuteProcess()`中，用于允许应用程序定制全局，如资源加载，代理等功能。有些功能是由所有进程共享，有些必须实现浏览器的过程中，有些必须在渲染过程中执行。

`CefApp`提供多个回调用于定制全局的操作，其中有两个比较重要的接口：

```c++
  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler();
  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler();
```

通过上面两个接口可以获取到browser进程或者是render进程的handler，从而执行两个进程的一些操作。

CEF程序会被多次启动并传递不同的参数，以启动不同的进程，如果在不同的进程中需要定制相关的操作，可以让CefApp分别继承`CefBrowserProcessHandler`和`CefRenderProcessHandler`然后重写两个handler的相关事件回调实现定制操作。程序启动时根据不同的启动参数创建不同的`CefApp`对象：

```c++
class BrowserApp : public CefApp, public CefBrowserProcessHandler {
public:
  // CefApp methods:
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {
    return this;
  }
  // CefBrowserProcessHandler methods:
  void OnContextInitialized() OVERRIDE;
}

class RendererApp : public CefApp, public CefRenderProcessHandler {
public:
  // CefApp methods:
  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {
    return this;
  }
  // CefRenderProcessHandler methods:
  virtual void OnWebKitInitialized() OVERRIDE;
}

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  CefMainArgs main_args(hInstance);
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());
  
  CefRefPtr<CefApp> app;
  if (!command_line->HasSwitch("type"))
    app = new BrowserApp();		// 创建browser进程实例
  else if (command_line->GetSwitchValue(kProcessType) == "renender")
    app = new RendererApp();	// 创建render进程实例
  else
    app = new OtherApp();		// 创建其他进程实例
  // ...其他操作
}
```

#### 2. CefApp接口列表

`CefApp`提供了一系列接口用于定制不同的操作：

```c++
class CefApp : public virtual CefBaseRefCounted {
 public:
  // 可定制命令行参数传递给CEF和chromium之前的一些操作。
  // 对于browser进程来说|process_type|为空
  virtual void OnBeforeCommandLineProcessing(
      const CefString& process_type,
      CefRefPtr<CefCommandLine> command_line) {}

  // 可用于注册一些自定义的scheme，如myscheme://domain，这个操作一般搭配
  // CefSchemeHandleFactory操作。
  // 这个接口将被不同进程的主线程调用，所以需要保证每个进程注册的scheme都是一样的
  virtual void OnRegisterCustomSchemes(
      CefRawPtr<CefSchemeRegistrar> registrar) {}

  // 返回资源包事件的handler。如果CefSettings.pack_loading_disabled为true则必须返回
  // handler。否则资源将会从pack文件中加载。
  // 这个接口会被Borwser进程和render进程的多线程中调用
  virtual CefRefPtr<CefResourceBundleHandler> GetResourceBundleHandler() {
    return NULL;
  }

  // 返回browser进程的handler。在browser进程的多线程中被调用
  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() {
    return NULL;
  }

  // 返回render进程的handler。在render进程的!!主线程!!中被调用
  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() {
    return NULL;
  }
};
```

### 二、 CefApp回调事件

#### 1. CefResourceBundleHandler 

`CefResourceBundleHandler`定义在`include/cef_resource_bundle_handler.h`。用于实现自定义资源包接口。包括3种事件：

```c++
class CefResourceBundleHandler : public virtual CefBaseRefCounted {
 public:
  // 用于获取一个通过|string_id|的本地化转换值。
  // |stirng_id|的有效值定义在include/cef_pack_strings.h中
  // 实现自定义转换则设置|string|的值并返回true；采用默认实现则返回false
  virtual bool GetLocalizedString(int string_id, CefString& string) = 0;

  // 通过指定的|resource_id|或者资源数据。提供资源数据则设置|data|和|data_size|为相应
  // 的值，并返回true；采用默认实现返回false。
  // |resource_id|的有效值定义在include/cef_pack_resources.h中
  virtual bool GetDataResource(int resource_id,
                               void*& data,
                               size_t& data_size) = 0;

  // 通过指定的|resource_id|获取与|scale_factor|最接近的比例因子。
  virtual bool GetDataResourceForScale(int resource_id,
                                       ScaleFactor scale_factor,
                                       void*& data,
                                       size_t& data_size) = 0;
};
```

#### 2. CefBrowserProcessHandler 

`CefBrowserProcessHandler`定义在`include/cef_browser_process_handler.h`中，提供了browser进程的事件回调接口，主要包括5种事件：

```c++
class CefBrowserProcessHandler : public virtual CefBaseRefCounted {
 public:
  // !! UI Thread中调用 !!
  // 当CEF Contex被初始化(CefInitialize)完毕之后立即调用
  virtual void OnContextInitialized() {}

  // 当子进程被启动时调用。
  // 当启动render进程时需要在!! UI Thread !!中调用
  // 启动一个GPU或者插件进程时需要在!! IO Thread !!中调用
  // 通过次接口可以在子进程被创建之前修改命令行参数
  virtual void OnBeforeChildProcessLaunch(
      CefRefPtr<CefCommandLine> command_line) {}

  // !! IO Thread !!中调用
  // 当新的渲染进程的主线程被创建时调用，通过这个接口可以为渲染进程的
  //  CefRenderProcessHandler::OnRenderThreadCreated()接口传递指定的额外的信息
  virtual void OnRenderProcessThreadCreated(
      CefRefPtr<CefListValue> extra_info) {}

  // !! UI Thread !!中调用
  // Linux平台下返回一个打印handler
  virtual CefRefPtr<CefPrintHandler> GetPrintHandler() { return NULL; }

  // 可以在任何线程调用
  // 当work在browser进程主线程被安排时调用。此回调与CefSettings.external_message_pump
  // 和CefDoMessageLoopWork（）一起使用，CEF消息循环必须集成到现有应用程序消息循环中.
  // delay_ms < 0则调用应该尽快被执行
  virtual void OnScheduleMessagePumpWork(int64 delay_ms) {}
};
```

#### 3. CefRenderProcessHandler 

`CefRenderProcessHandler`定义在`include/cef_render_process_handler.h`中，提供了render进程的事件回调接口，没有特别指定的话，**所有事件都应该在render主线程(`TID_RENDERER`)中调用**。主要包括11种事件：

```c++
class CefRenderProcessHandler : public virtual CefBaseRefCounted {
 public:
  // 当渲染进程的主线程被创建时调用
  virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {}

  // 当WebKit被初始化后调用
  virtual void OnWebKitInitialized() {}

  // 当一个browser被创建之后调用。在rowsing cross-origin时，会在具有相同标识符的旧浏
  // 览器被销毁之前创建新的浏览器。
  virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) {}

  // 一个browser被销毁前调用
  virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {}

  // 返回处理browser加载状态事件的handler
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() { return NULL; }

  // browser导航前被调用。取消导航返回true。允许导航返回false。
  virtual bool OnBeforeNavigation(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefRequest> request,
                                  NavigationType navigation_type,
                                  bool is_redirect) {
    return false;
  }

  // frame的V8 Contex被创建之后立即调用。通过CefV8Context::GetGlobal()可以获取JS的
  // 'window'对象。V8 handle只能被创建他的对象存取。通过CefV8Context::GetTaskRunner()
  // 可以获取到相关联线程上负责post task的task_runner
  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) {}

  // frame的V8 Contex被释放后立即调用。
  virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefFrame> frame,
                                 CefRefPtr<CefV8Context> context) {}

  // 为frame中未被捕获的全局exceptions调用。默认情况者这个函数是disabled的。
  // 设置CefSettings.uncaught_exception_stack_size > 0开启
  virtual void OnUncaughtException(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context,
                                   CefRefPtr<CefV8Exception> exception,
                                   CefRefPtr<CefV8StackTrace> stackTrace) {}

  // Called when a new node in the the browser gets focus。
  // 如果没有指定的node获得焦点则|node|为空。node对象表示这个函数被调用时DOM的快照。
  virtual void OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefDOMNode> node) {}

  // 当接收到来自其他进程的消息时被调用。如果消息被处理返回true。
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) {
    return false;
  }
};
```

