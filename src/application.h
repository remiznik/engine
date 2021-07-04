#pragma once

#include "script_system/script.h"
#include "render/render.h"
#include "render/scene.h"
#include "render/resource_manager.h"

namespace app {

	class Application
	{
	public:
		Application();

		bool initialize();
		void run();

		int getState() const { return btnState_; }
		int getX() const { return lastMousePos_.x;  }
		int getY() const { return lastMousePos_.y; }

		static Application* Application::GetApp();

		virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		bool initWindow();

		void update();
		void draw();

		void OnMouseMove(WPARAM btnState, int x, int y);
			
	private:
		// for window 
		static Application* app_;
		HWND      mhMainWnd = nullptr; // main window handle
		std::string mMainWndCaption = "d3d App";
		int clientWidth_ = 800;
		int clientHeight_ = 600;
		POINT lastMousePos_;		
		int btnState_ = 0;

		bool      mAppPaused = false;  // is the application paused?

		HINSTANCE mhAppInst = nullptr; // application instance handle

		render::RenderD12 render_;
		render::Scene scene_;

		core::FileSystem fileSystem_;
		script_system::Script script_;
		render::ResourceManager resourceManager_;

	};

}
