#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
flat in int FragTexID;
flat in vec3 FragNormals;

uniform sampler2D texturesArray[4];

const bool DIFFUSION_LIGHTING = true;
const vec3 AMBIENT_INTENSITY = vec3(0.5f);
const vec3 LIGHT_DIRECTION = normalize(vec3(0.2f, 0.6f, 0.5f));
const vec3 LIGHT_COLOUR = vec3(0.4f);

void main()
{
	if (DIFFUSION_LIGHTING) {
		float diff = max(dot(FragNormals, LIGHT_DIRECTION), 0.0f);
		vec3 diffuse = diff * LIGHT_COLOUR;

		vec4 result = vec4(AMBIENT_INTENSITY + diffuse, 1.0f);

		FragColor = texture(texturesArray[FragTexID], TexCoord) * result;
	} else {
		FragColor = texture(texturesArray[FragTexID], TexCoord);
	}
}