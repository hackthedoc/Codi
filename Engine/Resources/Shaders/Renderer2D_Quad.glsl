//--------------------------
//	   - Codi Engine -
//	Renderer2D Quad Shader
// -------------------------

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(set = 0, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

struct QuadInstanceData {
    mat4 Model;
    vec4 Color;
    float TilingFactor;
    int EntityID;
    float padding0;
    float padding1;
    float padding2;
};

layout(std430, binding = 1) readonly buffer QuadBuffer {
    QuadInstanceData Instances[];
};

struct VertexOutput {
	vec4 Color;
};

layout (location = 0) out VertexOutput Output;

void main() {
    uint instanceID = gl_InstanceIndex;
    QuadInstanceData instance = Instances[instanceID];

	Output.Color = instance.Color;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput {
	vec4 Color;
};

layout (location = 0) in VertexOutput Input;

void main() {
	o_Color = Input.Color;
}
