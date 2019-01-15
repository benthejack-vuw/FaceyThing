#version 410

uniform sampler2D texture;
uniform sampler2D color_overlay;

in vec2 blend_pos;
in vec2 tex_coord_0;

out vec4 outColor;

float greyscale(vec4 col){
  return .2126 * col.r + .7152 * col.g + .0722 * col.b;
}

void main()
{
    float lum = greyscale(texture2D(texture, tex_coord_0));
    vec4 colour_grad = texture2D(color_overlay, blend_pos);
    colour_grad.rgb *= lum;
    outColor = colour_grad;
}
