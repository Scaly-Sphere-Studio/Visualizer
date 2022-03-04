#version 330 core

in vec3 v_color;
//Color output using vertices data
out vec4 Color;


void main(){

  Color = vec4(v_color , 1);

}