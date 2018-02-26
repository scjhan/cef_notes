#include "app_renderer.h"

#include "include/wrapper/cef_helpers.h"

#include "tests/cefsimple/test.h"

AppRenderer::AppRenderer() {

}

AppRenderer::~AppRenderer() {}

// CefRenderProcessHandler methods:
// Called after WebKit has been initialized.
void AppRenderer::OnWebKitInitialized() {
  CEF_REQUIRE_RENDERER_THREAD();

  CefMessageRouterConfig config;
  message_router_ = CefMessageRouterRendererSide::Create(config);

  Test::RegisterExtension();
  Test::RegisterExtensionWithJSFunction();
}

void AppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser,
  CefRefPtr<CefFrame> frame,
  CefRefPtr<CefV8Context> context) {
  CEF_REQUIRE_RENDERER_THREAD();

  message_router_->OnContextCreated(browser, frame, context);

  Test::WindowBinding(context);
  Test::WindowBindingWithJSFuntion(context);
  Test::RegisterJsCallback(context);
}

void AppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser,
  CefRefPtr<CefFrame> frame,
  CefRefPtr<CefV8Context> context) {
  CEF_REQUIRE_RENDERER_THREAD();

  message_router_->OnContextReleased(browser, frame, context);
}
bool AppRenderer::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
  CefProcessId source_process,
  CefRefPtr<CefProcessMessage> message) {
  CEF_REQUIRE_RENDERER_THREAD();

  if (message->GetName() == MESSAGE_TO_RENDERER) {
    char js_code[1024];
    sprintf(js_code, "alert('message name %s, message entity %s')",
      message->GetArgumentList()->GetString(0).ToString().c_str(),
      message->GetArgumentList()->GetString(1).ToString().c_str());
    browser->GetMainFrame()->ExecuteJavaScript(js_code, browser->GetMainFrame()->GetURL(), 0);
  }

  return message_router_->OnProcessMessageReceived(browser, source_process, 
                                                   message);
}