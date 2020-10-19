//#include "TransformStructs.glsl"

// This is for a preview of the shader composition, but in time we must use more specific Light
// Shader
//#include "DefaultLight.glsl"

layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in vec3 in_tangent;
layout( location = 3 ) in vec3 in_bitangent;
layout( location = 4 ) in vec3 in_texcoord;
layout( location = 5 ) in vec4 in_color;

//uniform Transform transform;

//uniform mat4 uLightSpace;

out vec3 v_position;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec3 v_texcoord;
out vec4 v_color;

void main() {
    v_position  = in_position;
    v_normal    = in_normal;
    v_tangent   = in_tangent;
    v_bitangent = in_bitangent;
    v_texcoord  = in_texcoord;
    v_color     = in_color;
}
