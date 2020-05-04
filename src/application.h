#pragma once

#include "render/render.h"

namespace app {

	class Application
	{
	public:
		Application();

		bool initialize();
		void run();

		static Application* Application::GetApp();

		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		bool initWindow();

		void update();
		void draw();
			
	private:
		// for window 
		static Application* mApp;
		HWND      mhMainWnd = nullptr; // main window handle
		std::string mMainWndCaption = "d3d App";
		int mClientWidth = 800;
		int mClientHeight = 600;
		bool      mAppPaused = false;  // is the application paused?

		HINSTANCE mhAppInst = nullptr; // application instance handle

		render::RenderD12 m_render;
		

	};

}
