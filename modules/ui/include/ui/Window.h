//---------------------------------------------------------------------------

#ifndef WINDOW_H
#define WINDOW_H

//---------------------------------------------------------------------------

#include "WindowAdapter.h"

//---------------------------------------------------------------------------

namespace Rapture
{
	enum class WindowState
	{
		Normal,
		Hidden,
		Minimized,
		Maximized
	};

	enum class BorderStyle
	{
		Normal,
		Static,
		Fixed,
		None
	};

	link_class(Window, MetaClass<WindowAdapter>);

	class Window : public WindowAdapter
	{
	public:
		Window(Graphics * graphics, long left, long top, long width, long height, const WideString & caption = L"") : Window(graphics, IntRect {left, top, left + width, top + height}, caption) {}
		Window(Graphics * graphics, long width, long height, const WideString & caption = L"") : Window(graphics, IntSize {width, height}, caption) {}
		Window(Graphics * graphics, const IntSize & size, const WideString & caption = L"") : Window(graphics, IntRect {0, 0, size.x, size.y}, caption) {}
		Window(Graphics * graphics, const IntRect & rect, const WideString & caption = L"");
		virtual ~Window();

		long outerWidth() const
		{
			return _outerRegion.width();
		}

		long outerHeight() const
		{
			return _outerRegion.height();
		}

		LongSize outerSize() const
		{
			return _outerRegion.size();
		}

		LongRect outerRegion() const
		{
			return _outerRegion;
		}

		void centralize();

		virtual void registerHotkey(int id, int key, int modifiers = 0) override;
		virtual void unregisterHotkey(int id) override;

		bool isFullscreen() const
		{
			return _fullscreen;
		}

		void setFullscreen(bool fullscreen);
		void toggleFullscreen();

		void setState(WindowState state);

		void show();
		void hide();
		void minimize();
		void maximize();
		void restore();

		BorderStyle borderStyle() const
		{
			return _borderStyle;
		}

		void setBorderStyle(BorderStyle style);

		bool isVisible() const
		{
			return _state != WindowState::Hidden;
		}

		WindowState state() const
		{
			return _state;
		}

		void setCaption(const WideString & caption);
		WideString getCaption();

	protected:
		void makeFullscreen();
		void restoreSize();
		void applyBorderStyle();

		friend LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool _isShown = false;

		DWORD _normalStyle;
		LongRect _outerRegion;

		WINDOWPLACEMENT _fullscreenPlacement = {0};
		WINDOWPLACEMENT _windowedPlacement = {0};

		UINT_PTR _timer;
		WindowState _state = WindowState::Hidden;
		BorderStyle _borderStyle = BorderStyle::Normal;
	};

	inline bool IsKeyPressed(int v_key)
	{
		return hi_bit_mask::state(GetKeyState(v_key));
	}

	class WindowBackground : public Widget
	{
	public:
		WindowBackground(Window * window) : Widget(window)
		{
			setVisibility(true);
			setPlacement(ModelMask::FullSize, {0, 0, 0, 0});
		}

		virtual ~WindowBackground() {}

		virtual bool isDisplayable() const final
		{
			return true;
		}
	};
}

//---------------------------------------------------------------------------
#endif