// Vee.cpp : Defines the entry point for the application.
//

#include "Vee.hpp"
#include "Window.hpp"

using namespace std;

int main()
{
	Vee::Window* window = new Vee::Window("Vee", 1280, 720);

	while (!window->ShouldClose())
	{
		window->Update();
	}
}
