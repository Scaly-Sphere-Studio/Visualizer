#version 330 core

in vec4 v_col;
in vec2 uv;
flat in uint id;

//Color output using vertices data
out vec4 Color;

uniform sampler2DArray u_Textures[gl_MaxTextureImageUnits];

void main(){
  // Blend with texture
  vec4 tex_col = texture(u_Textures[id], vec3(uv, 0));
  Color = mix(v_col, tex_col, tex_col.a);
}