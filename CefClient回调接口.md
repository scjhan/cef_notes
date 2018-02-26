[TOC]

### 一、CefClient浏览器接口

#### 1. CefClient概念

`CefClient`是浏览器窗口的接口，提供一系列回调接口共浏览器窗口使用，在创建浏览器窗口时注册对应的`CefClient`：

```c++
CefRefPtr<CefClientImpl> client_impl(new CefClientImpl());
// 创建浏览器窗口
CefBrowserHost::CreateBrowser(window_info, client_impl, url, browser_settings, NULL);
```

#### 2. CefClient回调

`CefClient`为浏览器窗口提供了很多回调，当浏览器窗口的发生相关动作时，就会调用这些回调，用户可以在事件中获取或修改相关的信息。

CEF将各种回调封装为形如`CefXXXHandler`的类，并且在`CefClient`中提供`CefXXXHandler`来返回这个回调类指针，用户通过重写对应回调类的对应事件，即可获取或修改相关的信息。默认情况下，`CefXXXHandler`返回`NULL`也即采用默认处理相关事件。这些回调接口主要包括：

```c++
// include/cef_client.h

class CefClient : public virtual CefBaseRefCounted {
 public:
  // 处理右键菜单事件
  virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() {
    return NULL;
  }

  // 处理对话框事件
  virtual CefRefPtr<CefDialogHandler> GetDialogHandler() { return NULL; }

  // 处理页面状态相关的事件，如页面加载情况、地址栏、标题变化
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() { return NULL; }

  // 处理文件下载事件
  virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() { return NULL; }

  // 处理拖拽事件
  virtual CefRefPtr<CefDragHandler> GetDragHandler() { return NULL; }

  // 处理查找结果事件
  virtual CefRefPtr<CefFindHandler> GetFindHandler() { return NULL; }

  // 处理焦点事件
  virtual CefRefPtr<CefFocusHandler> GetFocusHandler() { return NULL; }

  // 处理地理位置事件
  virtual CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() {
    return NULL;
  }

  // 处理JS对话框事件
  virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() { return NULL; }

  // 处理键盘事件
  virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() { return NULL; }

  // 处理浏览器生命周期相关的事件，浏览器对象的创建、销毁以及弹出框
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() { return NULL; }

  // 处理浏览器页面加载状态的变化，如页面加载开始，完成，出错等
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() { return NULL; }

  // 处理窗口渲染被禁止时的事件
  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() { return NULL; }

  // 处理与浏览器请求相关的的事件，如资源的的加载，重定向等
  virtual CefRefPtr<CefRequestHandler> GetRequestHandler() { return NULL; }

  // 处理进程通信。返回true表示消息被处理
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) {
    return false;
  }
};
```

上述回调类中，比较常用的有：

```c++
GetDisplayHandler
CefLifeSpanHandler
CefLifeSpanHandler
```

#### 3. 一般实现

CefClient包含很多种回调，如果是要实现某种，比较常见的是：

```C++
class ClientHandler : public CefClient,
                      public CefLifeSpanHandler
                      /* other handler */ {
public:
  virtual CefRefPtr<CefLiseSpanHandler> GetLiseSpanHandler() OVERRIDE { return this; }
  
  // 重写CefLiseSpanHandler的函数
}
```

****

### 二、浏览器回调事件

`CefClient`中包含一系列回调接口，每个回调接口对应实现了相关的回调事件。

#### 1. 右键菜单事件CefContextMenuHandler

`CefContextMenuHandler`定义在`include/cef_contex_menu_handler.h`，包含的所有接口都必须在`UI thread`中调用。主要包含4个接口：

