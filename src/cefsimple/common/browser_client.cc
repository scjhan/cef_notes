#include "browser_client.h"

#include <cctype>

#include "include/wrapper/cef_helpers.h"
#include "include/cef_app.h"

#include "tests/cefsimple/test.h"

BrowserClient::BrowserClient() {
}

BrowserClient::~BrowserClient() {}

// CefClient methods:
// Call from CefRenderProcessHandler::OnProcessMessageReceived. Returns true
// if the message is handled by this router or false otherwise.
bool BrowserClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
  CefProcessId source_process,
  CefRefPtr<CefProcessMessage> message) {

  if (message->GetName() == MESSAGE_TO_BROWSER) {
    std::string msg_name = message->GetArgumentList()->GetString(0);
    std::string msg_entity = message->GetArgumentList()->GetString(1);
    ::MessageBox(NULL,  
                 internal::string2wstring(msg_entity).c_str(),
                 internal::string2wstring(msg_name).c_str(), 
                 NULL);

    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(MESSAGE_TO_RENDERER);
    message->GetArgumentList()->SetString(0, "browser_to_renderer");
    message->GetArgumentList()->SetString(1, "The message to renderer from browser.");
    browser->SendProcessMessage(PID_RENDERER, message);
  }

  return message_router_->OnProcessMessageReceived(browser, source_process, message);
}

// CefLifeSpanHandler methods:
// Called after a new browser is created. This callback will be the first
// notification that references |browser|.
void BrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  if (!message_router_) {
    CefMessageRouterConfig config;
    message_router_ = CefMessageRouterBrowserSide::Create(config);
    message_router_->AddHandler(&router_handler_, true);
  }

  Test::ExecuteJsCode(browser);

  browser->GetMainFrame()->LoadURL("https://www.baidu.com");
  Test::SimpleLoadUrl(browser, "https://baidu.com");
}

// Call from CefLifeSpanHandler::OnBeforeClose. Any pending queries associated
// with |browser| will be canceled and Handler::OnQueryCanceled will be
// called. No JavaScript callbacks will be executed since this indicates
// destruction of the browser.
void BrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  message_router_->RemoveHandler(&router_handler_);
  CefQuitMessageLoop();
}

// CefRequestHandler methods:
// Called on the UI thread before browser navigation. Return true to cancel
// the navigation or false to allow the navigation to proceed. The |request|
// object cannot be modified in this callback.
// CefLoadHandler::OnLoadingStateChange will be called twice in all cases.
// If the navigation is allowed CefLoadHandler::OnLoadStart and
// CefLoadHandler::OnLoadEnd will be called. If the navigation is canceled
// CefLoadHandler::OnLoadError will be called with an |errorCode| value of
// ERR_ABORTED.
bool BrowserClient::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
  CefRefPtr<CefFrame> frame,
  CefRefPtr<CefRequest> request,
  bool is_redirect) {
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnBeforeBrowse(browser, frame);

  return false;
}

// Called on the browser process UI thread when the render process
// terminates unexpectedly. |status| indicates how the process
// terminated.
void BrowserClient::OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
  TerminationStatus status) {
  CEF_REQUIRE_UI_THREAD();

  message_router_->OnRenderProcessTerminated(browser);

}

// RouterHandler methods:
bool BrowserClient::RouterHandler::OnQuery(CefRefPtr<CefBrowser> browser,
  CefRefPtr<CefFrame> frame,
  int64 query_id,
  const CefString& request,
  bool persistent,
  CefRefPtr<Callback> callback) {
  CEF_REQUIRE_UI_THREAD();

  const std::string reqstr = request;
  const std::string tolower("tolower");
  if (reqstr.find(tolower) != std::string::npos) {
    std::string message = reqstr.substr(tolower.size() + 1);
    std::string lower(message.size(), '\0');
    std::transform(message.begin(), message.end(), lower.begin(), std::tolower);
    callback->Success(lower);
    return true;
  }

  return false;
}