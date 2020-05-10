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

		void OnMouseMove(WPARAM btnState, int x, int y);
			
	private:
		// for window 
		static Application* mApp;
		HWND      mhMainWnd = nullptr; // main window handle
		std::string mMainWndCaption = "d3d App";
		int clientWidth_ = 800;
		int clientHeight_ = 600;
		POINT lastMousePos_;
		float theta_ = 1.5f * 3.14;
		float phi_ = 0.2f * 3.14;
		float radius_ = 15.0f;

		bool      mAppPaused = false;  // is the application paused?

		HINSTANCE mhAppInst = nullptr; // application instance handle

		render::RenderD12 render_;
		

	};

}