```c++
class CefContextMenuHandler : public virtual CefBaseRefCounted {
 public:
  typedef cef_event_flags_t EventFlags;

  // 右键菜单展示之前被调用。|params|包含右键菜单状态信息。|model|表示默认菜单，可以
  // 被清除为空使得菜单不显示或者显示自定义菜单。
  virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefContextMenuParams> params,
                                   CefRefPtr<CefMenuModel> model) {}

  // 允许展示自定义菜单时被调用。|params|提供菜单状态信息。|model|包含OnBeforeContexMenu
  // 调用之后被设定的结果。如果要展示自定义设置，则返回true，并在合适的时机同步或异步调用
  // callback，对应菜单项被选择可通过command_id来判断。
  virtual bool RunContextMenu(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefContextMenuParams> params,
                              CefRefPtr<CefMenuModel> model,
                              CefRefPtr<CefRunContextMenuCallback> callback) {
    return false;
  }

  // 当某个菜单项被选中时调用。如果是已经处理则返回true，否则返回false执行默认实现。
  // CEF定义了一些默认的command——Id，这些id定义在'include/internal/cef_types.h'
  // 的cef_menu_id_t中，自定义的command_id范围必须在MENU_ID_USER_FIRST到
  // MENU_ID_USER_LAST之间。
  virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                                    CefRefPtr<CefFrame> frame,
                                    CefRefPtr<CefContextMenuParams> params,
                                    int command_id,
                                    EventFlags event_flags) {
    return false;
  }

  // 右键菜单被取消（选择空的菜单项或者选择了某个项）时调用
  /*--cef()--*/
  virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame) {}
};
```

#### 2. 对话框事件CefDialogHandler

`CefDialogHandler`定义在`include/cef_dialog_handler`中，用于处理文件对话框事件。**所有回调都必须在`UI Thread`中调用**。

```c++
class CefDialogHandler : public virtual CefBaseRefCounted {
 public:
  // 运行一个文件选择对话框时被调用。|mode|表示要展示的对话框类型，定义在
  // include/internal/cef_types.hde cef_file_dialog_mode_t;|title|为对话框标题;
  // |default_file_path|表示打开的路径或者文件名;|accept_filters|表示打开的文件类型;
  // |selected_accept_filter|表示默认选择的类型。如果是展示自定义对话窗口则返回true，
  // 并在合适的时机执行callback。展示默认窗口返回false
  virtual bool OnFileDialog(CefRefPtr<CefBrowser> browser,
                            FileDialogMode mode,
                            const CefString& title,
                            const CefString& default_file_path,
                            const std::vector<CefString>& accept_filters,
                            int selected_accept_filter,
                            CefRefPtr<CefFileDialogCallback> callback) {
    return false;
  }
};
```

#### 3. 页面状态事件CefDisplayHandler

`CefDisplayHandler`定义在`include/cef_display_handler.h`中，用于处理页面状态相关的事件，如页面加载情况、地址栏、标题变化等。**所有事件都必须在`UI Thread`中调用**，包含8种事件：

```c++
class CefDisplayHandler : public virtual CefBaseRefCounted {
 public:
  // 当frame的地址改变时被调用
  virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               const CefString& url) {}

  // 当页面标题被改变时调用
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) {}

  // 当页面icon改变时调用
  virtual void OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                                  const std::vector<CefString>& icon_urls) {}

  // 当页面内容切换到全屏时被调用。如果|fullscreen|是true，则自动调整以填充browser
  // 区域，如果|fullscreen|是false，则调整为起初的大小
  virtual void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser,
                                      bool fullscreen) {}

  // 当browser展示工具提示时被调用。|text|参数包含提示文本。自定义展示处理返回true;
  // 由browser默认行为来展示返回false。当窗口渲染被禁止时由应用程序去绘制工具提示，
  // 这种情况下返回值会被无视。
  virtual bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) {
    return false;
  }

  // 当browser接收到状态消息时被调用。|value|参数包含要被显示的状态消息。
  virtual void OnStatusMessage(CefRefPtr<CefBrowser> browser,
                               const CefString& value) {}

  // 当显示console消息时被调用。如果要禁止消息输出到console则返回true
  virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                                const CefString& message,
                                const CefString& source,
                                int line) {
    return false;
  }

  // 当CefBrowserHost::SetAutoResizeEnabled设置为允许自动调整大小并且自动调整大小事件
  // 发生时被调用。|new_size|参数用来描述视图坐标的大小。如果事件被处理返回true；如过要
  // 默认处理返回false
  virtual bool OnAutoResize(CefRefPtr<CefBrowser> browser,
                            const CefSize& new_size) {
    return false;
  }
};
```

#### 4. 文件下载事件CefDownloadHandler

