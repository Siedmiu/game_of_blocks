#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

flat in int v_faceDirection[];
flat in int v_TexID[];

out vec2 TexCoord;
flat out int FragTexID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const float blockVertices[120] = {
	//pos             //texture cords
	1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  //bottom-right
	0.0f, 0.0f, 0.0f,  0.0f, 0.0f,  //bottom-left
	1.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top-right
	0.0f, 1.0f, 0.0f,  0.0f, 1.0f,  //top-left
	// ^ back ^

	0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  //bottom-left
	1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  //bottom-right
	0.0f, 1.0f, 1.0f,  0.0f, 1.0f,  //top-left
	1.0f, 1.0f, 1.0f,  1.0f, 1.0f,  //top-right
	// ^ front ^
		
	0.0f, 0.0f, 0.0f,  1.0f, 0.0f,  //bottom-back
	0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  //bottom-front
	0.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top-back
	0.0f, 1.0f, 1.0f,  0.0f, 1.0f,  //top-front
	// ^ left ^

	1.0f, 0.0f, 1.0f,  0.0f, 0.0f,  //bottom-front
	1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  //bottom-back
	1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  //top-front
	1.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //top-back
	// ^ right ^

	0.0f, 0.0f, 0.0f,  0.0f, 1.0f,  //back-left
	1.0f, 0.0f, 0.0f,  1.0f, 1.0f,  //back-right
	0.0f, 0.0f, 1.0f,  0.0f, 0.0f,  //front-left
	1.0f, 0.0f, 1.0f,  1.0f, 0.0f,  //front-right
	// ^ bottom ^

	1.0f, 1.0f, 0.0f,  1.0f, 1.0f,  //back-right
	0.0f, 1.0f, 0.0f,  0.0f, 1.0f,  //back-left
	1.0f, 1.0f, 1.0f,  1.0f, 0.0f,  //front-right
	0.0f, 1.0f, 1.0f,  0.0f, 0.0f   //front-left
	// ^ top ^
};

const bool CALCULATE_CURVATURE = true;
const float WORLD_CURVATURE_RATE = 0.002f;

void createVertex(vec3 offset, vec2 texCoord);

void main() {
    vec3 basePos = gl_in[0].gl_Position.xyz;
    
	createVertex(
	vec3(blockVertices[0 + 20 * v_faceDirection[0]], blockVertices[1 + 20 * v_faceDirection[0]], blockVertices[2 + 20 * v_faceDirection[0]]),
	vec2(blockVertices[3 + 20 * v_faceDirection[0]], blockVertices[4 + 20 * v_faceDirection[0]]));
	createVertex(
	vec3(blockVertices[0 + 5 + 20 * v_faceDirection[0]], blockVertices[1 + 5 + 20 * v_faceDirection[0]], blockVertices[2 + 5 + 20 * v_faceDirection[0]]),
	vec2(blockVertices[3 + 5 + 20 * v_faceDirection[0]], blockVertices[4 + 5 + 20 * v_faceDirection[0]]));
    createVertex(
	vec3(blockVertices[0 + 10 + 20 * v_faceDirection[0]], blockVertices[1 + 10 + 20 * v_faceDirection[0]], blockVertices[2 + 10 + 20 * v_faceDirection[0]]),
	vec2(blockVertices[3 + 10 + 20 * v_faceDirection[0]], blockVertices[4 + 10 + 20 * v_faceDirection[0]]));
	createVertex(
	vec3(blockVertices[0 + 15 + 20 * v_faceDirection[0]], blockVertices[1 + 15 + 20 * v_faceDirection[0]], blockVertices[2 + 15 + 20 * v_faceDirection[0]]),
	vec2(blockVertices[3 + 15 + 20 * v_faceDirection[0]], blockVertices[4 + 15 + 20 * v_faceDirection[0]]));

    EndPrimitive();
}

void createVertex(vec3 offset, vec2 texCoord) {
    vec4 blockPos = gl_in[0].gl_Position + vec4(offset, 0.0);

	if (CALCULATE_CURVATURE) {
        vec4 posView = view * model * blockPos;
        
        float fragmentDist = length(posView.xyz);
        float curvedY = posView.y - WORLD_CURVATURE_RATE * fragmentDist * fragmentDist;
        posView.y = curvedY;
        
        gl_Position = projection * posView;
	} else
    gl_Position = projection * view * model * blockPos;

    TexCoord = texCoord;
    FragTexID = v_TexID[0];
    EmitVertex();
}