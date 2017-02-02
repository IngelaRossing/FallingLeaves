#version 330 core

uniform float time;
uniform sampler2D tex;

in vec3 interpolatedNormal;
in vec2 st;

out vec4 color;

void main() {
     vec4 texcolor = texture(tex, st);

     if(texcolor.a < 0.1)
        discard;
     color = texcolor;
}