`CefDownloadHandler`定义在`include/cef_download_handler.h`中，用于处理文件下载事件，**所有事件都必须在`UI Thread`中调用**，包含2种事件：

```c++
class CefDownloadHandler : public virtual CefBaseRefCounted {
 public:
  // 下载前被调用。|suggested_name|表示下载文件的建议名称。默认情况下下载会被取消。
  virtual void OnBeforeDownload(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefDownloadItem> download_item,
      const CefString& suggested_name,
      CefRefPtr<CefBeforeDownloadCallback> callback) = 0;

  // 当下载状态或者进程信息更新时被调用。这个时间将会在OnBeforeDownLoad()之前或者之后
  // 被调用多次。
  virtual void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                                 CefRefPtr<CefDownloadItem> download_item,
                                 CefRefPtr<CefDownloadItemCallback> callback) {}
};
```

#### 5. 拖拽事件CefDragHandler

`CefDragHandler`定义在`include/cef_drag_handler.h`中，用于处理拖拽相关的事件。**所有回调必须在`UI Thread`调用**。主要由两种事件：

```c++
class CefDragHandler : public virtual CefBaseRefCounted {
 public:
  // 当一个外部拖拽事件进入浏览器窗口时被调用。|dragData|参数包含拖拽时间的数据，
  // |mask|参数表示拖拽类型，定义在include/internal/cef_types.h的
  // cef_drag_operations_mask_t中。默认处理返回false，取消事件返回true
  virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDragData> dragData,
                           DragOperationsMask mask) {
    return false;
  }

  // 当浏览器窗口的可拖动区域改变时被调用。可以通过'-webkit-app-region: drag/no-drag'
  // 指定CSS属性。如果在文档中没有定义可拖拽区域则这个事件也不会被调用。
  virtual void OnDraggableRegionsChanged(
      CefRefPtr<CefBrowser> browser,
      const std::vector<CefDraggableRegion>& regions) {}
};
```

#### 6. 查找结果事件CefFindHandler

`CefFindHandler`定义在`include/cef_find_handler.h`中，用于查找结果事件。**所有事件必须在`UI Thread`中调用**，主要有1种事件：

```c++
class CefFindHandler : public virtual CefBaseRefCounted {
 public:
  // 用于报告执行CefBrowserHost::Find()之后的查找结果。|identifer|是传递给Find()的
  // 识别码，|count|表示匹配项目数。|selectionRect|表示匹配的项目位置。
  // |activeMatchOrdinal| 表示当前的查找结果位置，如果这是最后一个查找请求则
  // |finalUpdate|为true
  virtual void OnFindResult(CefRefPtr<CefBrowser> browser,
                            int identifier,
                            int count,
                            const CefRect& selectionRect,
                            int activeMatchOrdinal,
                            bool finalUpdate) {}
};
```

#### 7. 焦点事件CefFocusHandler

`CefFocusHandler`定义在`include/cef_focus_handler.h`中，用于焦点事件。**所有事件必须在`UI Thread`中调用**，主要有3种事件：

```c++
class CefFocusHandler : public virtual CefBaseRefCounted {
 public:
  // 当浏览器组件失去焦点时被调用。例如TAB键被按下。如果浏览器将焦点给到下一个组件则
  // |next|为true，给到前一个组件则为false
  virtual void OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next) {}

  // 当浏览器组件请求焦点时被调用。|source|指明请求来源。如果允许设置焦点则返回true
  virtual bool OnSetFocus(CefRefPtr<CefBrowser> browser, FocusSource source) {
    return false;
  }

  // 浏览器组件取得焦点时被调用
  virtual void OnGotFocus(CefRefPtr<CefBrowser> browser) {}
};
```

#### 8. 地理位置事件CefGeolocationHandler

`CefGeolocationHandler`定义在`include/cef_geolocation_handler.h`中，用于地理位置信息权限请求事件。**所有事件必须在`UI Thread`中调用**，主要有2种事件：

