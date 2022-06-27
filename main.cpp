#include "visualizer.h"

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}

int main(void) try
{
	Visualizer::Ptr const& v = Visualizer::get();
	v->run();
}
CATCH_AND_LOG_FUNC_EXC;