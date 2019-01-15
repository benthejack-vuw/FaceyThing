#version 410

uniform sampler2D previous_frame;
uniform sampler2D noise_texture;
uniform float 		noise_y;
uniform float     wipe_speed;
uniform float     noise_scale;
in vec2           tex_coord_0;
out vec4          outColor;

void main()
{
	 	vec2 noise_pos = vec2(tex_coord_0.x, noise_y);
		float noise_amt = texture2D(noise_texture, noise_pos).r;
		vec2 tex_pos = tex_coord_0 - vec2(0.0, wipe_speed + noise_amt*noise_scale);
		outColor = texture2D(previous_frame, vec2(tex_coord_0.x, tex_pos.y));
}