```c++
class CefGeolocationHandler : public virtual CefBaseRefCounted {
 public:
  // 当页面请求读取位置信息权限时被调用。|requesting_url|表示请求这个权限的URL;
  // |request_id|是一个独一无二的权限请求ID。在函数内或者延迟调用
  // CefGeolocationCallback::Continue()来同意或者取消请求返回true，直接取消请求返
  // 回false
  virtual bool OnRequestGeolocationPermission(
      CefRefPtr<CefBrowser> browser,
      const CefString& requesting_url,
      int request_id,
      CefRefPtr<CefGeolocationCallback> callback) {
    return false;
  }

  // 请求被取消时调用
  virtual void OnCancelGeolocationPermission(CefRefPtr<CefBrowser> browser,
                                             int request_id) {}
};
```

#### 9. JS对话框事件CefJSDialogHandler

`CefJSDialogHandler`定义在`include/cef_jsdialog_handler.h`中，用于处理JavaScript对话框事件。**所有事件必须在`UI Thread`中调用**，主要有4种事件：

```c++
class CefJSDialogHandler : public virtual CefBaseRefCounted {
 public:
  // 当执行一个JS对话框时被调用。|origin_url|非空，它用于传递给
  // CefFormatUrlForSecurityDisplay函数来获取一个安全、用户友好的展示字符串。
  // |default_prompt_text|用于知道对话框提示。
  // 1. 设置|suppress_message|为true并返回false来抑制消息（抑制消息比立即执行回调更可取，
  // 因为它可以检测例如onbeforeunload中的垃圾邮件弹窗消息等恶意行为）
  // 2. 设置|suppress_message|为false并返回false使用默认实现（默认实现将会显示一个模态窗
  // 口并且抑制其他对话框请求直到该对话框被取消）
  // 3. 显示自定义对话框或者立即执行callback返回true。如果使用自定义对话框则在对话框被取消
  // 时必须调用一次|callback|
  virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
                          const CefString& origin_url,
                          JSDialogType dialog_type,
                          const CefString& message_text,
                          const CefString& default_prompt_text,
                          CefRefPtr<CefJSDialogCallback> callback,
                          bool& suppress_message) {
    return false;
  }

  // 执行一个请求用户是够离开页面的对话框。使用默认实现返回false。显示自定义对话框或者立即
  // 执行callback返回true。如果使用自定义对话框则在对话框被取消时必须调用一次|callback|
  virtual bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser,
                                    const CefString& message_text,
                                    bool is_reload,
                                    CefRefPtr<CefJSDialogCallback> callback) {
    return false;
  }

  // 取消任何等待对话框并重置保存对话框状态时被调用。一般调用场景为：由于页面导航等事件而
  // 被调用，无论当前是否存在任何对话框。
  virtual void OnResetDialogState(CefRefPtr<CefBrowser> browser) {}

  // 默认实现对话框被关闭时调用
  virtual void OnDialogClosed(CefRefPtr<CefBrowser> browser) {}
};
```

#### 10. 键盘事件CefKeyboardHandler

`CefKeyBoardHandler`定义在`include/cef_keyboard_handler.h`中，用于处理键盘事件。**所有事件必须在`UI Thread`中调用**，主要有2种事件：

```c++
class CefKeyboardHandler : public virtual CefBaseRefCounted {
 public:
  // 在键盘事件被发送到渲染进程之前被调用。|event|参数包含键盘事件信息;|os_event|参
  // 数为操作系统事件信息;如果事件被处理返回true。如果事件要在OnKeyEvent()中以快捷
  // 键消息处理，则设置|is_keyboard_shortcut|参数为true并返回false
  virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                             const CefKeyEvent& event,
                             CefEventHandle os_event,
                             bool* is_keyboard_shortcut) {
    return false;
  }

  // 当页面的渲染器和js有机会处理该事件时被调用。|event|参数包含键盘事件信息;
  // |os_event|参数为操作系统事件信息，如果是被处理，返回true
  virtual bool OnKeyEvent(CefRefPtr<CefBrowser> browser,
                          const CefKeyEvent& event,
                          CefEventHandle os_event) {
    return false;
  }
};
```

#### 11. 生命周期事件CefLifeSpanHandler

`CefLifeSpanHandler`定义在`include/cef_life_span_handler.h`，用来处理浏览器生命周期相关的事件，浏览器对象的创建、销毁以及弹出框。**所有事件都要在`UI Thread`中调用**。主要包含4种事件：

