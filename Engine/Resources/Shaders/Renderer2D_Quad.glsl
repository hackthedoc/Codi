//--------------------------
//	   - Codi Engine -
//	Renderer2D Quad Shader
// -------------------------

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(set = 0, binding = 0) uniform Camera {
	mat4 u_ViewProjection;
};

struct QuadInstanceData {
    mat4 Model;
    vec4 Color;
    float TexIndex;
    float TilingFactor;
    int EntityID;
    float padding0;
    float padding1;
};

layout(std430, binding = 1) readonly buffer QuadBuffer {
    QuadInstanceData Instances[];
};

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out float v_TilingFactor;
layout(location = 3) out flat float v_TexIndex;
//layout(location = 4) out flat int v_EntityID;

void main() {
    uint instanceID = gl_InstanceIndex;
    QuadInstanceData instance = Instances[instanceID];

	v_Color = instance.Color;
	v_TexCoord = a_TexCoord;
	v_TilingFactor = instance.TilingFactor;
	v_TexIndex = instance.TexIndex;
	//v_EntityID = instance.EntityID;

	gl_Position = u_ViewProjection * instance.Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
//layout(location = 1) out int o_EntityID;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in float v_TilingFactor;
layout(location = 3) in flat float v_TexIndex;
//layout(location = 4) in flat int v_EntityID;

layout(set = 0, binding = 2) uniform sampler2D u_Textures[32];

void main() {
	vec4 texColor = v_Color;

	int index = int(v_TexIndex);
	if(index >= 0 && index < 32)
		texColor *= texture(u_Textures[index], v_TexCoord * v_TilingFactor);

	if (texColor.a == 0.0)
		discard;

	o_Color = texColor;
	//o_EntityID = v_EntityID;
}
