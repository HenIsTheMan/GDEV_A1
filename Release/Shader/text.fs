#version 330 core
in vec2 texCoords;
out vec4 colour;

uniform sampler2D text;
uniform vec3 textColour;

void main(){    
    vec4 sampled = vec4(1.f, 1.f, 1.f, texture(text, texCoords).r);
    colour = vec4(textColour, 1.f) * sampled;
}  