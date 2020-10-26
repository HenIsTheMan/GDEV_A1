#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 texCoords;
uniform mat4 model;

void main(){
	gl_Position = model * vec4(aPos, 1.f);
	texCoords = vec2(aTexCoords.x, aTexCoords.y);
}