#version 330 core

//Coordinates and colors data input
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertex_color;

out vec4 v_color;
uniform mat4 u_MVP;


void main(){
  gl_Position = u_MVP * vec4(vertexPosition_modelspace, 1);
  v_color = vec4(vertex_color, 1);
}
