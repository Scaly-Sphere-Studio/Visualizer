#version 330 core

in vec4 v_col;
//Color output using vertices data
out vec4 Color;


void main(){
  //Render the colors
  //Color = v_col;
  Color = vec4(0.2,0.8,0.3,1);
}