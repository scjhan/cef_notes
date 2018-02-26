#include <windows.h>

#include "include/cef_sandbox_win.h"

#include "tests/cefsimple/browser/app_browser.h"
#include "tests/cefsimple/render/app_renderer.h"
#include "tests/cefsimple/common/app_other.h"

#include "tests/cefsimple/test.h"

const char kProcessType[] = "type";
const char kRendererProcess[] = "renderer";

int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  CefEnableHighDPISupport();

  CefMainArgs main_args(hInstance);
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  Test::SetOffScreen(command_line, true);

  CefRefPtr<CefApp> app;
  if (!command_line->HasSwitch(kProcessType))
    app = new AppBrowser();
  else if (command_line->GetSwitchValue(kProcessType) == kRendererProcess)
    app = new AppRenderer();
  else
    // gpu-process
    app = new AppOther(command_line->GetSwitchValue(kProcessType).ToWString());

  // 如果是主进程，返回-1继续执行
  // 其他进程会阻塞直到窗口关闭，返回正数
  int exit_code = CefExecuteProcess(main_args, app, NULL);
  if (exit_code >= 0) {
    return exit_code;
  }

  CefSettings settings;
  settings.no_sandbox = true;

  CefInitialize(main_args, settings, app, NULL);

  CefRunMessageLoop();

  CefShutdown();

  return 0;
}
