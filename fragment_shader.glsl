#version 330 core

out vec4 FragColor;

in vec2 TexCoord;
flat in int FragTexID;
flat in vec3 FragNormals;

uniform sampler2D texturesArray[4];
uniform float near;
uniform float far;

//diffusion lighting
const bool DIFFUSION_LIGHTING = true;
const vec3 AMBIENT_INTENSITY = vec3(0.5f);
const vec3 LIGHT_DIRECTION = normalize(vec3(0.2f, 0.6f, 0.5f)); //this will change dynamically in the future
const vec3 LIGHT_COLOUR = vec3(0.4f);

//atmospheric colouring
//it is possible to split the factor for each colour channel
//but the result might be too photo realistic
const bool ATMOSPHERIC_COLOURING = true;
const float ATTENUATION_FACTOR = 0.000002f;
const vec3 FOG_COLOUR = vec3(0.65f, 0.65f, 0.65f);

//z buffer depth linearization function
float linearizeDepth(float depth, float near, float far) {
    return (2.0 * near * far) / (far + near - depth * (far - near));
}

void main()
{
    vec4 texColor = texture(texturesArray[FragTexID], TexCoord);

    if (DIFFUSION_LIGHTING) {
        float diff = max(dot(FragNormals, LIGHT_DIRECTION), 0.0f);
        vec3 diffuse = diff * LIGHT_COLOUR;
        texColor *= vec4(AMBIENT_INTENSITY + diffuse, 1.0f);
    }

    if (ATMOSPHERIC_COLOURING) {
        float depth = gl_FragCoord.z;
        float distance = linearizeDepth(depth, near, far);

        //f(d) = e^(-ATTENUATION_FACTOR * d)
        float fogFactor = exp(-ATTENUATION_FACTOR * distance * distance);
        //f(d) = 1/(1+axx)
        //float fogFactor = 1.0 / (1.0 + ATTENUATION_FACTOR * distance * distance);
        fogFactor = clamp(fogFactor, 0.0, 1.0);

        texColor = vec4(mix(FOG_COLOUR, texColor.rgb, fogFactor), texColor.a);
    }

    FragColor = texColor;
}