```c++
class CefLifeSpanHandler : public virtual CefBaseRefCounted {
 public:
  // 一个弹出窗口被创建时调用。|browser|和|frame|参数表示弹窗发起方;|target_url|和
  // |target_frame_name|参数指明弹窗的属性;|target_disposition|指明弹窗位置;如果是
  // 用户点击了按钮或者链接导致弹窗，则|user_gesture|为true，如果是DomContentLoaded
  // 事件导致则为false;|popupFeatures|结构体包含请求的弹窗的其他信息。允许创建弹窗则
  // 设置相应的|windowInfo|, |client|, |settings|，|no_javascript_access|信息然后
  // 返回false，取消弹窗返回true；
  virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             const CefString& target_url,
                             const CefString& target_frame_name,
                             WindowOpenDisposition target_disposition,
                             bool user_gesture,
                             const CefPopupFeatures& popupFeatures,
                             CefWindowInfo& windowInfo,
                             CefRefPtr<CefClient>& client,
                             CefBrowserSettings& settings,
                             bool* no_javascript_access) {
    return false;
  }

  // 当一个新的browser被创建时调用，这个回调是所引用的browser的第一个通知事件
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) {}

  // 当browser接收到close的请求时这个回调会被调用。这可能是由于显示调用了
  // efBrowserHost::*CloseBrowser()。或者是用户点击关闭了一个由CEF创建的top-level
  // 窗口，然后这个top-level窗口的父窗口也就是browser将会调用这个回调。
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) { return false; }

  // 在Browser被销毁之前调用。释放所有的Browser引用，并且在这个回调返回之后将不会再执行
  // 任何其他的操作。这个回调将是所以用的browser的最后一个通知事件
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) {}
};
```

#### 12. 加载状态事件CefLoadHandler

`CefLoadHandler`定义在`include/cef_load_handler.h`中，用于处理浏览器页面加载状态的变化，如页面加载开始，完成，出错等。**所有回调在`UI Thread`或者渲染进程主线程(`TID_RENDERER`)中被调用**。主要有4种事件：

```c++
class CefLoadHandler : public virtual CefBaseRefCounted {
 public:
  typedef cef_errorcode_t ErrorCode;
  typedef cef_transition_type_t TransitionType;

  // 当加载状态被改变时调用。这个回调会被调用两次
  // 一次是程序实现的或者用户行为启动的加载
  // 另一次是在加载动作完成、取消或者失败时
  // 这个回调会在任何调用了OnLoadStart之前被调用并且在任何调用了OnLoadError或者
  // OnLoadEnd之后被调用
  virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                    bool isLoading,
                                    bool canGoBack,
                                    bool canGoForward) {}

  // 在导航被提交并之后browser在frame中加载内容时被调用。
  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           TransitionType transition_type) {}
  
  // browser加载一个frame完成时被调用。
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode) {}

  // 导航失败或者取消时被调用
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) {}
};
```

#### 13. 窗口渲染关闭事件CefRenderHandler

`CefRenderHandler`定义在`include/cef_render_handler.h`中，用于处理窗口渲染被禁止时的事件。**所有回调在`UI Thread`或者渲染进程主线程(`TID_RENDERER`)中被调用**。主要有13种事件：

