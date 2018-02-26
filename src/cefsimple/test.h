#ifndef CEF_TESTS_CEF_SIMPLE_TEST_H_
#define CEF_TESTS_CEF_SIMPLE_TEST_H_
#pragma once

#include "include/cef_browser.h"
#include "include/cef_urlrequest.h"
#include "include/wrapper/cef_helpers.h"

#include <map>
#include <string>

namespace internal {

static std::wstring string2wstring(std::string str);

}

#define MESSAGE_TO_BROWSER "MessageToBrowser"
#define MESSAGE_TO_RENDERER "MessageToRenderer"

static CefRefPtr<CefV8Context> g_cb_context;
static CefRefPtr<CefV8Value> g_callback;

class Test {
public:
  static void ExecuteJsCode(CefRefPtr<CefBrowser> browser) {
    bool open(false);
    if (open) {
      CefRefPtr<CefFrame> frame = browser->GetMainFrame();
      CefString js_code = "alert('ExecuteJavaScript works!');";
      frame->ExecuteJavaScript(js_code, frame->GetURL(), 0);
    }
  }

  static void WindowBinding(CefRefPtr<CefV8Context> context) {
    // Retrieve the context's window object.
    CefRefPtr<CefV8Value> object = context->GetGlobal();

    // Create a new V8 string value. See the "Basic JS Types" section below.
    CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value!");

    // Add the string to the window object as "window.myval". See the "JS Objects" section below.
    object->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);
  }

  static void RegisterExtension() {
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
public:
  class V8Handler : public CefV8Handler {
  public:
    virtual bool Execute(const CefString& name,
                         CefRefPtr<CefV8Value> object,
                         const CefV8ValueList& arguments,
                         CefRefPtr<CefV8Value>& retval,
                         CefString& exception) {
      if (name == "myfunc") {
        retval = CefV8Value::CreateString("My Value!");
        return true;
      }
      return false;
    }
    IMPLEMENT_REFCOUNTING(V8Handler);
  };
public:
  static void WindowBindingWithJSFuntion(CefRefPtr<CefV8Context> context) {
    CefRefPtr<CefV8Value> obj = context->GetGlobal();
    CefRefPtr<V8Handler> v8_handler(new V8Handler);
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", v8_handler);
    obj->SetValue("myfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);
  }

  static void RegisterExtensionWithJSFunction() {
    std::string extensionCode =
      "var test1;"
      "if (!test1)"
      "  test1 = {};"
      "(function() {"
      "  test1.myfunc = function() {"
      "    native function myfunc();"
      "    return myfunc();"
      "  };"
      "})();";

    CefRefPtr<V8Handler> v8_handler(new V8Handler);
    CefRegisterExtension("v8/test1", extensionCode, v8_handler);
  }
public:
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
        return true;
      }

      if (name == "send_message_to_browser") {
        CefRefPtr<CefProcessMessage> message = 
          CefProcessMessage::Create(MESSAGE_TO_BROWSER);
        CefRefPtr<CefListValue> args = message->GetArgumentList();
        args->SetString(0, name);
        args->SetString(1, "The message send to browser from renderer.");
        CefV8Context::GetCurrentContext()->GetBrowser()->SendProcessMessage(PID_BROWSER, message);
        return true;
      }
      return false;
    }
  private:
    IMPLEMENT_REFCOUNTING(JSBindingV8Handler);
  };
  static void RegisterJsCallback(CefRefPtr<CefV8Context> context) {
    CefRefPtr<CefV8Value> obj = context->GetGlobal();
    CefRefPtr<JSBindingV8Handler> handler(new JSBindingV8Handler);
    obj->SetValue("register", 
                  CefV8Value::CreateFunction("register", handler), 
                  V8_PROPERTY_ATTRIBUTE_NONE);

    // add run callback
    obj->SetValue("runcb",
                  CefV8Value::CreateFunction("runcb", handler),
                  V8_PROPERTY_ATTRIBUTE_NONE);
    obj->SetValue("SendMessageToBrowser",
                  CefV8Value::CreateFunction("send_message_to_browser", handler),
                  V8_PROPERTY_ATTRIBUTE_NONE);
  }
public:
  static void SimpleLoadUrl(CefRefPtr<CefBrowser> browser, 
                            const CefString &url) {
    browser->GetMainFrame()->LoadURL(url);
  }
  static void CustomLoadUrl(CefRefPtr<CefBrowser> browser) {
    CefRefPtr<CefRequest> request = CefRequest::Create();
    request->SetURL("https://baidu.com/s");
    request->SetMethod("GET");

    CefRequest::HeaderMap header_map;
    header_map.insert(std::make_pair("X-My-Header", "My Header Value"));
    request->SetHeaderMap(header_map);

    const std::string data("wd=CEF");
    CefRefPtr<CefPostData> post_data = CefPostData::Create();
    CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
    element->SetToBytes(data.size(), data.c_str());
    post_data->AddElement(element);
    request->SetPostData(post_data);
  }
