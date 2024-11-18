#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
flat in int FragTexID;

uniform sampler2D texturesArray[4];

void main()
{
	FragColor = texture(texturesArray[FragTexID], TexCoord);
}