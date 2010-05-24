#version 150 core

uniform float s;

#	 include <test.glsl>

out vec4 ocolor;

void main(){
     ocolor = vec4(0,1,0,1);
}
