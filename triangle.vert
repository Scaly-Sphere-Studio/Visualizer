//#version 330 core
//
////Coordinates and colors data input
//layout(location = 0) in vec3 vertexPosition_modelspace;
//layout(location = 1) in vec3 vertexPosition_color;
//
//out vec3 v_color;
//uniform mat4 MVP;
//
//
//void main(){
//  gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
//  v_color = vertexPosition_color;
//}

#version 330 core

//Coordinates and colors data input
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec3 box_pos;
layout(location = 2) in vec2 box_size;
layout(location = 3) in vec4 box_col;


out vec4 v_col;
uniform mat4 MVP;


void main(){

	//vec3 pos = vec3(vertexPosition_modelspace.xy + box_pos.xy, 0);
	vec3 pos = vec3(v_pos.x * box_size.x, v_pos.y * box_size.y, v_pos.z);
	pos += box_pos;

	gl_Position = MVP * vec4(pos, 1);

	v_col = box_col;
}

