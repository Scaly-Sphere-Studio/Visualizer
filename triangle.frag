//#version 330 core
//
//in vec4 v_color;
////Color output using vertices data
//out vec4 Color;
//
//
//void main(){
//
//  Color = v_color;
//
//}

#version 330 core

in vec4 v_col;
//Color output using vertices data
out vec4 Color;


void main(){

  Color = v_col;

}