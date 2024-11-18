#version 430 core

layout(std430, binding = 0) buffer VertexDataBuffer {
    float compressed[];
};

flat out int v_faceDirection;
flat out int v_TexID;

void main()
{
	float compressedData = compressed[gl_VertexID];
	uint data = floatBitsToUint(compressedData);

	uint x = (data >> 27) & 31u; // 5 bits
    uint y = (data >> 19) & 255u; // 8 bits
    uint z = (data >> 14) & 31u; // 5 bits
    uint texID = (data >> 9) & 31u; // 5 bits
	uint faceDirection = (data >> 6) & 7u; //3 bits

	gl_Position = vec4(float(x), float(y), float(z), 1.0);
	v_TexID = int(texID);
	v_faceDirection = int(faceDirection);
}