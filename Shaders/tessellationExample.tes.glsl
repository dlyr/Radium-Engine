layout( triangles, equal_spacing ) in;

#include "TransformStructs.glsl"

// This is for a preview of the shader composition, but in time we must use more specific Light
// Shader
#include "DefaultLight.glsl"

in vec3 tcs_position[];
in vec3 tcs_normal[];
in vec3 tcs_tangent[];
in vec3 tcs_bitangent[];
in vec3 tcs_texcoord[];
in vec4 tcs_color[];

uniform Transform transform;

layout( location = 0 ) out vec3 tes_position;
layout( location = 1 ) out vec3 tes_normal;
layout( location = 2 ) out vec3 tes_texcoord;
layout( location = 3 ) out vec3 tes_vertexcolor;
layout( location = 4 ) out vec3 tes_tangent;
layout( location = 5 ) out vec3 tes_viewVector;
layout( location = 6 ) out vec3 tes_lightVector;
layout( location = 7 ) out vec3 tes_patchDistance;

vec3 lerp3D( vec3 v0, vec3 v1, vec3 v2 ) {
    return vec3( gl_TessCoord.x ) * v0 + vec3( gl_TessCoord.y ) * v1 + vec3( gl_TessCoord.z ) * v2;
}

in gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
}
gl_in[gl_MaxPatchVertices];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

void main() {

    mat4 mvp = transform.proj * transform.view * transform.model;

    vec3 interp_position  = lerp3D( tcs_position[0], tcs_position[1], tcs_position[2] );
    gl_Position        = mvp * vec4( interp_position, 1. );
    vec3 interp_normal    = lerp3D( tcs_normal[0], tcs_normal[1], tcs_normal[2] );
    vec3 interp_tangent   = lerp3D( tcs_tangent[0], tcs_tangent[1], tcs_tangent[2] );
    vec3 interp_bitangent = lerp3D( tcs_bitangent[0], tcs_bitangent[1], tcs_bitangent[2] );
    vec3 interp_texcoord  = lerp3D( tcs_texcoord[0], tcs_texcoord[1], tcs_texcoord[2] );
    vec3 interp_color     = lerp3D( tcs_color[0].rgb, tcs_color[1].rgb, tcs_color[2].rgb );

    vec4 pos = transform.model * vec4( interp_position, 1.0 );
    pos /= pos.w;

    vec3 normal  = mat3( transform.worldNormal ) * interp_normal;
    vec3 tangent = mat3( transform.model ) * interp_tangent;

    vec3 eye = -transform.view[3].xyz * mat3( transform.view );

    tes_position = vec3( pos );
    tes_texcoord = interp_texcoord;

    tes_normal  = normal;
    tes_tangent = tangent;

    tes_viewVector    = vec3( eye - interp_position );
    tes_lightVector   = getLightDirection( light, interp_position );
    tes_vertexcolor   = interp_color.rgb;
    tes_patchDistance = gl_TessCoord;
}
