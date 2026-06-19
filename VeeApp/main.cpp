#include "vee.hpp"
#include "Sandbox.hpp"


int main()
{
	vee::Application app;
	app.Initialize();
	app.AddLayer(vee::MakeRef<vee::SandboxLayer>());
	app.Tick();
	app.ShutDown();
}