#version 330 core

//Coordinates and colors data input
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 box_pos;
layout(location = 2) in vec2 box_size;
layout(location = 3) in vec4 box_col;


out vec4 v_col;
uniform mat4 MVP;


void main(){
	//Updates the coordinates and size of the particles
	vec3 pos = vec3(v_pos.x * box_size.x, v_pos.y * box_size.y, v_pos.z);
	pos += box_pos;

	//Updates the camera position in world
	gl_Position = MVP * vec4(pos, 1);

	//Updates the colors
	v_col = box_col;
}

