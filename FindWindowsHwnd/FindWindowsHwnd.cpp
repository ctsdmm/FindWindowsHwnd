
#include <vector>
#include <string>
#include <Windows.h>
#include <dwmapi.h>
#include <array>
#include <iostream>
struct Window
{
public:
	Window(nullptr_t) {}
	Window(HWND hwnd, std::string const& title, std::string& className)
	{
		m_hwnd = hwnd;
		this->m_title = title;
		this->m_className = className;

	}

	HWND Hwnd() const noexcept { return m_hwnd; }
	std::string Title() const noexcept { return m_title; }
	std::string ClassName() const noexcept { return m_className; }
	std::string StrTitle() const noexcept { return m_title; }
	void  SetStrTitle(const std::string& name) { m_title = name; }

private:
	HWND m_hwnd;
	std::string m_title;
	std::string m_className;

};


inline bool IsAltTabWindow(Window const& window)
{
	HWND hwnd = window.Hwnd();
	HWND shellWindow = GetShellWindow();

	auto title = window.Title();
	auto className = window.ClassName();

	if (hwnd == shellWindow)
	{
		return false;
	}

	if (title.length() == 0)
	{
		return false;
	}

	if (!IsWindowVisible(hwnd))
	{
		return false;
	}

	if (GetAncestor(hwnd, GA_ROOT) != hwnd)
	{
		return false;
	}

	LONG style = GetWindowLong(hwnd, GWL_STYLE);
	if (!((style & WS_DISABLED) != WS_DISABLED))
	{
		return false;
	}

	DWORD cloaked = FALSE;
	HRESULT hrTemp = DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked));
	if (SUCCEEDED(hrTemp) &&
		cloaked == DWM_CLOAKED_SHELL)
	{
		return false;
	}
	return true;
}


inline std::string GetClassName(HWND hwnd)
{
	std::array<CHAR, 1024> className;


	::GetClassNameA(hwnd, className.data(), (int)className.size());

	std::string title(className.data());
	return title;
}

inline std::string GetWindowText(HWND hwnd)
{
	std::array<CHAR, 1024> windowText;
	::GetWindowTextA(hwnd, windowText.data(), (int)windowText.size());
	std::string title(windowText.data());
	return title;
}


inline BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	auto class_name = GetClassName(hwnd);
	auto title = GetWindowText(hwnd);

	auto window = Window(hwnd, title, class_name);

	if (!IsAltTabWindow(window))
	{
		return TRUE;
	}

	std::vector<Window>& windows = *reinterpret_cast<std::vector<Window>*>(lParam);

	// 
	int num = 1;
	std::string name = window.StrTitle();

	for (size_t i = 0; i < windows.size(); i++)
	{
		if (windows[i].StrTitle() == name)
		{
			name += std::to_string(num++);
			i = 0;
			window.SetStrTitle(name);
		}
	}
	windows.push_back(window);

	return TRUE;
}

inline const std::vector<Window> EnumerateWindows()
{
	std::vector<Window> windows;
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&windows));
	return windows;
}



int  main()
{
	auto info = EnumerateWindows();

	for (size_t i = 0; i < info.size(); i++)
	{
		std::cout << "Windows  " << i + 1 << "\n";
		std::cout << "-------------------------\n" << "windowsClassName:  " << info[i].ClassName() << "\nwindowsTitle:  " << info[i].Title() << "\nHWND:  " << info[i].Hwnd() << "\n-------------------------\n \n \n";
	}
	return 0;
}
