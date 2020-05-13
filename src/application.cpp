#include "application.h"
#include <WindowsX.h>
#include <DirectXMath.h>

#include "core/callabel.h"
#include "core/m_assert.h"

#include <thread>

namespace app {

	namespace {

		LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			// Forward hwnd on because we can get messages (e.g., WM_CREATE)
			// before CreateWindow returns, and thus before mhMainWnd is valid.
			return Application::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
		}

		class UpdateCamera : public core::Callable
		{
		public:
			UpdateCamera(render::RenderD12& render)
				: render_(render)
			{}

			virtual core::Value call(const vector<core::Value>& args) override
			{
				ASSERT(args.size() == 3, "Wrong count of args.");

				render_.updateCamera(args[0].get<double>(), args[1].get<double>(), args[2].get<double>());
				return core::Value();
			}

			virtual string toString() const
			{
				return "UpdateCamera";
			}

		private:
			render::RenderD12& render_;
		};

	}

	Application* Application::app_ = nullptr;
	Application* Application::GetApp()
	{
		return app_;
	}

	Application::Application() 
		: script_(fileSystem_), scene_(render_)

	{
		assert(app_ == nullptr);
		app_ = this;
	}

	bool Application::initialize()
	{

		if (!initWindow())
			return false;

		if (!render_.initialize(mhMainWnd))
			return false;

		if (!scene_.initialize())
			return false;

		script_.registreFunction("updateCamera", makeShared<UpdateCamera>(render_));

		if (!script_.initialize())
			return false;

		return true;
	}

	bool Application::initWindow()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = mhAppInst;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = "MainWnd";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, "RegisterClass Failed.", 0, 0);
			return false;
		}

		// Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, clientWidth_, clientHeight_ };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		mhMainWnd = CreateWindow("MainWnd", mMainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
		if (!mhMainWnd)
		{
			MessageBox(0, "CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(mhMainWnd, SW_SHOW);
		UpdateWindow(mhMainWnd);

		return true;
	}


	LRESULT Application::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			// WM_ACTIVATE is sent when the window is activated or deactivated.  
			// We pause the game when the window is deactivated and unpause it 
			// when it becomes active.  
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				mAppPaused = true;
				//mTimer.Stop();
			}
			else
			{
				mAppPaused = false;
				//mTimer.Start();
			}
			return 0;

			// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE:
			// Save the new client area dimensions.
			clientWidth_ = LOWORD(lParam);
			clientHeight_ = HIWORD(lParam);
			/*if (md3dDevice)
			{
				if (wParam == SIZE_MINIMIZED)
				{
					mAppPaused = true;
					mMinimized = true;
					mMaximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					mAppPaused = false;
					mMinimized = false;
					mMaximized = true;
					OnResize();
				}
				else if (wParam == SIZE_RESTORED)
				{

					// Restoring from minimized state?
					if (mMinimized)
					{
						mAppPaused = false;
						mMinimized = false;
						OnResize();
					}

					// Restoring from maximized state?
					else if (mMaximized)
					{
						mAppPaused = false;
						mMaximized = false;
						OnResize();
					}
					else if (mResizing)
					{
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					}
					else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
					{
						OnResize();
					}
				}
			}*/

			return 0;

			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
			mAppPaused = true;
			//mResizing = true;
			//mTimer.Stop();
			return 0;

			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
		case WM_EXITSIZEMOVE:
			mAppPaused = false;
			//mResizing = false;
			//mTimer.Start();
			//OnResize();
			return 0;

			// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

			// The WM_MENUCHAR message is sent when a menu is active and the user presses 
			// a key that does not correspond to any mnemonic or accelerator key. 
		case WM_MENUCHAR:
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE);

			// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			//OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			//OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			else if ((int)wParam == VK_F2)
			{
				//Set4xMsaaState(!m4xMsaaState);
			}

			return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void Application::OnMouseMove(WPARAM btnState, int x, int y)
	{
		if ((btnState & MK_LBUTTON) != 0)
		{		
			btnState_ = 1;
		}
		else if ((btnState & MK_RBUTTON) != 0)
		{			
			btnState_ = 2;
		}
		
		lastMousePos_.x = x;
		lastMousePos_.y = y;
	}


	void Application::update()
	{
		script_.update(btnState_, lastMousePos_.x, lastMousePos_.y);
		btnState_ = 0;		
	}

	void Application::draw()
	{
		render_.startDraw();
		scene_.draw();
		render_.draw();
	}

	void Application::run()	
	{
		MSG msg = { 0 };
		while (true)
		{	
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (!mAppPaused)
			{
				update();
				draw();
			}			
			std::this_thread::sleep_for(std::chrono::milliseconds(20));			
			
		}
	}
}