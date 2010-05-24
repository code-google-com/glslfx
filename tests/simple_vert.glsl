#version 150 core
#include "test.glsl"

uniform mat4 mv;
uniform mat4 p;

in vec4 pos;

void main(){
     gl_position = p * mv * pos;
}
