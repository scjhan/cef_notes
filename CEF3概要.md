这个Wiki页提供CEF3的概要以及一般使用方式

[TOC]

# Introduction

The Chromium Embedded Framework (CEF) is an open source project based on the [Google Chromium](http://www.chromium.org/Home) project. Unlike the Chromium project itself, which focuses mainly on Google Chrome application development, CEF focuses on facilitating embedded browser use cases in third-party applications. CEF insulates the user from the underlying Chromium and Blink code complexity by offering production-quality stable APIs, release branches tracking specific Chromium releases, and binary distributions. Most features in CEF have default implementations that provide rich functionality while requiring little or no integration work from the user. As of this article’s publication there are over 100 million installed instances of CEF around the world embedded in products from a wide range of companies and industries. A partial list of companies and products using CEF is available on the [CEF Wikipedia page](http://en.wikipedia.org/wiki/Chromium_Embedded_Framework#Applications_using_CEF). Some use cases for CEF include:

- Embedding an HTML5-compliant Web browser control in an existing native application.
- Creating a light-weight native “shell” application that hosts a user interface developed primarily using Web technologies.
- Rendering Web content “off-screen” in applications that have their own custom drawing frameworks.
- Acting as a host for automated testing of existing Web properties and applications.

CEF3 is the next generation of CEF based on the multi-process [Chromium Content API](http://www.chromium.org/developers/content-module/content-api). Advantages to CEF3’s multi-process architecture include:

- Improved performance and stability (JavaScript and plugins run in a separate process).
- Support for Retina displays.
- GPU acceleration for WebGL and 3D CSS.
- Cool new features like WebRTC (webcam support) and speech input.
- Better automated UI testing via the DevTools remote debugging protocol and [ChromeDriver2](https://code.google.com/p/chromedriver/wiki/ChromeDriver2).
- Faster access to current and future Web features and standards.

This document introduces the general concepts involved when developing an application using CEF3.

# 开始使用(Getting Started)

## 使用二进制包(Using a Binary Distribution)

CEF3二进制包在这个页面下载 [项目下载页](http://www.magpcss.net/cef_downloads/)。包括构建指定CEF3版本指定平台（Windows，Max OS X， Linux）的所有所需文件。不管是什么平台，文件整体结构大致如下：

- **cefclient  ** 包含一个cefclient的应用程序示例，此应用程序展示了广泛的CEF功能。


- **cefsimple** 包好一个cefsimple的应用程序示例。此应用程序展示创建一个最简单的浏览器窗口需要做的事。
- **Debug** 包含CEF共享库（libcef）的Debug版本构建以及在平台上运行所需的任何其他库。
- **include** 包含所有CEF头文件
- **libcef_dll** 包含libcef_dll_wrapper 静态库的源代码，应用程序使用CEF C++ API必须要链接此库。更多信息查看"C++封装"章节。
- **Release** 包含CEF共享库(libcef)的release版本构建以及平台上运行所需的任何其他库。
- **Resources** 包含应用程序使用CEF需要的资源(Windows和LInux)。包含.pak文件(二进制全局资源)和平台相关的可能需要的文件。

每个二进制包包含一个README.txt文件和一个LICENSE.txt文件，README.txt用以描述平台相关的细节，而LICENSE.txt包含CEF的BSD版权说明。如果你发布了基于CEF的应用，则应该在应用程序的某个地方包含该版权声明。例如，你可以在"关于”和“授权"页面列出该版权声明，或者单独一个文档包含该版权声明。“关于”和“授权”信息也可以分别在CEF浏览器的"about:license"和"about:credits"页面查看。

基于CEF二进制包的应用程序可以使用每个平台上的经典编译工具。包括Windows平台上的Visual Studio，Mac OSX平台上的Xcode，以及Linux平台上的gcc/make编译工具链。CEF项目的下载页面包含了这些平台上编译特定版本CEF所需的编译工具的版本信息。在Linux上编译CEF时需要特别注意依赖工具链。

[Tutorial](https://bitbucket.org/chromiumembedded/cef/wiki/Tutorial.md) Wiki页面有更多关于如何使用CEF3二进制包创建简单应用程序的细节。

## 源码编译(Building from Source Code)

CEF可以从源码编译，用户可以使用本地编译系统或者像 [TeamCity](http://www.jetbrains.com/teamcity/)这样的自动化编译系统编译。首先你需要使用svn或者git下载Chromium和CEF的源码。由于Chromium源码很大，只建议在内存大于6GB的现代机器上编译。编译Chromium和CEF的细节请参考 [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md)页面。

# 示例应用程序(Sample Application)

cefclient是一个完整的CEF客户端应用程序示例，并且它的源码包含在CEF每个二进制发布包中。使用CEF创建一个新的应用程序，最简单的方法是先从cefclient应用程序开始，删除你不需要的部分。本文档中许多示例都是来源于cefclient应用程序。

# 重要概念(Important Concepts)

一下是一些基于CEF3开发应用程序的重要的基础概念，在开始之前需要先理解这些概念。

## C++封装(C++ Wrapper)

libcef共享库导出了C API来隔离用户与CEF运行库和基础代码。libcef_dll_wrapper 项目以源代码的形式发布在二进制release版本中，将C API导出为C++ API，然后连接到客户端应用程序。这个C/C++ API通过[转换工具](https://bitbucket.org/chromiumembedded/cef/src/master/tools/translator.README.txt?at=master)自动生成的。使用C API的指引在 [使用CAPI](https://bitbucket.org/chromiumembedded/cef/wiki/UsingTheCAPI.md) 页面

## 进程(Processes)

CEF3使用多进程运行。主进程叫做Browser进程，负责窗口创建、绘制和网络交互，这通常就是host application的进程，并且大部分应用程序逻辑将在Browser进程中运行。Blink渲染和JavaScript都在Render进程执行，一些应用逻辑记录Javascript绑定和DOM节点的访问也在Render进程执行。默认的[进程模型](http://www.chromium.org/developers/design-documents/process-models)为每一个单独的origin(scheme+domain)创建一个新的Render进程。其他进程按照需要创建，例如"plugin"进程用来处[插件](http://www.chromium.org/developers/design-documents/plugin-architecture)例如Flash，"gpu"进程用来处理[合成加速](http://www.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome) 。

By default the main application executable will be spawned multiple times to represent separate processes. This is handled via command-line flags that are passed into the CefExecuteProcess function. If the main application executable is large, takes a long time to load, or is otherwise unsuitable for non-browser processes the host can use a separate executable for those other processes. This can be configured via the CefSettings.browser_subprocess_path variable. See the “Application Structure” section for more information.

The separate processes spawned by CEF3 communicate using Inter-Process Communication (IPC). Application logic implemented in the browser and render processes can communicate by sending asynchronous messages back and forth. [JavaScriptIntegration](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md) in the render process can expose asynchronous APIs that are handled in the browser process. See the “Inter-Process Communication” section for more information.

Platform-specific debugging tips are also available for [Windows](https://www.chromium.org/developers/how-tos/debugging-on-windows), [Mac OS X](http://www.chromium.org/developers/how-tos/debugging-on-os-x) and [Linux](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_debugging.md).

## 线程(Threads)

每个CEF3进程运行着多个线程，完整的线程枚举查看 [cef_thread_id_t](http://magpcss.org/ceforum/apidocs3/projects/(default)/cef_thread_id_t.html) 。以下是一些常用的线程： 

- **TID_UI** 线程：browser进程的主线程，如果CefInitialize()设置了CefSettings.multi_threaded_message_loop值为false，则这个线程也是应用程序的主线程。
- **TID_IO** 线程：browser进程处理IPC和网络消息
- **TID_FILE** 线程：browser进程与文件系统进行交互。所有阻塞的操作应该只在这个线程或者应用程序创建的自定义[CefThread](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefThread.html)中执行。
- **TID_RENDERER** 线程：renderer进程的主线程。所有Blink和V8引擎交互必须在这个线程内执行。

由于CEF是多线程的，所以多个线程访问数据时，使用消息传递或锁保护数据成员很重要。CefPostTask函数族支持简单的线程间异步消息传递，详细信息查看"投递任务"章节。

使用CefCurrentlyOn() 函数可以判别当前线程。CEF sample程序使用以下几个定义来让方法在指定的线程中执行。这些定义包含在 [include/wrapper/cef_helpers.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_helpers.h?at=master)头文件中：

```c++
#define CEF_REQUIRE_UI_THREAD()       DCHECK(CefCurrentlyOn(TID_UI));
#define CEF_REQUIRE_IO_THREAD()       DCHECK(CefCurrentlyOn(TID_IO));
#define CEF_REQUIRE_FILE_THREAD()     DCHECK(CefCurrentlyOn(TID_FILE));
#define CEF_REQUIRE_RENDERER_THREAD() DCHECK(CefCurrentlyOn(TID_RENDERER));
```

CEF提供base::Lock和base::AutoLock来支持代码块的同步访问，这个定义在[include/base/cef_lock.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/base/cef_lock.h?at=master) 头文件中.例如：

```c++
// Include the necessary header.
#include "include/base/cef_lock.h"

// Class declaration.
class MyClass : public CefBase {
 public:
  MyClass() : value_(0) {}
  // Method that may be called on multiple threads.
  void IncrementValue();
 private:
  // Value that may be accessed on multiple theads.
  int value_;
  // Lock used to protect access to |value_|.
  base::Lock lock_;
  IMPLEMENT_REFCOUNTING(MyClass);
};

// Class implementation.
void MyClass::IncrementValue() {
  // Acquire the lock for the scope of this method.
  base::AutoLock lock_scope(lock_);
  // |value_| can now be modified safely.
  value_++;
}
```

## 引用计数(Reference Counting)

框架的所有类实现CefBase接口，所有的实例指针使用CefRedPtr智能指针通过AddRef()和Release()来自动管理引用计数。一个简单的实现如下：

```c++
class MyClass : public CefBase {
 public:
  // Various class methods here...

 private:
  // Various class members here...

  IMPLEMENT_REFCOUNTING(MyClass);  // Provides atomic refcounting implementation.
};

// References a MyClass instance
CefRefPtr<MyClass> my_class = new MyClass();
```

## 字符串(Strings)

CEF定义了自己的字符串数据结构，主要有几个原因：

- libcef和宿主应用程序可能使用不同的运行库来管理堆内存。所有对象，包括strings，需要使用与分配内存相同的运行库来释放。
- libcef可以编译成支持不同的string类型 (UTF8, UTF16 或者宽字符)。默认情况下是UTF16，但是可以通过在 [cef_string.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/internal/cef_string.h?at=master) 中修改并重新编译CEF。当使用宽字符时需要注意不同平台小大小可能会变化。

UTF16的字符串数据结构如下：

```c++
typedef struct _cef_string_utf16_t {
  char16* str;  // Pointer to the string
  size_t length;  // String length
  void (*dtor)(char16* str);  // Destructor for freeing the string on the correct heap
} cef_string_utf16_t;
```

选定的字符串将会被重命名为基础的类型：

```c++
typedef char16 cef_char_t;
typedef cef_string_utf16_t cef_string_t;
```

CEF提供大量的C API来操作CEF string（通过#defines映射到指定的函数），例如：

- **cef_string_set** 将使用或不复制值将字符串值赋给结构。
- **cef_string_clear** 将清除字符串内容
- **cef_string_cmp** 将比较两个字符串

CEF也提供了所有支持的string类型 (ASCII, UTF8, UTF16 and wide)的转换函数，详细转换函数信息查看 [cef_string.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/internal/cef_string.h?at=master) 和 [cef_string_types.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/internal/cef_string_types.h?at=master) 头文件。

C++中，CEF strings使用CefString简化，并提供了与std::string(UTF8)和std::string(wide)的自动转换。也可以使用一个已存在的cef_string_t structure进行赋值。

从std::string赋值:

```c++
std::string str = "Some UTF8 string";

// Equivalent ways of assigning |str| to |cef_str|. Conversion from UTF8 will occur if necessary.
CefString cef_str(str);
cef_str = str;
cef_str.FromString(str);

// Equivalent ways of assigning |cef_str| to |str|. Conversion to UTF8 will occur if necessary.
str = cef_str;
str = cef_str.ToString();
```

从std::wstring赋值:

```c++
std::wstring str = “Some wide string”;

// Equivalent ways of assigning |str| to |cef_str|. Conversion from wide will occur if necessary.
CefString cef_str(str);
cef_str = str;
cef_str.FromWString(str);

// Equivalent ways of assigning |cef_str| to |str|. Conversion to wide will occur if necessary.
str = cef_str;
str = cef_str.ToWString();
```

如果已知格式是ASCII，可以使用FromASCII()方法：

```c++
const char* cstr = "Some ASCII string";
CefString cef_str;
cef_str.FromASCII(cstr);
```

一些数据结构如CefSettings具有have cef_string_t成员. CefString可以用于简单的初始化这些成员：

```c++
CefSettings settings;
const char* path = "/path/to/log.txt";

// Equivalent assignments.
CefString(&settings.log_file).FromASCII(path);
cef_string_from_ascii(path, strlen(path), &settings.log_file);
```

## 命令行参数(Command Line Arguments)

CEF3和Chromium的许多特性可以使用命令行参数进行配置。这些参数采用"`--some-argument[=optional-param]`"的形式，并通过`CefExecuteProcess()`和`CefMainArgs`结构（参考"应用程序结构"章节）传递给CEF。

- 禁用对命令行参数的处理可以在传递`CefSettings`结构给`CefInitialize()`之前设置`CefSettings.command_line_args_disabled`为true。
- 如果想为CEF3和Chromium宿主程序指定命令行参数，实现`CefApp::OnBeforeCommandLineProcessing()`方法。


- 在传递CefSettings结构给CefInitialize()之前，我们可以设置CefSettings.command_line_args_disabled为true来禁用对命令行参数的处理。如果想指定命令行参数传入主应用程序，实现CefApp::OnBeforeCommandLineProcessing()方法。
- 传递特定应用程序(非CEF/Chromium)命令行参数给子进程，实现`CefBrowserProcessHandler::OnBeforeChildProcessLaunch() `方法。

更多支持的CEF/Chromium命令行参数开关查看 [shared/common/client_switches.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/shared/common/client_switches.cc?at=master) 文档的注释。

# Application Layout

Application layout can differ significantly depending on the platform. For example, on Mac OS X your application layout must follow a specific app bundle structure. Windows and Linux are more flexible, allowing you to customize the location where CEF libraries and resources are stored. For a complete working example of the required layout you can download a “Sample Application” from <http://opensource.spotify.com/cefbuilds/index.html>. Some distributed files are required and some are optional. Requirements and additional information for each file can be found in the binary distribution README.txt file.

## Windows

On Windows the default layout places the libcef library and related resources next to the application executable. The directory structure looks like this for 2623 branch:

```
Application/
    cefclient.exe  <= cefclient application executable
    libcef.dll <= main CEF library
    icudtl.dat <= unicode support data
    libEGL.dll, libGLESv2.dll, ... <= accelerated compositing support libraries
    cef.pak, devtools_resources.pak, ... <= non-localized resources and strings
    natives_blob.bin, snapshot_blob.bin <= V8 initial snapshot
    locales/
        en-US.pak, ... <= locale-specific resources and strings

```

The location of the CEF libraries and resource files can be customized using the CefSettings structure (see the README.txt file or “CefSettings” section for details). The cefclient application on Windows compiles in resources via the BINARY resource type in [cefclient/resources/win/cefclient.rc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/resources/win/cefclient.rc?at=master) but an application could just as easily load resources from the local file system.

## Linux

On Linux the default layout places the libcef library and related resources next to the application executable. Note however that there’s a discrepancy between where libcef.so is located in the client distribution and where it’s located in the binary distribution that you build yourself. The location depends on how the linker rpath value is set when building the application executable. For example, a value of “-Wl,-rpath,.” (“.” meaning the current directory) will allow you to place libcef.so next to the application executable. The path to libcef.so can also be specified using the LD_LIBRARY_PATH environment variable. The directory structure looks like this for 2623 branch:

```
Application/
    cefclient  <= cefclient application executable
    chrome-sandbox <= sandbox support binary
    libcef.so <= main CEF library
    icudtl.dat <= unicode support data
    cef.pak, devtools_resources.pak, ... <= non-localized resources and strings
    natives_blob.bin, snapshot_blob.bin <= V8 initial snapshot
    locales/
        en-US.pak, ... <= locale-specific resources and strings
    files/
        binding.html, ... <= cefclient application resources

```

The location of the CEF libraries and resource files can be customized using the CefSettings structure (see the README.txt file of “CefSettings” section for details).

## Mac OS X

The application (app bundle) layout on Mac OS X is mandated by the Chromium implementation and consequently is not very flexible. The directory structure looks like this for 2623 branch:

```
cefclient.app/
    Contents/
        Frameworks/
            Chromium Embedded Framework.framework/
                Chromium Embedded Framework <= main application library
                Resources/
                    cef.pak, devtools_resources.pak, ... <= non-localized resources and strings
                    icudtl.dat <= unicode support data
                    natives_blob.bin, snapshot_blob.bin <= V8 initial snapshot
                    en.lproj/, ... <= locale-specific resources and strings
            cefclient Helper.app/
                Contents/
                    Info.plist
                    MacOS/
                        cefclient Helper <= helper executable
                    Pkginfo
        Info.plist
        MacOS/
            cefclient <= cefclient application executable
        Pkginfo
        Resources/
            binding.html, ... <= cefclient application resources

```

The "Chromium Embedded Framework.framework" is an [unversioned framework](http://src.chromium.org/viewvc/chrome/trunk/src/build/mac/copy_framework_unversioned.sh?view=markup) that contains all CEF binaries and resources. Executables (cefclient, cefclient Helper, etc) are linked to libcef.dylib using [install_name_tool](https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man1/install_name_tool.1.html) and a path relative to @executable_path.

The "cefclient Helper" app is used for executing separate processes (renderer, plugin, etc) with different characteristics. It needs to have a separate app bundle and Info.plist file so that, among other things, it doesn’t show dock icons.

# 应用程序结构(Application Structure)

每个CEF3应用程序有着相同的基本结构。

- 提供一个入口函数，用于初始化CEF、运行每一个子进程逻辑或者CEF消息循环。
- 提供一个 [CefApp](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefApp.html)的实现，用于处理指定进程的回调。
- 提供一个 [CefClient](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefClient.html)的实现，用于处理指定browser实例的回调。
- 调用CefBrowserHost::CreateBrowser()创建一个browser实例，通过[CefLifeSpanHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html)管理browser的生命周期。

## 入口函数(Entry-Point Function)

正如“进程”章节描述，CEF3应用程序运行在多个进程中。这些进程可以使用相同的执行体或者为每一子进程提供相应的执行体。进程的执行从入口函数开始。完整的Windows，Linux，Mac OS-X平台实例在[cefclient/cefclient_win.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/cefclient_win.cc?at=master), [cefclient/cefclient_gtk.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/cefclient_gtk.cc?at=master)和 [cefclient/cefclient_mac.mm](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/cefclient_mac.mm?at=master) .

当启动一个子进程时，会通过命令行指定一些配置信息，这些命令行参数必须通过CefMainArgs结构体传入到CefExecuteProcess函数。CefMainArgs的定义与平台相关，在Linux、Mac OS X平台下，它接收 [main() 函数](https://en.wikipedia.org/wiki/Main_function)传入的argc和argv参数值。

```c++
CefMainArgs main_args(argc, argv);
```

在Windows平台下，它接收[wWinMain](http://msdn.microsoft.com/en-us/library/windows/desktop/ff381406(v=vs.85).asp)函数传入的参数：实例句柄（HINSTANCE），这个实例能够通过函数GetModuleHandle(NULL)获取。

```c++
CefMainArgs main_args(hInstance);
```

### 单一执行体(Single Executable)

当以单一执行体运行时，根据不同的进程类型入口函数不同。Windows、Linux平台支持单一执行体，Mac OS X平台则不行。

```c++
// Program entry-point function.
int main(int argc, char* argv[]) {
  // Structure for passing command-line arguments.
  // The definition of this structure is platform-specific.
  CefMainArgs main_args(argc, argv);

  // Optional implementation of the CefApp interface.
  CefRefPtr<MyApp> app(new MyApp);

  // Execute the sub-process logic, if any. This will either return immediately for the browser
  // process or block until the sub-process should exit.
  int exit_code = CefExecuteProcess(main_args, app.get());
  if (exit_code >= 0) {
    // The sub-process terminated, exit now.
    return exit_code;
  }

  // Populate this structure to customize CEF behavior.
  CefSettings settings;

  // Initialize CEF in the main process.
  CefInitialize(main_args, settings, app.get());

  // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
  CefRunMessageLoop();

  // Shut down CEF.
  CefShutdown();

  return 0;
}
```

### 分离子进程执行体(Separate Sub-Process Executable)

当使用独立的子进程执行体时，需要2个分离的可执行工程和2个分离的入口函数。

主程序入口函数：

```c++
// Program entry-point function.
int main(int argc, char* argv[]) {
  // Structure for passing command-line arguments.
  // The definition of this structure is platform-specific.
  CefMainArgs main_args(argc, argv);

  // Optional implementation of the CefApp interface.
  CefRefPtr<MyApp> app(new MyApp);

  // Populate this structure to customize CEF behavior.
  CefSettings settings;

  // Specify the path for the sub-process executable.
  CefString(&settings.browser_subprocess_path).FromASCII("/path/to/subprocess");

  // Initialize CEF in the main process.
  CefInitialize(main_args, settings, app.get());

  // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
  CefRunMessageLoop();

  // Shut down CEF.
  CefShutdown();

  return 0;
}
```

子进程入口函数：

```c++
// Program entry-point function.
int main(int argc, char* argv[]) {
  // Structure for passing command-line arguments.
  // The definition of this structure is platform-specific.
  CefMainArgs main_args(argc, argv);

  // Optional implementation of the CefApp interface.
  CefRefPtr<MyApp> app(new MyApp);

  // Execute the sub-process logic. This will block until the sub-process should exit.
  return CefExecuteProcess(main_args, app.get());
}
```

#### 集成消息轮询(Message Loop Integration)

CEF可以不用它自己提供的消息循环，而与已经存在的程序中消息环境集成在一起，有两种方式可以做到：

1. 周期性执行`CefDoMessageLoopWork()`函数来替代调用`CefRunMessageLoop()`。`CefDoMessageLoopWork()`的每一次调用都将执行一次CEF消息循环的单次迭代。需要注意的是，此方法调用次数太少时，CEF消息循环会饿死，将极大的影响browser的性能，调用次数太频繁又将影响CPU使用率。
2. 设置`CefSettings.multi_threaded_message_loop=true`（Windows平台下有效），这个设置项将导致CEF在单独的线程上运行browser的界面，而不是在主线程上，这种场景下`CefDoMessageLoopWork()`或者`CefRunMessageLoop()`都不需要调用，`CefInitialze()`、`CefShutdown()`仍然在主线程中调用。你需要提供主程序线程通信的机制（查看`cefclient_win.cpp`中提供的消息窗口实例）。在Windows平台下，你可以通过命令行参数`--multi-threaded-message-loop`测试上述消息模型。

## CefSettings

CefSettings结构允许配置应用程序范围的CEF设置。一些常见的配置项包括：

- **single_process** 设置true则browser和renderer使用单进程。也可以通过"single-process"命令行开光来配置。更多信息查看"进程"章节。
- **browser_subprocess_path** 为子进程启动的分离可执行程序的路径。更多信息查看"分离子进程执行体"章节。
- **multi_threaded_message_loop** 设置为true则browser进程消息循环将会运行在一个分离的线程。更多信息查看"集成消息轮询"章节。
- **command_line_args_disabled** 设置为true则关闭browser进程使用标准CEF和Chromium命令行参数的配置功能。更多信息查看"命令行参数"章节。
- **cache_path** 缓存数据在磁盘中的存储路径。如果为空，则部分功能使用in-memory的缓存，其他功能使用临时的磁盘缓存。形如本地存储的HTML5数据库只能在设置了缓存路径才能跨session存储。
- **locale** locale的值将传递给Blink。如果为空则使用默认值"en-US"。在Linux平台下此项被忽略，使用环境变量中的值，解析的依次顺序为：`LANGUAE`，`LC_ALL`，`LC_MESSAGES`和`LANG`。此项也可以通过命令行开关"lang"配置。
- **log_file** 设置的文件夹和文件名将用于输出debug日志。如果此项为空，默认的日志文件名为"debug.log"，位于应用程序所在的目录。此项也可以通过命令开关"log-file"配置。
- **log_severity** 日志级别。只有此等级、或者比此等级高的日志的才会被记录。此项可以通过命令行开关"log-severity"配置，可以设置的值为"verbose"，"info"，"warning"，"error"，"error-report"，"disable"。
- **resources_dir_path** 资源文件完整路径。如果为空则cef.pak和devtools_resources.pak文件在Windows/Linux平台下必须在组件文件夹，MAC OS X平台下必须在应用程序资源文件夹。可以通过命令行开关"resources-dir-path"配置。
- **locales_dir_path** locales文件夹的完整路径。如果为空，则locales文件夹会被设置在组件文件夹。Mac  OS X平台下这个值会被忽略，pack文件总是保存在应用程序资源文件夹。可以通过命令行开关"locales-dir-path"配置。
- **remote_debugging_port** 这个值的范围为1024~65535。允许在指定端口开启远程调试。例如指定为8080，则远程调试URL为http://localhost:8080。CEF可以被任何CEF或者chrome浏览器进行远程调试。可以通过命令行开关"remote-debugging-port"配置。

## CefBrowser和CefFrame(CefBrowser and CefFrame)

[CefBrowser](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefBrowser.html) 和 [CefFrame](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefFrame.html) 对象用来发送命令给浏览器以及在回调函数里获取状态信息 。每一个CefBrowser 对象包含一个代表页面的顶层frame的主CefFrame对象，以及零个或多个代表页面子frame的CefFrame对象。例如，一个浏览器加载了两个iframe，则该CefBrowser对象拥有三个CefFrame对象（顶层frame和两个iframe）。

在浏览器的主frame里加载一个URL：

```c++
browser->GetMainFrame()->LoadURL(some_url);
```

执行浏览器的回退操作：

```c++
browser->GoBack();
```

从主frame里获取HTML内容：

```c++
// Implementation of the CefStringVisitor interface.
class Visitor : public CefStringVisitor {
 public:
  Visitor() {}

  // Called asynchronously when the HTML contents are available.
  virtual void Visit(const CefString& string) OVERRIDE {
    // Do something with |string|...
  }

  IMPLEMENT_REFCOUNTING(Visitor);
};

browser->GetMainFrame()->GetSource(new Visitor());
```

CefBrowser和CefFrame对象存在于Browser进程和Render进程。在browser进程里，Host行为可以通过CefBrowser::GetHost()方法控制。例如，浏览器窗口的原生句柄可以用下面的代码获取：

```c++
// CefWindowHandle is defined as HWND on Windows, NSView* on Mac OS X
// and GtkWidget* on Linux.
CefWindowHandle window_handle = browser->GetHost()->GetWindowHandle();
```

其他方法如历史导航，加载字符串和请求，发送编辑命令，提取text/html内容等。请参考支持函数相关的文档。

## CefApp

 [CefApp](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefApp.html) 接口提供了访问指定进程回调的方式，比较重要的有：

- **OnBeforeCommandLineProcessing** 提供了以编程方式设置命令行参数的机会，更多细节，请参考"Command Line Arguments"一节。
- **OnRegisterCustomSchemes** 提供了注册自定义schemes的机会，更多细节，请参考"Request Handling"一节。
- **GetBrowserProcessHandler** 返回browser进程的H=handler，该handler包括了诸如OnContextInitialized的回调。
- **GetRenderProcessHandler** 返回定制Render进程的Handler，该Handler包含了JavaScript相关的一些回调以及消息处理的回调。 更多细节，请参考[Javascript集成](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md)和"Inter-Process Communication"章节。

简单的CefApp实现可查看[cefsimple/simple_app.h](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_app.h?at=master) and [cefsimple/simple_app.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_app.cc?at=master).

## CefClient

 [CefClient](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefClient.html)提供访问指定browser实例回调的方式。一个单独的CefClient实例可以在任意数量的browser中共享。下面是几个重要的回调：

- 比如处理browser的生命周期，右键菜单，对话框，通知显示， 拖曳事件，焦点事件，键盘事件等等。大多数handler是可选的，详细信息请查看cef_client.h文件中相关说明。
- **OnProcessMessageReceived** 在Browser收到Render进程的消息时被调用。更多细节，请参考“Inter-Process Communication” 章节。

一个简单的CefClient实现可查看[cefsimple/simple_handler.h](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler.h?at=master) 和 [cefsimple/simple_handler.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler.cc?at=master).

## Browser生命周期(Browser Life Span)

Browser生命周期从执行 CefBrowserHost::CreateBrowser() 或者 CefBrowserHost::CreateBrowserSync() 开始。可以在CefBrowserProcessHandler::OnContextInitialized() 回调或者特殊平台例如windows的WM_CREATE 中方便的执行业务逻辑。

```c++
// Information about the window that will be created including parenting, size, etc.
// The definition of this structure is platform-specific.
CefWindowInfo info;
// On Windows for example...
info.SetAsChild(parent_hwnd, client_rect);

// Customize this structure to control browser behavior.
CefBrowserSettings settings;

// CefClient implementation.
CefRefPtr<MyClient> client(new MyClient);

// Create the browser asynchronously. Initially loads the Google URL.
CefBrowserHost::CreateBrowser(info, client.get(), “http://www.google.com”, settings, NULL);
```

 [CefLifeSpanHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html) 类提供必要的接口来管理browser生命周期。下面是相关方法和成员的摘录。

```c++
class MyClient : public CefClient,
                 public CefLifeSpanHandler,
                 ... {
  // CefClient methods.
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
    return this;
  }

  // CefLifeSpanHandler methods.
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

  // Member accessors.
  CefRefPtr<CefBrowser> GetBrower() { return m_Browser; }
  bool IsClosing() { return m_bIsClosing; }

 private:
  CefRefPtr<CefBrowser> m_Browser;
  int m_BrowserId;
  int m_BrowserCount;
  bool m_bIsClosing;

  IMPLEMENT_REFCOUNTING(MyClient);
};
```

 OnAfterCreated() 方法将在browser被创建之后立即调用。host程序可以在这个方法中保存main browser对象的引用。

```c++
void MyClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  // Must be executed on the UI thread.
  REQUIRE_UI_THREAD();

  if (!m_Browser.get())   {
    // Keep a reference to the main browser.
    m_Browser = browser;
    m_BrowserId = browser->GetIdentifier();
  }

  // Keep track of how many browsers currently exist.
  m_BrowserCount++;
}
```

使用CefBrowserHost::CloseBrowser()销毁一个browser。

```c++
// Notify the browser window that we would like to close it. This will result in a call to 
// MyHandler::DoClose() if the JavaScript 'onbeforeunload' event handler allows it.
browser->GetHost()->CloseBrowser(false);
```

Browser对象的关闭事件来源于他的父窗口的关闭方法（比如，在父窗口上点击X控钮。）。父窗口需要调用 CloseBrowser(false) 并且等待操作系统的第二个关闭事件来决定是否允许关闭。注意一下面示例中对IsCloseing()的判断-它在第一个关闭事件中返回false，在第二个关闭事件中返回true(当 DoCloase 被调用后)。

如果browser是其他window的父窗口，则close事件将来源于父窗口的OS函数（如点击父窗口关闭按钮X）。接着父窗口需要调用CloseBrowser(false) 并等待第二个OS close事件来决策定browser是否被允许关闭。如果在JavaScript 'onbeforeunload'事件处理或者 DoClose()回调中取消了关闭操作，则操作系统的第二个关闭事件可能不会发送。注意下面例子用IsClosing()检查，这个函数在第一个OS close事件返回false，在第二个OS close事件中返回true(当 DoCloase 被调用后)。

Windows平台下，在父窗口的WndProc里处理WM_ClOSE消息：

```c++
case WM_CLOSE:
  if (g_handler.get() && !g_handler->IsClosing()) {
    CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();
    if (browser.get()) {
      // Notify the browser window that we would like to close it. This will result in a call to 
      // MyHandler::DoClose() if the JavaScript 'onbeforeunload' event handler allows it.
      browser->GetHost()->CloseBrowser(false);

      // Cancel the close.
      return 0;
    }
  }

  // Allow the close.
  break;

case WM_DESTROY:
  // Quitting CEF is handled in MyHandler::OnBeforeClose().
  return 0;
}
```

Linux平台下，处理`delete_event`信号:

```c++
gboolean delete_event(GtkWidget* widget, GdkEvent* event,
                      GtkWindow* window) {
  if (g_handler.get() && !g_handler->IsClosing()) {
    CefRefPtr<CefBrowser> browser = g_handler->GetBrowser();
    if (browser.get()) {
      // Notify the browser window that we would like to close it. This will result in a call to 
      // MyHandler::DoClose() if the JavaScript 'onbeforeunload' event handler allows it.
      browser->GetHost()->CloseBrowser(false);

      // Cancel the close.
      return TRUE;
    }
  }

  // Allow the close.
  return FALSE;
}
```

OS X下关闭会更复杂，查看 [cefsimple/cefsimple_mac.mm](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/cefsimple_mac.mm?at=master) 的注释来完整的理解在OS X平台下shotdown如何工作。

DoClose() 方法设置m_bIsClosing标志，返回false来发送第二个OS close事件.

```c++
bool MyClient::DoClose(CefRefPtr<CefBrowser> browser) {
  // Must be executed on the UI thread.
  REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed description of this
  // process.
  if (m_BrowserId == browser->GetIdentifier()) {
    // Set a flag to indicate that the window close should be allowed.
    m_bIsClosing = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}
```

当操作系统捕捉到第二次关闭事件，它才会允许父窗口真正关闭。该动作会先触发OnBeforeClose()回调，请在该回调里释放所有对浏览器对象的引用。

```c++
void MyHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  // Must be executed on the UI thread.
  REQUIRE_UI_THREAD();

  if (m_BrowserId == browser->GetIdentifier()) {
    // Free the browser pointer so that the browser can be destroyed.
    m_Browser = NULL;
  }

  if (--m_BrowserCount == 0) {
    // All browser windows have closed. Quit the application message loop.
    CefQuitMessageLoop();
  }
}
```

完整的流程，请参考cefclient例子里对不同平台的处理。

## Off-Screen Rendering

With off-screen rendering CEF does not create a native browser window. Instead, CEF provides the host application with invalidated regions and a pixel buffer and the host application notifies CEF of mouse, keyboard and focus events. Off-screen rendering does not currently support accelerated compositing so performance may suffer as compared to a windowed browser. Off-screen browsers will receive the same notifications as windowed browsers including the life span notifications described in the previous section. To use off-screen rendering:

1. Implement the [CefRenderHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRenderHandler.html) interface. All methods are required unless otherwise indicated.
2. Call CefWindowInfo::SetAsWindowless() before passing the CefWindowInfo structure to CefBrowserHost::CreateBrowser(). If no parent window is passed to SetAsWindowless some functionality like context menus may not be available.
3. The CefRenderHandler::GetViewRect() method will be called to retrieve the desired view rectangle.
4. The CefRenderHandler::OnPaint() method will be called to provide invalid regions and the updated pixel buffer. The cefclient application draws the buffer using OpenGL but your application can use whatever technique you prefer.
5. To resize the browser call CefBrowserHost::WasResized(). This will result in a call to GetViewRect() to retrieve the new size followed by a call to OnPaint().
6. Call the CefBrowserHost::SendXXX() methods to notify the browser of mouse, keyboard and focus events.
7. Call CefBrowserHost::CloseBrowser() to destroy browser.

Run cefclient with the “--off-screen-rendering-enabled” command-line flag for a working example.

# 投递任务(Posting Tasks)

Tasks可以在单一进程的多个线程中好似用CefPostTask函数族投递（完整列表查看 [include/cef_task.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_task.h?at=master) 头文件）。task将会在目标线程的消息循环中异步执行。

CEF提供base::Bind和base::Callback模版回调类来传递绑定的方法，对象和参数给CefPostTask。关于complete base::Bind和base::Callback 用法的完整信息查看 [include/base/cef_callback.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/base/cef_callback.h?at=master)头文件注释。 [include/wrapper/cef_closure_task.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_closure_task.h?at=master)头文件提供base::Closure到CefTask的转换，例如：

```c++
// Include the necessary headers.
#include “include/base/cef_bind.h”
#include “include/wrapper/cef_closure_task.h”

// To execute a bound function:

// Define a function.
void MyFunc(int arg) { /* do something with |arg| on the UI thread */ }

// Post a task that will execute MyFunc on the UI thread and pass an |arg|
// value of 5.
CefPostTask(TID_UI, base::Bind(&MyFunc, 5));

// To execute a bound method:

// Define a class.
class MyClass : public CefBase {
 public:
  MyClass() {}
  void MyMethod(int arg) { /* do something with |arg| on the UI thread */ }
 private:
  IMPLEMENT_REFCOUNTING(MyClass);
};

// Create an instance of MyClass.
CefRefPtr<MyClass> instance = new MyClass();

// Post a task that will execute MyClass::MyMethod on the UI thread and pass
// an |arg| value of 5. |instance| will be kept alive until after the task
// completes.
CefPostTask(TID_UI, base::Bind(&MyClass::MyMethod, instance, 5));
```

如果host应用程序需要保持run loop的引用，可以使用CefTaskRunner 类。例如，获取UI线程的task runner：

```c++
CefRefPtr<CefTaskRunner> task_runner = CefTaskRunner::GetForThread(TID_UI);
```

# 进程间通信(Inter-Process Communication (IPC))

CEF3运行在多进程中，所以需要在这些进程中提供消息通信机制。CefBrowser和CefFrame对象同时存在于browser和render进程中，which helps to facilitate this process。每一个CefBrowser和CefFrame对象有一个与之关联的唯一ID，用来匹配进程两边的对象。

## 进程启动消息(Process Startup Messages)

在browser进程中，通过`CefBrowserProcessHandler::OnRenderProcessThreadCreated()`可以为所有render进程提供相同的启动时信息。这个将会把信息传递给render进程的`CefRenderProcessHandler::OnRenderThreadCreated()` 。

## 进程运行时消息(Process Runtime Messages)

通过`CefProcessMessage `类可以在进程生命周期中的任意时间内使用进程消息传递信息。进程消息关联与一个指定的`CefBrowser`实例，并通过`CefBrowser::SendProcessMessage() `方法发送。进程消息通过`CefProcessMessage::GetArgumentList()`来包含任何需要的状态消息。

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

从browser进程发往render进程的消息将会在`CefRenderProcessHandler::OnProcessMessageReceived()`被接收。

从render进程发往browser进程的消息将会在`CefClient::OnProcessMessageReceived()`被接收。

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

## 异步JavaScript绑定(Asynchronous JavaScript Bindings)

[JavaScript集成](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md) 讲的是JavaScript绑定在render进程中的实现，但实际上JavaScript绑定经常需要和browser进程通信。`JavaScript`API本身就应该被设计成使用 [closures](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Closures) 和 [promises](http://www.html5rocks.com/en/tutorials/es6/promises/)进行异步工作的。

### 通用消息路由(Generic Message Router)

CEF提供一个通用的实现来路由JavaScript 和C++之间的异步消息路由，其中JavaScript 运行在renderer进程，C++运行在browser进程。应用程序通过标准CEF C++回调 (`OnBeforeBrowse`, `OnProcessMessageRecieved`, `OnContextCreated`, 等等)传递数据来与router交互。renderer这边的router支持一般的Javascript回调注册、执行，browser这边的router通过一个或多个的应用程序提供的Handler实例来支持特定的应用程序逻辑。查看 [message_router example](https://bitbucket.org/chromiumembedded/cef-project/src/master/examples/message_router/?at=master)这个演示`CefRouterMessage`使用方法的独立程序。查看 [include/wrapper/cef_message_router.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_message_router.h?at=master) 这个完整的使用文档。

### 自定义实现(Custom Implementation)

基于CEF的应用程序还可以提供自定义的异步JavaScript绑定实现。下面是一个简单的实现步骤：

1. render进程的JavaScript绑定传递一个回调函数

```javascript
// In JavaScript register the callback function.
app.setMessageCallback('binding_test', function(name, args) {
  document.getElementById('result').value = "Response: "+args[0];
});
```

2. render进程保存回调函数的引用

```c++
// Map of message callbacks.
typedef std::map<std::pair<std::string, int>,
                 std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value> > >
                 CallbackMap;
CallbackMap callback_map_;

// In the CefV8Handler::Execute implementation for “setMessageCallback”.
if (arguments.size() == 2 && arguments[0]->IsString() &&
    arguments[1]->IsFunction()) {
  std::string message_name = arguments[0]->GetStringValue();
  CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
  int browser_id = context->GetBrowser()->GetIdentifier();
  callback_map_.insert(
      std::make_pair(std::make_pair(message_name, browser_id),
                     std::make_pair(context, arguments[1])));
}
```

3. render进程发送异步IPC消息给browser进程，请求执行该动作。
4. browser进程接受IPC消息并执行动作。
5. 动作完成之后browser进程发送一个异步IPC消息给render进程，并返回结果。
6. render进程接收到IPC消息并执行回调函数，返回结果。

```c++
// Execute the registered JavaScript callback if any.
if (!callback_map_.empty()) {
  const CefString& message_name = message->GetName();
  CallbackMap::const_iterator it = callback_map_.find(
      std::make_pair(message_name.ToString(),
                     browser->GetIdentifier()));
  if (it != callback_map_.end()) {
    // Keep a local reference to the objects. The callback may remove itself
    // from the callback map.
    CefRefPtr<CefV8Context> context = it->second.first;
    CefRefPtr<CefV8Value> callback = it->second.second;

    // Enter the context.
    context->Enter();

    CefV8ValueList arguments;

    // First argument is the message name.
    arguments.push_back(CefV8Value::CreateString(message_name));

    // Second argument is the list of message arguments.
    CefRefPtr<CefListValue> list = message->GetArgumentList();
    CefRefPtr<CefV8Value> args = CefV8Value::CreateArray(list->GetSize());
    SetList(list, args);  // Helper function to convert CefListValue to CefV8Value.
    arguments.push_back(args);

    // Execute the callback.
    CefRefPtr<CefV8Value> retval = callback->ExecuteFunction(NULL, arguments);
    if (retval.get()) {
      if (retval->IsBool())
        handled = retval->GetBoolValue();
    }

    // Exit the context.
    context->Exit();
  }
}

```

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

## 同步请求(Synchronous Requests)

在少数情况下，可能需要在browser进程和render进程之间实现同步通信。这应该尽可能避免，因为它会对渲染过程中的性能产生负面影响。如果必须要使用同步通信，考虑使用[同步XMLHttpRequests](https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/Synchronous_and_Asynchronous_Requests) ，这将在browser进程在网络层等待处理时阻塞render进程。browser进程可以使用自定义的scheme handler或者网络拦截器处理请求，具体信息查看"Network Layer"章节。

# 网络层(Network Layer)

默认情况下，CEF3的网络请求会被宿主程序手工处理。然而CEF3也暴露了一系列网络相关的函数用以处理网络请求。

网络相关的回调函数可在不同线程被调用，因此要注意相关文档的说明，并对自己的数据进行线程安全保护。

## 自定义请求(Custom Requests)

通过CefFrame::LoadURL()方法可以简单的实现在browser frame内加载一个URL。

```c++
browser->GetMainFrame()->LoadURL(some_url);
```

应用程序如果希望发送包含自定义请求头或者上传数据的复杂请求可以使用 CefFrame::LoadRequest() 方法。这个方法接收一个 [CefRequest](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequest.html) 对象作为参数。

```c++
// Create a CefRequest object.
CefRefPtr<CefRequest> request = CefRequest::Create();

// Set the request URL.
request->SetURL(some_url);

// Set the request method. Supported methods include GET, POST, HEAD, DELETE and PUT.
request->SetMethod("POST");

// Optionally specify custom headers.
CefRequest::HeaderMap headerMap;
headerMap.insert(
    std::make_pair("X-My-Header", "My Header Value"));
request->SetHeaderMap(headerMap);

// Optionally specify upload content.
// The default “Content-Type” header value is "application/x-www-form-urlencoded".
// Set “Content-Type” via the HeaderMap if a different value is desired.
const std::string& upload_data = "arg1=val1&arg2=val2";
CefRefPtr<CefPostData> postData = CefPostData::Create();
CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
element->SetToBytes(upload_data.size(), upload_data.c_str());
postData->AddElement(element);
request->SetPostData(postData);
```

### 浏览器无关请求(Browser-Independent Requests)

应用程序可以通过[CefURLRequest](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefURLRequest.html) 类发送与浏览器无关的请求。实现[CefURLRequestClient](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefURLRequestClient.html) 接口来处理回复消息。CefURLRequest 可以在browser和render进程中使用。

```c++
class MyRequestClient : public CefURLRequestClient {
 public:
  MyRequestClient()
    : upload_total_(0),
      download_total_(0) {}

  virtual void OnRequestComplete(CefRefPtr<CefURLRequest> request) OVERRIDE {
    CefURLRequest::Status status = request->GetRequestStatus();
    CefURLRequest::ErrorCode error_code = request->GetRequestError();
    CefRefPtr<CefResponse> response = request->GetResponse();

    // Do something with the response...
  }

  virtual void OnUploadProgress(CefRefPtr<CefURLRequest> request,
                                uint64 current,
                                uint64 total) OVERRIDE {
    upload_total_ = total;
  }

  virtual void OnDownloadProgress(CefRefPtr<CefURLRequest> request,
                                  uint64 current,
                                  uint64 total) OVERRIDE {
    download_total_ = total;
  }

  virtual void OnDownloadData(CefRefPtr<CefURLRequest> request,
                              const void* data,
                              size_t data_length) OVERRIDE {
    download_data_ += std::string(static_cast<const char*>(data), data_length);
  }

 private:
  uint64 upload_total_;
  uint64 download_total_;
  std::string download_data_;

 private:
  IMPLEMENT_REFCOUNTING(MyRequestClient);
};
```

发送请求：

```c++
// Set up the CefRequest object.
CefRefPtr<CefRequest> request = CefRequest::Create();
// Populate |request| as shown above...

// Create the client instance.
CefRefPtr<MyRequestClient> client = new MyRequestClient();

// Start the request. MyRequestClient callbacks will be executed asynchronously.
CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, client.get());
// To cancel the request: url_request->Cancel();
```

CefURLRequest 定义的请求可以通过CefRequest::SetFlags()来指定一些自定义行为。支持的flags包括：

- **UR_FLAG_SKIP_CACHE** 请求响应时，缓存将被跳过
- **UR_FLAG_ALLOW_CACHED_CREDENTIALS** 能会发送cookie并在响应端被保存，UR_FLAG_ALLOW_CACHED_CREDENTIALS也必须被设置。
- **UR_FLAG_REPORT_UPLOAD_PROGRESS** 当请求拥有body时上传处理时间会被创建。
- **UR_FLAG_NO_DOWNLOAD_DATA** CefURLRequestClient::OnDownloadData方法不会被调用。
- **UR_FLAG_NO_RETRY_ON_5XX**5xx重定向错误会被交给相关Observer去处理，而不是自动重试。这个功能目前只能在Browser进程的请求端使用。

例如，跳过缓存并不报告下载数据：

```c++
request->SetFlags(UR_FLAG_SKIP_CACHE | UR_FLAG_NO_DOWNLOAD_DATA);
```

## 请求处理(Request Handling)

CEF3支持两种处理程序内网络请求的方法。一种是scheme handler，它允许为特定的请求(scheme+domain)注册响应。另一种是request interception，允许应用程序自行处理任意请求。 

**使用HTTP scheme而不是自定义scheme，以避免一系列潜在问题。**

注册自定义scheme（有别于"HTTP","HTTPS"等）可以让CEF按希望的方式处理请求。如果希望自定义的scheme能有类似于HTTP的行为（支持POST请求和执行[HTTP access control (CORS)](https://developer.mozilla.org/en-US/docs/HTTP/Access_control_CORS) 限制），则需要注册为"standard"scheme。如果需要执行到其他schemes的跨域请求或者通过XMLHttpRequest发送POST请求给scheme handler。则需要使用HTTP scheme而不是自定义scheme，以避免 [潜在的问题](https://bitbucket.org/chromiumembedded/cef/issue/950)。如果希望使用自定义schemes，所有进程都要实现CefApp::OnRegisterCustomSchemes() 回调。

```c++
void MyApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) {
  // Register "client" as a standard scheme.
  registrar->AddCustomScheme("client", true, ...);
}
```

### 通用资源管理器(Generic Resource Manager)

CEF提供为从来自一个或多个数据源的资源请求提供了一个通用的实现。用户为不同的资源注册handlers，例如磁盘目录，zip压缩文件或者自定义文件，以及处理请求的管理器。应用程序通过传递数据给标准CEF C++回调来与router交互。可以[resource_manager example](https://bitbucket.org/chromiumembedded/cef-project/src/master/examples/resource_manager/?at=master)在查看CefResourceManager 的用法演示，完整用法可以查看 [include/wrapper/cef_resource_manager.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_resource_manager.h?at=master)文档。

### Scheme处理(Scheme Handler)

scheme handler通过 CefRegisterSchemeHandlerFactory()函数注册，建议在CefBrowserProcessHandler::OnContextInitialized()函数调用。例如可以注册一个"client://myapp"请求的handler：

```c++
CefRegisterSchemeHandlerFactory("client", “myapp”, new MySchemeHandlerFactory());
```

handler可以用于内建schemes(HTTP, HTTPS等等)。当使用内建scheme时为应用选择一个特定的域名（如"myapp"或"internal"）。实现 [CefSchemeHandlerFactory](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefSchemeHandlerFactory.html) 和[CefResourceHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefResourceHandler.html)类来处理请求，并提供回复数据。如果使用自定义scheme，别忘了实现 CefApp::OnRegisterCustomSchemes方法。在 [scheme_handler example](https://bitbucket.org/chromiumembedded/cef-project/src/master/examples/scheme_handler/?at=master) 查看CefSchemeHandlerFactory 的用法演示。完整用法查看 [include/cef_scheme.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_scheme.h?at=master)文档。

如果回复数据类型是已知的，可以使用 [CefStreamResourceHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefStreamResourceHandler.html) 类提供的默认CefResourceHandler实现。

```c++
// CefStreamResourceHandler is part of the libcef_dll_wrapper project.
#include "include/wrapper/cef_stream_resource_handler.h"

const std::string& html_content = “<html><body>Hello!</body></html>”;

// Create a stream reader for |html_content|.
CefRefPtr<CefStreamReader> stream =
    CefStreamReader::CreateForData(
        static_cast<void*>(const_cast<char*>(html_content.c_str())),
        html_content.size());

// Constructor for HTTP status code 200 and no custom response headers.
// There’s also a version of the constructor for custom status code and response headers.
return new CefStreamResourceHandler("text/html", stream);
```

### 请求拦截(Request Interception)

CefRequestHandler::GetResourceHandler() 方法支持任意请求的拦截。它与scheme handler使用相同的CefResourceHandler 类。如果使用自定义scheme，别忘记实现CefApp::OnRegisterCustomSchemes方法：

```c++
CefRefPtr<CefResourceHandler> MyHandler::GetResourceHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request) {
  // Evaluate |request| to determine proper handling...
  if (...)
    return new MyResourceHandler();

  // Return NULL for default handling of the request.
  return NULL;
}
```

### 回复过滤(Response Filtering)

CefRequestHandler::GetResourceResponseFilter() 方法支持过滤请求回复数据。查看 [cefclient/browser/response_filter_test.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/browser/response_filter_test.cc?at=master)  的例子。

## 其他回调(Other Callbacks)

 [CefRequestHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequestHandler.html) 接口提供各种网络事件相关的回调，包括认证，cookie处理，外部协议处理，证书错误等。

## 代理解析(Proxy Resolution)

CEF3使用类似Google Chrome一样的方式，通过命令行参数传递代理配置。

```yaml
--proxy-server=host:port
      Specify the HTTP/SOCKS4/SOCKS5 proxy server to use for requests. An individual proxy
      server is specified using the format:

        [<proxy-scheme>://]<proxy-host>[:<proxy-port>]

      Where <proxy-scheme> is the protocol of the proxy server, and is one of:

        "http", "socks", "socks4", "socks5".

      If the <proxy-scheme> is omitted, it defaults to "http". Also note that "socks" is equivalent to
      "socks5".

      Examples:

        --proxy-server="foopy:99"
            Use the HTTP proxy "foopy:99" to load all URLs.

        --proxy-server="socks://foobar:1080"
            Use the SOCKS v5 proxy "foobar:1080" to load all URLs.

        --proxy-server="sock4://foobar:1080"
            Use the SOCKS v4 proxy "foobar:1080" to load all URLs.

        --proxy-server="socks5://foobar:66"
            Use the SOCKS v5 proxy "foobar:66" to load all URLs.

      It is also possible to specify a separate proxy server for different URL types, by prefixing
      the proxy server specifier with a URL specifier:

      Example:

        --proxy-server="https=proxy1:80;http=socks4://baz:1080"
            Load https://* URLs using the HTTP proxy "proxy1:80". And load http://*
            URLs using the SOCKS v4 proxy "baz:1080".

--no-proxy-server
      Disables the proxy server.

--proxy-auto-detect
      Autodetect  proxy  configuration.

--proxy-pac-url=URL
      Specify proxy autoconfiguration URL.
```

如果代理请求认证，CefRequestHandler::GetAuthCredentials()回调会被调用。如果isProxy参数为true，则需要返回用户名和密码。

```c++
bool MyHandler::GetAuthCredentials(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    bool isProxy,
    const CefString& host,
    int port,
    const CefString& realm,
    const CefString& scheme,
    CefRefPtr<CefAuthCallback> callback) {
  if (isProxy) {
    // Provide credentials for the proxy server connection.
    callback->Continue("myuser", "mypass");
    return true;
  }
  return false;
}
```

网络内容加载可能会因为代理而有延迟。为了更好的用户体验，可以考虑让你的应用程序先显示一个闪屏，等内容加载好了再通过[meta refresh](http://en.wikipedia.org/wiki/Meta_refresh) 显示真实网页。可以指定`--no-proxy-server`禁用代理并做相关测试。代理延迟也可以通过chrome浏览器重现，方式是使用命令行传参：`chrome -url=...`。 