public:
  class UrlRequestClient : public CefURLRequestClient {
  public:
    virtual void OnRequestComplete(CefRefPtr<CefURLRequest> request) OVERRIDE {
      CefURLRequest::Status status = request->GetRequestStatus();
      CefURLRequest::ErrorCode error_code = request->GetRequestError();
      CefRefPtr<CefResponse> response = request->GetResponse();

      // ... do something
    }

    virtual void OnUploadProgress(CefRefPtr<CefURLRequest> request,
                                  int64 current,
                                  int64 total) OVERRIDE {
      upload_total_ += total;
    }

    virtual void OnDownloadProgress(CefRefPtr<CefURLRequest> request,
                                    int64 current,
                                    int64 total) OVERRIDE {
      download_data_ += total;
    }

    virtual void OnDownloadData(CefRefPtr<CefURLRequest> request,
                                const void* data,
                                size_t data_length) OVERRIDE {
      download_data_ += std::string(static_cast<const char*>(data), data_length);
    }

    virtual bool GetAuthCredentials(bool isProxy,
      const CefString& host,
      int port,
      const CefString& realm,
      const CefString& scheme,
      CefRefPtr<CefAuthCallback> callback) OVERRIDE {
      CEF_REQUIRE_IO_THREAD();
      return true;
    }
  private:
    IMPLEMENT_REFCOUNTING(UrlRequestClient);
    size_t upload_total_;
    size_t download_totsl_;
    std::string download_data_;
  };
  static void SendRequest(const CefString &url) {
    CefRefPtr<CefRequest> request = CefRequest::Create();
    request->SetURL(url);
    CefRefPtr<UrlRequestClient> client(new UrlRequestClient);
    CefRefPtr<CefURLRequest> url_request = CefURLRequest::Create(request, client.get(), NULL);
    // url_request->Cancel();
  }
public:
  static void SetOffScreen(CefRefPtr<CefCommandLine> command_line, bool open) {
    if (open)
      command_line->AppendSwitch("--off-screen-rendering-enabled");
  }
};

//class JsBinding {
//public:
//  static std::string GetRegisterCode() {
//    std::string reg_code =
//      "var app;"
//      "if(!app)"
//      "  app = {};"
//      "(function() {"
//      "  app.SendMessage = function(name, args) {"
//      "    native function SendMessage();"
//      "    return SendMessage(name, args);"
//      "  }"
//      "  app.RegisterFunction(name, callback) {"
//      "    native function RegisterFunction();"
//      "    return RegisterFunction(name, callback);"
//      "  }"
//      "})();";
//    return reg_code;
//  }
//public:
//  class V8Handler : public CefV8Handler {
//  public:
//    static const std::string kSendMessage;
//    static const std::string kRegisterFunction;
//  public:
//    V8Handler() {}
//    ~V8Handler() {
//      if (!callbacks_.empty()) {
//        CallbackMap::iterator beg = callbacks_.begin();
//        for (; beg != callbacks_.end(); ++beg) {
//
//        }
//      }
//    }
//    virtual bool Execute(const CefString& name,
//                         CefRefPtr<CefV8Value> object,
//                         const CefV8ValueList& arguments,
//                         CefRefPtr<CefV8Value>& retval,
//                         CefString& exception) {
//      if (name == kSendMessage) {
//
//      }
//      if (name == kRegisterFunction &&
//          arguments.size() == 2 &&
//          arguments[0]->IsString() &&
//          arguments[1]->IsFunction()) {
//        std::string name = arguments[0]->GetStringValue();
//        CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
//        int browser_id = context->GetBrowser()->GetIdentifier();
//        callbacks_.insert(std::make_pair(
//          std::make_pair(name, browser_id),
//          std::make_pair(context, arguments[1])));
//      }
//
//    }
//  private:
//    typedef std::map<std::pair<std::string, int>, 
//      std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value> > > CallbackMap;
//    CallbackMap callbacks_;
//  };
//};
//
//const std::string JsBinding::V8Handler::kSendMessage = "kSendMessage";
//const std::string JsBinding::V8Handler::kRegisterFunction = "RegisterFunction";

namespace internal {

  static std::wstring string2wstring(std::string str)
  {
    std::wstring result;
    size_t len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    TCHAR* buffer = new TCHAR[len + 1];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';  result.append(buffer);
    delete[] buffer;
    return result;
  }
}

#endif // !CEF_TESTS_CEF_SIMPLE_TEST_H_
