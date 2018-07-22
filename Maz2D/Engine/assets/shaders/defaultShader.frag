#version 330 core

uniform bool useTexture;
uniform vec4 inColor;
uniform sampler2D diffuseTexture;

in vec2 uvCoord;

out vec4 outColor;

void main()
{
	outColor = useTexture ? texture(diffuseTexture, uvCoord) : inColor;
}
