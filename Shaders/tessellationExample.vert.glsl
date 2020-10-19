#include "TransformStructs.glsl"

// This is for a preview of the shader composition, but in time we must use more specific Light
// Shader
#include "DefaultLight.glsl"

layout( location = 0 ) in vec3 in_position;
layout( location = 1 ) in vec3 in_normal;
layout( location = 2 ) in vec3 in_tangent;
layout( location = 3 ) in vec3 in_bitangent;
layout( location = 4 ) in vec3 in_texcoord;
layout( location = 5 ) in vec4 in_color;

uniform Transform transform;

uniform mat4 uLightSpace;

layout( location = 0 ) out vec3 tcs_position;
layout( location = 1 ) out vec3 tcs_normal;
layout( location = 2 ) out vec3 tcs_texcoord;
layout( location = 3 ) out vec3 tcs_vertexcolor;
layout( location = 4 ) out vec3 tcs_tangent;
layout( location = 5 ) out vec3 tcs_viewVector;
layout( location = 6 ) out vec3 tcs_lightVector;

void main() {

    tcs_position    = in_position;
    tcs_normal      = in_normal;
    tcs_texcoord    = in_texcoord;
    tcs_vertexcolor = in_vertexcolor;
    tcs_tangent     = in_tangent;
    tcs_viewVector  = in_viewVector;
    tcs_lightVector = in_lightVector;
}
