#include <iostream>
// This header includes ALL the headers from the SSS/GL library
#include <SSS/GL.hpp>
#include "shader.hpp"
#include "visualizer.h"
#include "commons.h"

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}

int main(void)
{
	Visualizer v;
	v.run();
}