```c++
class CefRenderHandler : public virtual CefBaseRefCounted {
 public:
  // 返回辅助功能通知处理
  virtual CefRefPtr<CefAccessibilityHandler> GetAccessibilityHandler() {
    return NULL;
  }

  // 用于获取根窗口的矩形坐标，如果提供返回true
  virtual bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
    return false;
  }

  // 用于获取视图的矩形坐标，如果提供返回true
  virtual bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) = 0;

  // 用于获取view坐标转换为屏幕坐标的结果，如果提供返回true
  virtual bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
                              int viewX,
                              int viewY,
                              int& screenX,
                              int& screenY) {
    return false;
  }

  // 用于浏览器为CefScreenInfo对象填充适当的值，如果已修改返回true。
  // 如果屏幕矩形留空，则将使用通过GetViewRect函数获取的矩形。如果该矩形仍然为空或者是
  // 无效的弹出窗口，则窗口可能无法正确绘制。
  virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
                             CefScreenInfo& screen_info) {
    return false;
  }

  // 当浏览器要显示或者隐藏弹窗组件时被调用。如果|show|为true则弹窗必须要显示
  virtual void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) {}

  // 当浏览器要移动或者调整弹窗大小时被调用，|rect|包含新的视图坐标位置和大小
  virtual void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) {
  }

  // 当一个元素需要被绘制时调用。传递给这个函数的像素值将会根据CefScreenIndo返回的
  // CefScreenInfo.device_scale_factor的值相对于视图坐标进行转换。|type|指明元素
  // 是视图还是弹出窗口。|buffer|包含整个图片的像素数据。|dirtyRects|包含需要重新
  // 绘制的像素坐标中的一组矩形。|buffer|的大小为|width|*|height|*4 bytes，表示一
  // 个以左上角为源点的BGRA图片
  virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                       PaintElementType type,
                       const RectList& dirtyRects,
                       const void* buffer,
                       int width,
                       int height) = 0;

  // 当浏览器光标改变时被调用。如果|type|是CT_CUSTOM则|custom_cursor_info|会被填充
  // 为自定义光标信息
  virtual void OnCursorChange(CefRefPtr<CefBrowser> browser,
                              CefCursorHandle cursor,
                              CursorType type,
                              const CefCursorInfo& custom_cursor_info) {}

  // 当用户在web view中开始拖拽内容时被调用。|drag_data|指定拖拽内容的上下文信息。
  // (|x|, |y|)表示开始拖拽的屏幕坐标。运行系统消息循环的OS API可以在StartDragging
  // 调用中使用。
  // 终止拖拽动作返回false。
  // 处理拖拽动作返回true，然后在合适的时机同步或异步调用
  // CefBrowserHost::DragSourceEndedAt和DragSourceSystemDragEnded

  virtual bool StartDragging(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefDragData> drag_data,
                             DragOperationsMask allowed_ops,
                             int x,
                             int y) {
    return false;
  }

  // 当webview在drag & drop操作中更新鼠标光标时被调用。
  virtual void UpdateDragCursor(CefRefPtr<CefBrowser> browser,
                                DragOperation operation) {}

  // 当滚动栏偏移量改变时被调用
  virtual void OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser,
                                     double x,
                                     double y) {}

  // 当输入法编辑器（IME）composition范围改变时被调用。|selected_range|表示被选中的字符范
  // 围。|character_bounds|表示每个字符在视图坐标中的边界
  virtual void OnImeCompositionRangeChanged(CefRefPtr<CefBrowser> browser,
                                            const CefRange& selected_range,
                                            const RectList& character_bounds) {}
};
```

#### 14. 请求事件CefRequestHandler

`CefRequestHandler`定义在`include/cef_request_handler.h`，用于处理与浏览器请求相关的的事件，如资源的的加载，重定向等。主要包含5种事件类型：

