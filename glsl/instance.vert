#version 330 core

//Coordinates and colors data input
layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec2 box_size;
layout(location = 2) in vec4 box_col;
layout(location = 3) in vec3 box_pos;
//Textures
layout(location = 4) in vec2 tex_uv;
layout(location = 5) in uint tex_id;
//Transformations
layout(location = 6) in vec3 box_translate;
//layout(location = 7) in float box_scale;
//layout(location = 8) in vec3 box_rotate;


uniform mat4 u_MVP;

out vec4 v_col;
out vec2 uv;
flat out uint id;

void main(){

	//Updates the coordinates and size of the particles
	//vec3 pos = vec3(v_pos.x * box_size.x, v_pos.y * box_size.y, v_pos.z);
	vec3 pos = v_pos  * vec3(box_size, 1.0f);

	pos += box_pos + box_translate;

	//Updates the camera position in world
	gl_Position = u_MVP * vec4(pos,1);

	//Pass colors and textures parameters to the Fragment Shader
	v_col = box_col;
	uv = tex_uv;
	id = tex_id;
}

