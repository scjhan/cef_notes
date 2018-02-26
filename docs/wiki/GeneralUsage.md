This Wiki page provides an overview of CEF3 and general usage information.

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

# Getting Started

## Using a Binary Distribution

Binary distributions of CEF3 are available from the [project Downloads page](http://www.magpcss.net/cef_downloads/). They contain all files necessary to build a particular version of CEF3 on a particular platform (Windows, Mac OS X or Linux). No matter the platform they all share the same general structure:

- **cefclient** contains the cefclient sample application configured to build using the files in the binary distribution. This application demonstrates a wide range of CEF functionalities.
- **cefsimple** contains the cefsimple sample application configured to build using the files in the binary distribution. This application demonstrates the minimal functionality required to create a browser window.
- **Debug** contains a debug build the CEF shared library (libcef) and any other libraries required to run on the platform. This application demonstrates the minimal functionality required to create a browser window.
- **include** contains all required CEF header files.
- **libcef_dll** contains the source code for the libcef_dll_wrapper static library that all applications using the CEF C++ API must link against. See the “C++ Wrapper” section for more information.
- **Release** contains a release build the CEF shared library (libcef) and any other libraries required to run on the platform.
- **Resources** contains resources required by applications using CEF (Windows and Linux only). This includes .pak files (binary files with globbed resources) and potentially other files such depending on the platform.

Each binary distribution also contains a README.txt file that describes the platform-specific distribution in greater detail and a LICENSE.txt file that contains CEF’s BSD license. When distributing an application based on CEF you should include the license text somewhere in your application’s distribution. For example, you can list it on an “About” or “Credits” page in your application’s UI, or in the documentation bundled with your application. License and credit information is also available inside of a CEF3 browser window by loading “about:license” and “about:credits” respectively.

Applications based on CEF binary distributions can be built using standard platform build tools. This includes Visual Studio on Windows, Xcode on Mac OS X and gcc/make on Linux. The project Downloads page contains information about the OS and build tool versions required for specific binary releases. When building on Linux also pay careful attention to the listed package dependencies.

See the [Tutorial](https://bitbucket.org/chromiumembedded/cef/wiki/Tutorial.md) Wiki page for detailed instructions on how to create a simple application using the CEF3 binary distribution.

## Building from Source Code

CEF can be built from source code either locally or using automated build systems like [TeamCity](http://www.jetbrains.com/teamcity/). This requires the download of Chromium and CEF source code via Git. The Chromium code base is quite large and building Chromium from source code is only recommended on moderately powerful machines with more than 6GB of RAM. Detailed instructions for building Chromium and CEF from source code are available on the [BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md) Wiki page.

# Sample Application

The cefclient sample application is a complete working example of CEF integration and is included in source code form with each binary distribution. The easiest way to create a new application using CEF is to start with the cefclient application and remove the parts that you don’t need. Many of the examples in this document originate from the cefclient application.

# Important Concepts

There are some important underlying concepts to developing CEF3-based applications that should be understood before proceeding.

## C++ Wrapper

The libcef shared library exports a C API that isolates the user from the CEF runtime and code base. The libcef_dll_wrapper project, which is distributed in source code form as part of the binary release, wraps this exported C API in a C++ API that is then linked into the client application. The code for this C/C++ API translation layer is automatically generated by the [translator](https://bitbucket.org/chromiumembedded/cef/src/master/tools/translator.README.txt?at=master) tool. Direct usage of the C API is described on the [UsingTheCAPI](https://bitbucket.org/chromiumembedded/cef/wiki/UsingTheCAPI.md) page.

## Processes

CEF3 runs using multiple processes. The main process which handles window creation, painting and network access is called the “browser” process. This is generally the same process as the host application and the majority of the application logic will run in the browser process. Blink rendering and JavaScript execution occur in a separate “render” process. Some application logic, such as JavaScript bindings and DOM access, will also run in the render process. The default [process model](http://www.chromium.org/developers/design-documents/process-models) will spawn a new render process for each unique origin (scheme + domain). Other processes will be spawned as needed, such as “plugin” processes to handle [plugins](http://www.chromium.org/developers/design-documents/plugin-architecture) like Flash and “gpu” processes to handle [accelerated compositing](http://www.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome).

By default the main application executable will be spawned multiple times to represent separate processes. This is handled via command-line flags that are passed into the CefExecuteProcess function. If the main application executable is large, takes a long time to load, or is otherwise unsuitable for non-browser processes the host can use a separate executable for those other processes. This can be configured via the CefSettings.browser_subprocess_path variable. See the “Application Structure” section for more information.

The separate processes spawned by CEF3 communicate using Inter-Process Communication (IPC). Application logic implemented in the browser and render processes can communicate by sending asynchronous messages back and forth. [JavaScriptIntegration](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md) in the render process can expose asynchronous APIs that are handled in the browser process. See the “Inter-Process Communication” section for more information.

Platform-specific debugging tips are also available for [Windows](https://www.chromium.org/developers/how-tos/debugging-on-windows), [Mac OS X](http://www.chromium.org/developers/how-tos/debugging-on-os-x) and [Linux](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_debugging.md).

## Threads

Each process in CEF3 runs multiple threads. For a complete list of threads see the [cef_thread_id_t](http://magpcss.org/ceforum/apidocs3/projects/(default)/cef_thread_id_t.html) enumeration. These are some of the commonly-referenced threads:

- **TID_UI** thread is the main thread in the browser process. This thread will be the same as the main application thread if CefInitialize() is called with a CefSettings.multi_threaded_message_loop value of false.
- **TID_IO** thread is used in the browser process to handle IPC and network messages.
- **TID_FILE** thread is used in the browser process to interact with the file system. Blocking operations should only be performed on this thread or a [CefThread](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefThread.html) created by the client application.
- **TID_RENDERER** thread is the main thread in the renderer process. All Blink and V8 interation must take place on this thread.

Due to the multi-threaded nature of CEF it’s important to use message passing or locking to protect data members from access on multiple threads. The CefPostTask family of functions support easy asynchronous message passing between threads. See the “Posting Tasks” section for more information.

The current thread can be verified using the CefCurrentlyOn() function. The CEF sample applications use the following defines to verify that methods are executed on the expected thread. These defines are included in the [include/wrapper/cef_helpers.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_helpers.h?at=master) header file.

```
#define CEF_REQUIRE_UI_THREAD()       DCHECK(CefCurrentlyOn(TID_UI));
#define CEF_REQUIRE_IO_THREAD()       DCHECK(CefCurrentlyOn(TID_IO));
#define CEF_REQUIRE_FILE_THREAD()     DCHECK(CefCurrentlyOn(TID_FILE));
#define CEF_REQUIRE_RENDERER_THREAD() DCHECK(CefCurrentlyOn(TID_RENDERER));

```

To support synchronized access to blocks of code CEF provides base::Lock and base::AutoLock types via the [include/base/cef_lock.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/base/cef_lock.h?at=master) header file. For example:

```
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

## Reference Counting

All framework classes implement the CefBase interface and all instance pointers are handled using the CefRefPtr smart pointer implementation that automatically handles reference counting via calls to AddRef() and Release().The easiest way to implement these classes is as follows:

```
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

## Strings

CEF defines its own data structure for representing strings. This is for a few different reasons:

- The libcef library and the host application may use different runtimes for managing heap memory. All objects, including strings, need to be freed using the same runtime that allocated the memory.
- The libcef library can be compiled to support different underlying string types (UTF8, UTF16 or wide). The default is UTF16 but it can be changed by modifying the defines in [cef_string.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/internal/cef_string.h?at=master) and recompiling CEF. When choosing the wide string type keep in mind that the size will vary depending on the platform.

For UTF16 the string structure looks like this:

```
typedef struct _cef_string_utf16_t {
  char16* str;  // Pointer to the string
  size_t length;  // String length
  void (*dtor)(char16* str);  // Destructor for freeing the string on the correct heap
} cef_string_utf16_t;

```

The selected string type is then typedef’d to the generic type:

```
typedef char16 cef_char_t;
typedef cef_string_utf16_t cef_string_t;

```

CEF provides a number of C API functions for operating on the CEF string types (mapped via #defines to the type-specific functions). For example:

- **cef_string_set** will assign a string value to the structure with or without copying the value.
- **cef_string_clear** will clear the string value.
- **cef_string_cmp** will compare two string values.

CEF also provides functions for converting between all supported string types (ASCII, UTF8, UTF16 and wide). See the [cef_string.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/internal/cef_string.h?at=master) and [cef_string_types.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/internal/cef_string_types.h?at=master) headers for the complete list of functions.

Usage of CEF strings in C++ is simplified by the CefString class. CefString provides automatic conversion to and from std::string (UTF8) and std::wstring (wide) types. It can also be used to wrap an existing cef_string_t structure for assignment purposes.

Assignment to and from std::string:

```
std::string str = “Some UTF8 string”;

// Equivalent ways of assigning |str| to |cef_str|. Conversion from UTF8 will occur if necessary.
CefString cef_str(str);
cef_str = str;
cef_str.FromString(str);

// Equivalent ways of assigning |cef_str| to |str|. Conversion to UTF8 will occur if necessary.
str = cef_str;
str = cef_str.ToString();

```

Assignment to and from std::wstring:

```
std::wstring str = “Some wide string”;

// Equivalent ways of assigning |str| to |cef_str|. Conversion from wide will occur if necessary.
CefString cef_str(str);
cef_str = str;
cef_str.FromWString(str);

// Equivalent ways of assigning |cef_str| to |str|. Conversion to wide will occur if necessary.
str = cef_str;
str = cef_str.ToWString();

```

Use the FromASCII() method if you know that the format is ASCII:

```
const char* cstr = “Some ASCII string”;
CefString cef_str;
cef_str.FromASCII(cstr);

```

Some structures like CefSettings have cef_string_t members. CefString can be used to simplify the assignment to those members:

```
CefSettings settings;
const char* path = “/path/to/log.txt”;

// Equivalent assignments.
CefString(&settings.log_file).FromASCII(path);
cef_string_from_ascii(path, strlen(path), &settings.log_file);

```

## Command Line Arguments

Many features in CEF3 and Chromium can be configured using command line arguments. These arguments take the form "--some-argument[=optional-param]" and are passed into CEF via CefExecuteProcess() and the CefMainArgs structure (see the “Application Structure” section below).

- To disable processing of arguments from the command line set CefSettings.command_line_args_disabled to true before passing the CefSettings structure into CefInitialize().
- To specify CEF/Chromium command line arguments inside the host application implement the CefApp::OnBeforeCommandLineProcessing() method.
- To pass application-specific (non-CEF/Chromium) command line arguments to sub-processes implement the CefBrowserProcessHandler::OnBeforeChildProcessLaunch() method.

See comments in [shared/common/client_switches.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/shared/common/client_switches.cc?at=master) for more information on how to discover supported CEF/Chromium command line switches.

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

# Application Structure

Every CEF3 application has the same general structure.

- Provide an entry-point function that initializes CEF and runs either sub-process executable logic or the CEF message loop.
- Provide an implementation of [CefApp](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefApp.html) to handle process-specific callbacks.
- Provide an implementation of [CefClient](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefClient.html) to handle browser-instance-specific callbacks.
- Call CefBrowserHost::CreateBrowser() to create a browser instance and manage the browser life span using [CefLifeSpanHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html).

## Entry-Point Function

As described in the “Processes” section a CEF3 application will run multiple processes. The processes can all use the same executable or a separate executable can be specified for the sub-processes. Execution of the process begins in the entry-point function. Complete platform-specific examples for Windows, Linux and Mac OS-X are available in [cefclient/cefclient_win.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/cefclient_win.cc?at=master), [cefclient/cefclient_gtk.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/cefclient_gtk.cc?at=master) and [cefclient/cefclient_mac.mm](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/cefclient_mac.mm?at=master) respectively.

When launching sub-processes CEF will specify configuration information using the command-line that must be passed into the CefExecuteProcess function via the CefMainArgs structure. The definition of CefMainArgs is platform-specific. On Linux and Mac OS X it accepts the argc and argv values which are passed into the [main() function](https://en.wikipedia.org/wiki/Main_function).

```
CefMainArgs main_args(argc, argv);

```

On Windows it accepts the instance handle (HINSTANCE) which is passed into the [wWinMain() function](http://msdn.microsoft.com/en-us/library/windows/desktop/ff381406(v=vs.85).aspx). The instance handle is also retrievable via GetModuleHandle(NULL).

```
CefMainArgs main_args(hInstance);

```

### Single Executable

When running as a single executable the entry-point function is required to differentiate between the different process types. The single executable structure is supported on Windows and Linux but not on Mac OS X.

```
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

### Separate Sub-Process Executable

When using a separate sub-process executable you need two separate executable projects and two separate entry-point functions.

Main application entry-point function:

```
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
  CefString(&settings.browser_subprocess_path).FromASCII(“/path/to/subprocess”);

  // Initialize CEF in the main process.
  CefInitialize(main_args, settings, app.get());

  // Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
  CefRunMessageLoop();

  // Shut down CEF.
  CefShutdown();

  return 0;
}

```

Sub-process application entry-point function:

```
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

## Message Loop Integration

CEF can also integrate with an existing application message loop instead of running its own message loop. There are two ways to do this.

1. Call CefDoMessageLoopWork() on a regular basis instead of calling CefRunMessageLoop(). Each call to CefDoMessageLoopWork() will perform a single iteration of the CEF message loop. Caution should be used with this approach. Calling the method too infrequently will starve the CEF message loop and negatively impact browser performance. Calling the method too frequently will negatively impact CPU usage.
2. Set CefSettings.multi_threaded_message_loop = true (Windows only). This will cause CEF to run the browser UI thread on a separate thread from the main application thread. With this approach neither CefDoMessageLoopWork() nor CefRunMessageLoop() need to be called. CefInitialize() and CefShutdown() should still be called on the main application thread. You will need to provide your own mechanism for communicating with the main application thread (see for example the message window usage in cefclient_win.cpp). You can test this mode in cefclient on Windows by running with the “--multi-threaded-message-loop” command-line flag.

## CefSettings

The CefSettings structure allows configuration of application-wide CEF settings. Some commonly configured members include:

- **single_process** Set to true to use a single process for the browser and renderer. Also configurable using the "single-process" command-line switch. See the “Processes” section for more information.
- **browser_subprocess_path** The path to a separate executable that will be launched for sub-processes. See the “Separate Sub-Process Executable” section for more information.
- **multi_threaded_message_loop** Set to true to have the browser process message loop run in a separate thread. See the “Message Loop Integration” section for more information.
- **command_line_args_disabled** Set to true to disable configuration of browser process features using standard CEF and Chromium command-line arguments. See the “Command Line Arguments” section for more information.
- **cache_path** The location where cache data will be stored on disk. If empty an in-memory cache will be used for some features and a temporary disk cache will be used for others. HTML5 databases such as localStorage will only persist across sessions if a cache path is specified.
- **locale** The locale string that will be passed to Blink. If empty the default locale of "en-US" will be used. This value is ignored on Linux where locale is determined using environment variable parsing with the precedence order: LANGUAGE, LC_ALL, LC_MESSAGES and LANG. Also configurable using the "lang" command-line switch.
- **log_file** The directory and file name to use for the debug log. If empty, the default name of "debug.log" will be used and the file will be written to the application directory. Also configurable using the "log-file" command-line switch.
- **log_severity** The log severity. Only messages of this severity level or higher will be logged. Also configurable using the "log-severity" command-line switch with a value of "verbose", "info", "warning", "error", "error-report" or "disable".
- **resources_dir_path** The fully qualified path for the resources directory. If this value is empty the cef.pak and/or devtools_resources.pak files must be located in the module directory on Windows/Linux or the app bundle Resources directory on Mac OS X. Also configurable using the "resources-dir-path" command-line switch.
- **locales_dir_path** The fully qualified path for the locales directory. If this value is empty the locales directory must be located in the module directory. This value is ignored on Mac OS X where pack files are always loaded from the app bundle Resources directory. Also configurable using the "locales-dir-path" command-line switch.
- **remote_debugging_port** Set to a value between 1024 and 65535 to enable remote debugging on the specified port. For example, if 8080 is specified the remote debugging URL will be [http://localhost:8080](http://localhost:8080/). CEF can be remotely debugged from any CEF or Chrome browser window. Also configurable using the "remote-debugging-port" command-line switch.

## CefBrowser and CefFrame

The [CefBrowser](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefBrowser.html) and [CefFrame](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefFrame.html) objects are used for sending commands to the browser and for retrieving state information in callback methods. Each CefBrowser object will have a single main CefFrame object representing the top-level frame and zero or more CefFrame objects representing sub-frames. For example, a browser that loads two iframes will have three CefFrame objects (the top-level frame and the two iframes).

To load a URL in the browser main frame:

```
browser->GetMainFrame()->LoadURL(some_url);

```

To navigate the browser backwards:

```
browser->GoBack();

```

To retrieve the main frame HTML contents:

```
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

CefBrowser and CefFrame objects exist in both the browser process and the render process. Host behavior can be controlled in the browser process via the CefBrowser::GetHost() method. For example, the native handle for a windowed browser can be retrieved as follows:

```
// CefWindowHandle is defined as HWND on Windows, NSView* on Mac OS X
// and GtkWidget* on Linux.
CefWindowHandle window_handle = browser->GetHost()->GetWindowHandle();

```

Other methods are available for history navigation, loading of strings and requests, sending edit commands, retrieving text/html contents, and more. See the documentation for the complete list of supported methods.

## CefApp

The [CefApp](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefApp.html) interface provides access to process-specific callbacks. Important callbacks include:

- **OnBeforeCommandLineProcessing** which provides the opportunity to programmatically set command-line arguments. See the “Command Line Arguments” section for more information.
- **OnRegisterCustomSchemes** which provides an opportunity to register custom schemes. See the “”Request Handling” section for more information.
- **GetBrowserProcessHandler** which returns the handler for functionality specific to the browser process including the OnContextInitialized() method.
- **GetRenderProcessHandler** which returns the handler for functionality specific to the render process. This includes JavaScript-related callbacks and process messages. See the [JavaScriptIntegration](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md) Wiki page and the “Inter-Process Communication” section for more information.

An example CefApp implementation can be seen in [cefsimple/simple_app.h](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_app.h?at=master) and [cefsimple/simple_app.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_app.cc?at=master).

## CefClient

The [CefClient](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefClient.html) interface provides access to browser-instance-specific callbacks. A single CefClient instance can be shared among any number of browsers. Important callbacks include:

- Handlers for things like browser life span, context menus, dialogs, display notifications, drag events, focus events, keyboard events and more. The majority of handlers are optional. See the documentation in cef_client.h for the side effects, if any, of not implementing a specific handler.
- **OnProcessMessageReceived** which is called when an IPC message is received from the render process. See the “Inter-Process Communication” section for more information.

An example CefClient implementation can be seen in [cefsimple/simple_handler.h](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler.h?at=master) and [cefsimple/simple_handler.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/simple_handler.cc?at=master).

## Browser Life Span

Browser life span begins with a call to CefBrowserHost::CreateBrowser() or CefBrowserHost::CreateBrowserSync(). Convenient places to execute this logic include the CefBrowserProcessHandler::OnContextInitialized() callback or platform-specific message handlers like WM_CREATE on Windows.

```
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

The [CefLifeSpanHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefLifeSpanHandler.html) class provides the callbacks necessary for managing browser life span. Below is an extract of the relevant methods and members.

```
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

The OnAfterCreated() method will be called immediately after the browser object is created. The host application can use this method to keep a reference to the main browser object.

```
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

To destroy the browser call CefBrowserHost::CloseBrowser().

```
// Notify the browser window that we would like to close it. This will result in a call to 
// MyHandler::DoClose() if the JavaScript 'onbeforeunload' event handler allows it.
browser->GetHost()->CloseBrowser(false);

```

If the browser is parented to another window then the close event may originate in the OS function for that parent window (for example, by clicking the X on the parent window). The parent window then needs to call CloseBrowser(false) and wait for a second OS close event to indicate that the browser has allowed the close. The second OS close event will not be sent if the close is canceled by a JavaScript ‘onbeforeunload’ event handler or by the DoClose() callback. Notice the IsClosing() check in the below examples -- it will return false for the first OS close event and true for the second (after DoClose is called).

Handling in the parent window WndProc on Windows:

```
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

Handling the “delete_event” signal on Linux:

```
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

Shutdown on OS X is more complicated. See comments in [cefsimple/cefsimple_mac.mm](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefsimple/cefsimple_mac.mm?at=master) for a complete understanding of how shutdown works on that platform.

The DoClose() method sets the m_bIsClosing flag and returns false to send the second OS close event.

```
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

When the OS function receives the second OS close event it allows the parent window to actually close. This then results in a call to OnBeforeClose(). Make sure to release any references to the browser object in the OnBeforeClose() callback.

```
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

See the cefclient application for complete working examples on each platform.

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

# Posting Tasks

Tasks can be posted between the various threads in a single process using the CefPostTask family of methods (see the [include/cef_task.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_task.h?at=master) header file for the complete list). The task will execute asynchronously on the message loop of the target thread.

CEF provides base::Bind and base::Callback templated callback classes for passing bound methods, objects and arguments to CefPostTask. For complete base::Bind and base::Callback usage information see comments in the [include/base/cef_callback.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/base/cef_callback.h?at=master) header. The [include/wrapper/cef_closure_task.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_closure_task.h?at=master) header provides helpers for converting a base::Closure to a CefTask. For example:

```
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

If the host application needs to keep a reference to a run loop it can use the CefTaskRunner class. For example, to get the task runner for the UI thread:

```
CefRefPtr<CefTaskRunner> task_runner = CefTaskRunner::GetForThread(TID_UI);

```

# Inter-Process Communication (IPC)

Since CEF3 runs in multiple processes it is necessary to provide mechanisms for communicating between those processes. CefBrowser and CefFrame objects exist in both the browser and render processes which helps to facilitate this process. Each CefBrowser and CefFrame object also has a unique ID value associated with it that will match on both sides of the process boundary.

## Process Startup Messages

To provide all render processes with the same information on startup implement CefBrowserProcessHandler::OnRenderProcessThreadCreated() in the browser process. This will pass information to CefRenderProcessHandler::OnRenderThreadCreated() in the render process.

## Process Runtime Messages

To pass information at any time during the process lifespan use process messages via the CefProcessMessage class. These messages are associated with a specific CefBrowser instance and are sent using the CefBrowser::SendProcessMessage() method. The process message should contain whatever state information is required via CefProcessMessage::GetArgumentList().

```
// Create the message object.
CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create(“my_message”);

// Retrieve the argument list object.
CefRefPtr<CefListValue> args = msg>GetArgumentList();

// Populate the argument values.
args->SetString(0, “my string”);
args->SetInt(0, 10);

// Send the process message to the render process.
// Use PID_BROWSER instead when sending a message to the browser process.
browser->SendProcessMessage(PID_RENDERER, msg);

```

A message sent from the browser process to the render process will arrive in CefRenderProcessHandler::OnProcessMessageReceived(). A message sent from the render process to the browser process will arrive in CefClient::OnProcessMessageReceived().

```
bool MyHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  // Check the message name.
  const std::string& message_name = message->GetName();
  if (message_name == “my_message”) {
    // Handle the message here...
    return true;
  }
  return false;
}

```

To associate the message with a particular CefFrame pass the frame ID (retrievable via CefFrame::GetIdentifier()) as an argument and retrieve the associated CefFrame in the receiving process via the CefBrowser::GetFrame() method.

```
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

## Asynchronous JavaScript Bindings

[JavaScriptIntegration](https://bitbucket.org/chromiumembedded/cef/wiki/JavaScriptIntegration.md) is implemented in the render process but frequently need to communicate with the browser process. The JavaScript APIs themselves should be designed to work asynchronously using [closures](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Closures) and [promises](http://www.html5rocks.com/en/tutorials/es6/promises/).

### Generic Message Router

Javascript运行在renderer进程，C++运行在browser进程，两者之间CEF提供了一个异步消息路由的通用实现。应用程序通过标准CEF C++回调(OnBeforeBrowse, OnProcessMessageRecieved, OnContextCreated等)传递数据来与router交互。renderer这边的router支持一般的Javascript回调注册、执行，browser这边的router通过一个或多个的应用程序提供的Handler实例来支持特定的应用程序逻辑。

CEF provides a generic implementation for routing asynchronous messages between JavaScript running in the renderer process and C++ running in the browser process. An application interacts with the router by passing it data from standard CEF C++ callbacks (OnBeforeBrowse, OnProcessMessageRecieved, OnContextCreated, etc). The renderer-side router supports generic JavaScript callback registration and execution while the browser-side router supports application-specific logic via one or more application-provided Handler instances. See the [message_router example](https://bitbucket.org/chromiumembedded/cef-project/src/master/examples/message_router/?at=master) for a stand-alone example application that demonstates CefMessageRouter usage. See [include/wrapper/cef_message_router.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_message_router.h?at=master) for complete usage documentation.

### Custom Implementation

A CEF-based application can also provide its own custom implementation of asynchronous JavaScript bindings. A simplistic implementation could work as follows:

\1. The JavaScript binding in the render process is passed a callback function.

```
// In JavaScript register the callback function.
app.setMessageCallback('binding_test', function(name, args) {
  document.getElementById('result').value = "Response: "+args[0];
});

```

\2. The render process keeps a reference to the callback function.

```
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

\3. The render process sends an asynchronous IPC message to the browser process requesting that work be performed.

\4. The browser process receives the IPC message and performs the work.

\5. Upon completion of the work the browser process sends an asynchronous IPC message back to the render process with the result.

\6. The render process receives the IPC message and executes the callback function with the result.

```
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

\7. Release any V8 references associated with the context in CefRenderProcessHandler::OnContextReleased().

```
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

## Synchronous Requests

In rare cases it may be necessary to implement synchronous communication between the browser and render processes. This should be avoided whenever possible because it will negatively impact performance in the render process. However, if you must have synchronous communication consider using [synchronous XMLHttpRequests](https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/Synchronous_and_Asynchronous_Requests) which will block the render process while awaiting handling in the browser process network layer. The browser process can handle the requests using a custom scheme handler or network interception. See the “Network Layer” section for more information.

# Network Layer

By default network requests in CEF3 will be handled in a manner transparent to the host application. For applications wishing for a closer relationship with the network layer CEF3 exposes a range of network-related functionalities.

Network-related callbacks can occur on different threads so make sure to pay attention to the documentation and properly protect your data members.

## Custom Requests

The simplest way to load a URL in a browser frame is via the CefFrame::LoadURL() method.

```
browser->GetMainFrame()->LoadURL(some_url);

```

Applications wishing to send more complex requests containing custom request headers or upload data can use the CefFrame::LoadRequest() method. This method accepts a [CefRequest](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequest.html) object as the single argument.

```
// Create a CefRequest object.
CefRefPtr<CefRequest> request = CefRequest::Create();

// Set the request URL.
request->SetURL(some_url);

// Set the request method. Supported methods include GET, POST, HEAD, DELETE and PUT.
request->SetMethod(“POST”);

// Optionally specify custom headers.
CefRequest::HeaderMap headerMap;
headerMap.insert(
    std::make_pair("X-My-Header", "My Header Value"));
request->SetHeaderMap(headerMap);

// Optionally specify upload content.
// The default “Content-Type” header value is "application/x-www-form-urlencoded".
// Set “Content-Type” via the HeaderMap if a different value is desired.
const std::string& upload_data = “arg1=val1&arg2=val2”;
CefRefPtr<CefPostData> postData = CefPostData::Create();
CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
element->SetToBytes(upload_data.size(), upload_data.c_str());
postData->AddElement(element);
request->SetPostData(postData);

```

### Browser-Independent Requests

Applications can send network requests not associated with a particular browser via the [CefURLRequest](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefURLRequest.html) class. Implement the [CefURLRequestClient](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefURLRequestClient.html) interface to handle the resulting response. CefURLRequest can be used in both the browser and render processes.

```
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

To send the request:

```
// Set up the CefRequest object.
CefRefPtr<CefRequest> request = CefRequest::Create();
// Populate |request| as shown above...

// Create the client instance.
CefRefPtr<MyRequestClient> client = new MyRequestClient();

// Start the request. MyRequestClient callbacks will be executed asynchronously.
CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, client.get());
// To cancel the request: url_request->Cancel();

```

Requests made with CefURLRequest can also specify custom behaviors via the CefRequest::SetFlags() method. Supported bit flags include:

- **UR_FLAG_SKIP_CACHE** If set the cache will be skipped when handling the request.
- **UR_FLAG_ALLOW_CACHED_CREDENTIALS** If set cookies may be sent with the request and saved from the response. UR_FLAG_ALLOW_CACHED_CREDENTIALS must also be set.
- **UR_FLAG_REPORT_UPLOAD_PROGRESS** If set upload progress events will be generated when a request has a body.
- **UR_FLAG_NO_DOWNLOAD_DATA** If set the CefURLRequestClient::OnDownloadData method will not be called.
- **UR_FLAG_NO_RETRY_ON_5XX** If set 5XX redirect errors will be propagated to the observer instead of automatically re-tried. This currently only applies for requests originated in the browser process.

For example, to skip the cache and not report download data:

```
request->SetFlags(UR_FLAG_SKIP_CACHE | UR_FLAG_NO_DOWNLOAD_DATA);

```

## Request Handling

CEF3 supports two approaches for handling network requests inside of an application. The scheme handler approach allows registration of a handler for requests targeting a particular origin (scheme + domain). The request interception approach allows handling of arbitrary requests at the application’s discretion.

**Use the HTTP scheme instead of a custom scheme to avoid a range of potential issues.**

If you choose to use a custom scheme (anything other than “HTTP”, “HTTPS”, etc) you must register it with CEF so that it will behave as expected. If you would like your custom scheme to behave similar to HTTP (support POST requests and enforce [HTTP access control (CORS)](https://developer.mozilla.org/en-US/docs/HTTP/Access_control_CORS) restrictions) then it should be registered as a “standard” scheme. If you are planning to perform cross-origin requests to other schemes or send POST requests via XMLHttpRequest to your scheme handler then you should use the HTTP scheme instead of a custom scheme to avoid [potential issues](https://bitbucket.org/chromiumembedded/cef/issue/950). If you wish to use custom schemes the attributes are registered via the CefApp::OnRegisterCustomSchemes() callback which must be implemented in all processes.

```
void MyApp::OnRegisterCustomSchemes(CefRefPtr<CefSchemeRegistrar> registrar) {
  // Register "client" as a standard scheme.
  registrar->AddCustomScheme("client", true, ...);
}

```

### Generic Resource Manager

CEF provides a generic implementation for managing resource requests from one or more data sources. This user registers handlers for different data sources, such as directories on disk, zip archives or custom implementations, and the manager handles the requests. An application interacts with the router by passing it data from standard CEF C++ callbacks (OnBeforeResourceLoad, GetResourceHandler). See the [resource_manager example](https://bitbucket.org/chromiumembedded/cef-project/src/master/examples/resource_manager/?at=master) for a stand-alone example application that demonstates CefResourceManager usage. See [include/wrapper/cef_resource_manager.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/wrapper/cef_resource_manager.h?at=master) for complete usage documentation.

### Scheme Handler

A scheme handler is registered via the CefRegisterSchemeHandlerFactory() function. A good place to call this function is from CefBrowserProcessHandler::OnContextInitialized(). For example, you can register a handler for “client://myapp/” requests:

```
CefRegisterSchemeHandlerFactory("client", “myapp”, new MySchemeHandlerFactory());

```

Handlers can be used with both built-in schemes (HTTP, HTTPS, etc) and custom schemes. When using a built-in scheme choose a domain name unique to your application (like “myapp” or “internal”). Implement the [CefSchemeHandlerFactory](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefSchemeHandlerFactory.html) and [CefResourceHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefResourceHandler.html) classes to handle the request and provide response data. If using custom schemes don't forget to implement the CefApp::OnRegisterCustomSchemes method as described above. See the [scheme_handler example](https://bitbucket.org/chromiumembedded/cef-project/src/master/examples/scheme_handler/?at=master) for a stand-alone example application that demonstates CefSchemeHandlerFactory usage. See [include/cef_scheme.h](https://bitbucket.org/chromiumembedded/cef/src/master/include/cef_scheme.h?at=master) for complete usage documentation.

If the response data is known at request time the [CefStreamResourceHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefStreamResourceHandler.html) class provides a convenient default implementation of CefResourceHandler.

```
// CefStreamResourceHandler is part of the libcef_dll_wrapper project.
#include “include/wrapper/cef_stream_resource_handler.h”

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

### Request Interception

The CefRequestHandler::GetResourceHandler() method supports the interception of arbitrary requests. It uses the same CefResourceHandler class as the scheme handler approach. If using custom schemes don't forget to implement the CefApp::OnRegisterCustomSchemes method as described above.

```
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

### Response Filtering

The CefRequestHandler::GetResourceResponseFilter() method supports filtering of request response data. See [cefclient/browser/response_filter_test.cc](https://bitbucket.org/chromiumembedded/cef/src/master/tests/cefclient/browser/response_filter_test.cc?at=master) (accessible via the Tests menu > Other Tests > Response Filtering from inside the cefclient sample application) for a working example.

## Other Callbacks

The [CefRequestHandler](http://magpcss.org/ceforum/apidocs3/projects/(default)/CefRequestHandler.html) interface provides callbacks for various network-related events incuding authentication, cookie handling, external protocol handling, certificate errors and so on.

## Proxy Resolution

Proxy settings are configured in CEF3 using the same command-line flags as Google Chrome.

```
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

If the proxy requires authentication the CefRequestHandler::GetAuthCredentials() callback will be executed with an |isProxy| value of true to retrieve the username and password.

```
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

Web content loading during application startup can be delayed due to network proxy resolution (for example, if "Automatically detect proxy settings" is checked on Windows). For best user experience consider designing your application to first show a static splash page and then redirect to the actual website using [meta refresh](http://en.wikipedia.org/wiki/Meta_refresh) -- the redirect will be blocked until proxy resolution completes. For testing purposes proxy resolution can be disabled using the "--no-proxy-server" command-line flag. Proxy resolution delays can also be duplicated in Google Chrome by running "chrome --url=..." from the command line.