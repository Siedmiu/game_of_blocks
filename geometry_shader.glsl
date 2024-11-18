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

const int FACE_DIRECTION_BACK = 0;
const int FACE_DIRECTION_FRONT = 1;
const int FACE_DIRECTION_LEFT = 2;
const int FACE_DIRECTION_RIGHT = 3;
const int FACE_DIRECTION_BOTTOM = 4;
const int FACE_DIRECTION_TOP = 5;

void createVertex(vec3 offset, vec2 texCoord) {
    vec4 blockPos = gl_in[0].gl_Position + vec4(offset, 0.0);
    gl_Position = projection * view * model * blockPos;
    TexCoord = texCoord;
    FragTexID = v_TexID[0];
    EmitVertex();
}

void main() {
    vec3 basePos = gl_in[0].gl_Position.xyz;
    
    // Generate vertices based on face direction
    switch(v_faceDirection[0]) {
        case FACE_DIRECTION_BACK:
            // Bottom-right
            createVertex(vec3(1.0, 0.0, 0.0), vec2(1.0, 0.0));
            // Bottom-left
            createVertex(vec3(0.0, 0.0, 0.0), vec2(0.0, 0.0));
            // Top-right
            createVertex(vec3(1.0, 1.0, 0.0), vec2(1.0, 1.0));
            // Top-left
            createVertex(vec3(0.0, 1.0, 0.0), vec2(0.0, 1.0));
            break;

        case FACE_DIRECTION_FRONT:
            createVertex(vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0));
            createVertex(vec3(1.0, 0.0, 1.0), vec2(1.0, 0.0));
            createVertex(vec3(0.0, 1.0, 1.0), vec2(0.0, 1.0));
            createVertex(vec3(1.0, 1.0, 1.0), vec2(1.0, 1.0));
            break;

        case FACE_DIRECTION_LEFT:
            createVertex(vec3(0.0, 0.0, 0.0), vec2(1.0, 0.0));
            createVertex(vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0));
            createVertex(vec3(0.0, 1.0, 0.0), vec2(1.0, 1.0));
            createVertex(vec3(0.0, 1.0, 1.0), vec2(0.0, 1.0));
            break;

        case FACE_DIRECTION_RIGHT:
            createVertex(vec3(1.0, 0.0, 1.0), vec2(0.0, 0.0));
            createVertex(vec3(1.0, 0.0, 0.0), vec2(1.0, 0.0));
            createVertex(vec3(1.0, 1.0, 1.0), vec2(0.0, 1.0));
            createVertex(vec3(1.0, 1.0, 0.0), vec2(1.0, 1.0));
            break;

        case FACE_DIRECTION_BOTTOM:
            createVertex(vec3(0.0, 0.0, 0.0), vec2(0.0, 1.0));
            createVertex(vec3(1.0, 0.0, 0.0), vec2(1.0, 1.0));
            createVertex(vec3(0.0, 0.0, 1.0), vec2(0.0, 0.0));
            createVertex(vec3(1.0, 0.0, 1.0), vec2(1.0, 0.0));
            break;

        case FACE_DIRECTION_TOP:
            createVertex(vec3(1.0, 1.0, 0.0), vec2(1.0, 1.0));
            createVertex(vec3(0.0, 1.0, 0.0), vec2(0.0, 1.0));
            createVertex(vec3(1.0, 1.0, 1.0), vec2(1.0, 0.0));
            createVertex(vec3(0.0, 1.0, 1.0), vec2(0.0, 0.0));
            break;
    }
    
    EndPrimitive();
}