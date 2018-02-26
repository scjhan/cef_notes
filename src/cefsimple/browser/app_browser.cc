#include "app_browser.h"

#include "include/wrapper/cef_helpers.h"

#include "tests/cefsimple/common/browser_client.h"
#include "tests/cefsimple/utils.h"

AppBrowser::AppBrowser() {}

AppBrowser::~AppBrowser() {}

// Called on the browser process UI thread immediately after the CEF context
// has been initialized.
void AppBrowser::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

  CefWindowInfo window_info;
  window_info.SetAsPopup(NULL, "cefsimple");

  CefBrowserSettings browser_settings;

  CefRefPtr<BrowserClient> browser_client(new BrowserClient());

  std::string path = GetApplicationDir();
  CefString url = "file://" + path + "/binding.html";

  CefBrowserHost::CreateBrowser(window_info, browser_client, url, browser_settings, NULL);
}
