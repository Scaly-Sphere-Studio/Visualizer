#version 330 core

in vec4 v_col;
in vec2 uv;
//Color output using vertices data
out vec4 Color;

uniform sampler2D tex;

void main(){
  // Raw color ...
  Color = v_col;
  // ... or blend with texture
  //vec4 tex_col = texture(tex, uv);
  //Color = mix(v_col, tex_col, tex_col.a);
}