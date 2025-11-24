#include "Vee.hpp"
#include "platform/windows/windows_window.hpp"

using namespace std;

int main()
{
	vee::IWindow* window = new vee::WindowsWindow();

	vee::WindowProperties props;
	props.Name = "Vee Engine";
	props.Width = 1280;
	props.Height = 720;
	window->Initialize(props);
	while (!window->ShouldClose())
	{
		window->Update();
	}
}
