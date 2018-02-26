[TOC]

## CEF3多进程通信

CEF3运行在多进程中，所以需要在这些进程中提供消息通信机制。`CefBrowser`和`CefFrame`对象同时存在于browser和render进程中，每一个`CefBrowser`和`CefFrame`对象有一个与之关联的唯一ID。于是，CEF中进程通信通过`CefBrowser`对象提供的`SendProcessMessage`接口进行消息发送，并通过`CefRenderProcessHandler`和`CefClinet`提供的`OnProcessMessageReceived`接口进行消息接收。具体联系如下图：

![cef3_process_message](C:\Users\chenjunhan\Desktop\笔记\cef\img\cef3_process_message.jpg)

在CEF3中，`CefBrowser`表示的是一个浏览器页面，每一个页面会有多个`CefFrame`，最顶层的是`mainframe` ，每个`CefFrame`里面都会有自己的`context`，他们之间的联系如下图：

![association](C:\Users\chenjunhan\Desktop\笔记\cef\img\association.jpg)

### 1. 进程启动时消息

在browser进程中，通过`CefBrowserProcessHandler::OnRenderProcessThreadCreated()`可以为所有render进程提供相同的启动时信息。这个将会把信息传递给render进程的`CefRenderProcessHandler::OnRenderThreadCreated()` 。

### 2.进程运行时消息

#### 2.1. 消息发送

通过`CefProcessMessage `类可以在进程生命周期中的任意时间内使用进程消息传递信息。进程消息关联与一个指定的`CefBrowser`实例，并通过`CefBrowser::SendProcessMessage() `方法发送。进程消息通过`CefProcessMessage::GetArgumentList()`来包含任何需要的状态消息。

`CefBrowser::SendProcessMessage()`函数原型如下：

```c++
// include/internal/cef_types.h
typedef enum {
  ///
  // Browser process.
  ///
  PID_BROWSER,
  ///
  // Renderer process.
  ///
  PID_RENDERER,
} cef_process_id_t;

// include/cef_browser.h
class CefBrowser : public virtual CefBaseRefCounted {
 public:
  virtual bool SendProcessMessage(CefProcessId target_process,
                                  CefRefPtr<CefProcessMessage> message) = 0;
};
```

一个简单的实现如下：

```c++
// Create the message object.
CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create(“my_message”);

// Retrieve the argument list object.
CefRefPtr<CefListValue> args = msg>GetArgumentList();

// Populate the argument values.
args->SetString(0, "my string");
args->SetInt(0, 10);

// Send the process message to the render process.
// Use PID_BROWSER instead when sending a message to the browser process.
browser->SendProcessMessage(PID_RENDERER, msg);
```

#### 2.2. 消息接受

从browser进程发往render进程的消息将会在`CefRenderProcessHandler::OnProcessMessageReceived()`被接收。

从render进程发往browser进程的消息将会在`CefClient::OnProcessMessageReceived()`被接收。

两个接受函数原型如下：

```c++
// include/cef_client.h
class CefClient : public virtual CefBaseRefCounted {
 public:
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) {
    return false;
  }
};

// include/cef_render_process_handler.h
class CefRenderProcessHandler : public virtual CefBaseRefCounted {
 public:
  virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                        CefProcessId source_process,
                                        CefRefPtr<CefProcessMessage> message) {
    return false;
  }
};
```

一个简单的实现如下：

```c++
bool MyHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  // Check the message name.
  const std::string& message_name = message->GetName();
  if (message_name == "my_message") {
    // Handle the message here...
    return true;
  }
  return false;
}
```

与特定的CefFrame进行消息通信可以传递一个frame ID(通过`CefFrame::GetIdentifier()`获取)来作为参数，然后在接受消息的进程中通过`CefBrowser::GetFrame()`方法得到对应的CefFrame。

```c++
// Helper macros for splitting and combining the int64 frame ID value.
#define MAKE_INT64(int_low, int_high) \
    ((int64) (((int) (int_low)) | ((int64) ((int) (int_high))) << 32))
#define LOW_INT(int64_val) ((int) (int64_val))
#define HIGH_INT(int64_val) ((int) (((int64) (int64_val) >> 32) & 0xFFFFFFFFL))

// Sending the frame ID.
const int64 frame_id = frame->GetIdentifier();
args->SetInt(0, LOW_INT(frame_id));
args->SetInt(1, HIGH_INT(frame_id));

// Receiving the frame ID.
const int64 frame_id = MAKE_INT64(args->GetInt(0), args->GetInt(1));
CefRefPtr<CefFrame> frame = browser->GetFrame(frame_id);
```

### 3. 进程内多线程通信

CEF3中，多进程间通信主要通过`PostTask`的形式，在`include/cef_task.h`中提供了`CefPostTask`和`CefPostDelayedTask`两个接口，用于投递多线程消息，消息投递的对象是一个`CefTask` 实例。这个过程是异步的，消息将会被投递到对应线程的消息队列中。

```c++
// include/cef_task.h
class CefTask : public virtual CefBaseRefCounted {
 public:
  virtual void Execute() = 0;
};

bool CefPostTask(CefThreadId threadId, CefRefPtr<CefTask> task);

bool CefPostDelayedTask(CefThreadId threadId,
                        CefRefPtr<CefTask> task,
                        int64 delay_ms);
```

一个简单的实现如下：

```c++
#include "include/cef_task.h"

class MyTask : public CefTask {
 public:
  virtual void Execute() OVERRIDE {
    DoSomethingOnUiThread();
  }
  IMPLEMENT_REFCOUNTING(MyTask);
};

void DoSomething() {
  if (CefCurrentlyOn(TID_UI))
    DoSomethingOnUiThread();
  else {
    CefRefPtr<MyTask> task(new MyTask);
    CefPostTask(TID_UI, task);
  }
}
```

除了实现自定义实现Task进行消息投递，CEF3还提供了Closure的方式来进行消息投递：

```c++
if (!CefCurrentlyOn(TID_UI)) {
  CefPostTask(TID_UI, base::Bind(...));
}
```

