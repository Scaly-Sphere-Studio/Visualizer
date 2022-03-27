#version 330 core

//Coordinates and colors data input
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexPosition_color;

out vec3 v_color;
uniform mat4 MVP;


void main(){
  gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
  v_color = vertexPosition_color;
}
