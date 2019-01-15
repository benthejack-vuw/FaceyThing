#version 410
uniform mat4 ciModelViewProjection;
in vec4 ciPosition;
in vec2 ciTexCoord0;

in vec2 bounds_pos;

out vec2 blend_pos;
out vec2 tex_coord_0;

void main()
{
  blend_pos = bounds_pos;
	tex_coord_0 = ciTexCoord0;
	gl_Position = ciModelViewProjection * ciPosition;
}