```c++
class CefRequestHandler : public virtual CefBaseRefCounted {
 public:
  // !! UI线程调用 !!
  // 取消导航返回true，允许导航继续返回false
  virtual bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefRequest> request,
                              bool is_redirect) {
    return false;
  }

  // !! UI线程中调用 !!
  // OnBeforeBrowse之前在某些限定条件下可能需要导航到不同的浏览器时被调用。
  virtual bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                const CefString& target_url,
                                WindowOpenDisposition target_disposition,
                                bool user_gesture) {
    return false;
  }

  // !! IO线程中调用 !!
  // 在一个资源请求被加载前调用。返回RV_CONTINUE来继续请求或者返回RV_CONTINUE_ASYNC并
  // 在后面的时间里调用CefRequestCallback::Continue()来异步继续或取消请求。返回
  // RV_CANCEL取消请求。
  virtual ReturnValue OnBeforeResourceLoad(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      CefRefPtr<CefRequestCallback> callback) {
    return RV_CONTINUE;
  }

  // !! IO线程中调用 !!
  // 允许资源默认加载返回NULL，或者返回指定的资源加载handler
  virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) {
    return NULL;
  }

  // !! IO线程中调用 !!
  // 当资源请求被重定向时调用。|request|参数表示旧的url；|response|参数将包含重定向结
  // 果；|new_url|表示新的url
  virtual void OnResourceRedirect(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefRequest> request,
                                  CefRefPtr<CefResponse> response,
                                  CefString& new_url) {}

  // !! IO线程中调用 !!
  // 当接收到资源回复时被调用。允许默认加载返回false；重定向或者修改请求并重试返回true
  virtual bool OnResourceResponse(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefRequest> request,
                                  CefRefPtr<CefResponse> response) {
    return false;
  }


  // !! IO线程中调用 !!
  // 可选择的过滤资源响应内容。  
  virtual CefRefPtr<CefResponseFilter> GetResourceResponseFilter(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      CefRefPtr<CefResponse> response) {
    return NULL;
  }

  // !! IO线程中调用 !!
  // 当资源加载完成时被调用。
  virtual void OnResourceLoadComplete(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefRequest> request,
                                      CefRefPtr<CefResponse> response,
                                      URLRequestStatus status,
                                      int64 received_content_length) {}

  // !! IO线程中调用 !!
  // 当浏览器需要用于证书时被调用，|scheme|是认证的scheme，如"basic"或者"digest"。
  // 认证信息有效时返回true，需要在函数中调用或者延迟调用CefAuthCallback::Continue()
  // 返回false立即取消请求
  virtual bool GetAuthCredentials(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  bool isProxy,
                                  const CefString& host,
                                  int port,
                                  const CefString& realm,
                                  const CefString& scheme,
                                  CefRefPtr<CefAuthCallback> callback) {
    return false;
  }
  
  // !! IO线程调用 !!
  // 当JS通过webkitStorageInfo.requestQuota函数指定存储配额大小时被调用。|origin_url|
  // 是对应的请求页面，|new_size|是请求的配额大小(bytes)。返回true时需要在函数中调用
  // CefRequestCallback::Continue()来允许或者否决请求。返回false立即取消请求。
  virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
                              const CefString& origin_url,
                              int64 new_size,
                              CefRefPtr<CefRequestCallback> callback) {
    return false;
  }
  
  // !! UI线程调用 !!
  // 当请求是未知协议时被调用，设置|allow_os_execution|参数为true来通过注册OS协议处理
  // 来尝试执行请求。
  virtual void OnProtocolExecution(CefRefPtr<CefBrowser> browser,
                                   const CefString& url,
                                   bool& allow_os_execution) {}

  // !! UI线程调用 !!
  // SSL证书校验错误时被调用。返回true时在函数中调用CefRequestCallback::Continue()或者
  // 在一段时间后继续或者取消请求。立即取消请求返回false。
  virtual bool OnCertificateError(CefRefPtr<CefBrowser> browser,
                                  cef_errorcode_t cert_error,
                                  const CefString& request_url,
                                  CefRefPtr<CefSSLInfo> ssl_info,
                                  CefRefPtr<CefRequestCallback> callback) {
    return false;
  }

  // !! UI线程调用 !!
  // 在请求客户端证书进行认证时被调用。采用默认操作自动选择第一个有效证书返回false；返回
  // true时，需要在函数中调用CefSelectClientCertificateCallback::Select或者在一段时间
  // 后选择证书。|isProxy|参数指明是否采用代理，|host|和|port|表示SSL服务端信息，
  // |certificates|参数表示可选择的证书列表
  virtual bool OnSelectClientCertificate(
      CefRefPtr<CefBrowser> browser,
      bool isProxy,
      const CefString& host,
      int port,
      const X509CertificateList& certificates,
      CefRefPtr<CefSelectClientCertificateCallback> callback) {
    return false;
  }

  // 当插件奔溃时调用，|plugin_path|参数指明奔溃的插件路径
  virtual void OnPluginCrashed(CefRefPtr<CefBrowser> browser,
                               const CefString& plugin_path) {}

  // !! UI线程调用 !!
  // 当渲染进程渲染browser相关的视图并准备接受或者处理IPC消息时被调用
  virtual void OnRenderViewReady(CefRefPtr<CefBrowser> browser) {}

  // !! UI线程调用 !!
  // 渲染进程意外终止时被调用，|status|参数指明终止原因
  virtual void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                         TerminationStatus status) {}
};
```

