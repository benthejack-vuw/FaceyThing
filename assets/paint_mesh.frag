#version 410

uniform sampler2D texture;
uniform sampler2D color_overlay;
uniform float hue_rotate;
uniform float fade;

in vec2 blend_pos;
in vec2 tex_coord_0;

out vec4 outColor;

float greyscale(vec4 col){
  return .2126 * col.r + .7152 * col.g + .0722 * col.b;
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    if(gl_FragCoord.y < 2 ){
      discard;
    }

    float lum = greyscale(texture2D(texture, tex_coord_0));
    vec3 colour_grad = rgb2hsv(texture2D(color_overlay, blend_pos).rgb);
    colour_grad.x += hue_rotate;
    colour_grad.x = colour_grad.x > 1.0 ? colour_grad.x-1.0 : colour_grad.x;
    colour_grad.y/=2;
    colour_grad.z*=1.1;
    colour_grad = hsv2rgb(colour_grad);
    colour_grad.rgb *= lum;
    outColor = vec4(colour_grad.rgb, fade);
}
