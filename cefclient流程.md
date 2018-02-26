### 1. 入口函数

```c++
// cefclient_win.cc
int RunMain(HINSTANCE hInstance, int nCmdShow) {
  // 创建一个cef_app
  CefRefPtr<CefApp> app = new ClientAppBrowser();
  // 初始化Contex单例，MainContextImpl实现Contex的纯虚函数，外界通过Contex单例来访问这些操作
  scoped_ptr<MainContextImpl> context(new MainContextImpl(command_line, true));
  
  // 创建并显示win窗口
  context->GetRootWindowManager()->CreateRootWindow(window_config);
}
```

### 2. 窗口创建过程

```c++
// from cefclient_win.cc
context->GetRootWindowManager()->CreateRootWindow(window_config);

// contex中的RootWindowManager负责管理CefClient运行时产生的窗口，通过Create创建一个新的窗口
scoped_refptr<RootWindow> RootWindowManager::CreateRootWindow(const RootWindowConfig& config) {
  // 第一步，调用RootWinow的Create函数创建一个窗口，窗口有两种
  // useview类型（返回RootWindowViews）
  // 非useview类型（返回RootWindowWin）
  scoped_refptr<RootWindow> root_window = RootWindow::Create(MainContext::Get()->UseViews());
  
  // 调用窗口的Init函数，这里会初始化整个窗口的部件，包括导入有所测试
  root_window->Init(this, config, settings);
  
  // 这里将root_window的引用保存到Manager中管理
  OnRootWindowCreated(root_window);
  
  return root_window;
}
```

#### 2.1 RootWindowViews窗口

```c++
// root_window_views.h
void RootWindowViews::Init(RootWindow::Delegate* delegate, 
                           const RootWindowConfig& config, 
                           const CefBrowserSettings& settings) {
  // RootWindowViews含有成员client_handler_，继承于多个Cef handler接口，用于处理各种消息回调
  // client_handler_重写了CefLifeSpanHandler::OnAfterCreated函数，创建handler之后做一些初始化操作
  // 初始化操作包括
  // 1. test_runner::CreateMessageHandlers(message_handler_set_);
  // 这个操作将各种测试的handler插入到message_handler_set_中
  // 2. message_router_->AddHandler(*(it), false);
  // 这个操作遍历message_handler_set_，将所有测试handler添加到message_router里面
  CreateClientHandler(config.url);
  
  // 继续初始化window
  // 这个操作将加载icon缓存，然后注册一个回调RootWindowViews::CreateViewsWindow，用于创建窗口
  InitOnMainThread(settings, config.url);
}

void RootWindowViews::CreateViewsWindow(
    const CefBrowserSettings& settings,
    const std::string& startup_url,
    CefRefPtr<CefRequestContext> request_context,
  const ImageCache::ImageSet& images) {
  // 调用ViewWindow::Create创建窗口并显示出来
  ViewsWindow::Create(this, client_handler_, startup_url, settings, request_context);
}

// views_window.cc
CefRefPtr<ViewsWindow> ViewsWindow::Create(
    Delegate* delegate,
    CefRefPtr<CefClient> client,
    const CefString& url,
    const CefBrowserSettings& settings,
  CefRefPtr<CefRequestContext> request_context) {
  // 创建ViewWindow
  CefRefPtr<ViewsWindow> views_window = new ViewsWindow(delegate, NULL);
  
  // 创建BrowserView
  CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
      client, url, settings, request_context, views_window);
  
  // 设置创建BrowserView
  views_window->SetBrowserView(browser_view);
  
  // 创建窗口并显示
  CefWindow::CreateTopLevelWindow(views_window);
  
  reutrn views_window;
}
```

