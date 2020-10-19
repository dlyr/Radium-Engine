layout( triangles, equal_spacing ) in;

#include "TransformStructs.glsl"

// This is for a preview of the shader composition, but in time we must use more specific Light
// Shader
#include "DefaultLight.glsl"

in vec3 tes_position[];
in vec3 tes_normal[];
in vec3 tes_texcoord[];
in vec3 tes_vertexcolor[];
in vec3 tes_tangent[];
in vec3 tes_viewVector[];
in vec3 tes_lightVector[];

uniform Transform transform;

layout( location = 0 ) out vec3 out_position;
layout( location = 1 ) out vec3 out_normal;
layout( location = 2 ) out vec3 out_texcoord;
layout( location = 3 ) out vec3 out_vertexcolor;
layout( location = 4 ) out vec3 out_tangent;
layout( location = 5 ) out vec3 out_viewVector;
layout( location = 6 ) out vec3 out_lightVector;

vec3 lerp3D( vec3 v0, vec3 v1, vec3 v2 ) {
    return vec3( gl_TessCoord.x ) * v0 + vec3( gl_TessCoord.y ) * v1 + vec3( gl_TessCoord.z ) * v2;
}


vec3 lerp3D( vec3 v0, vec3 v1, vec3 v2 ) {
    return vec3( gl_TessCoord.x ) * v0 + vec3( gl_TessCoord.y ) * v1 + vec3( gl_TessCoord.z ) * v2;
}


void main() {

    mat4 mvp = transform.proj * transform.view * transform.model;

    vec3 in_position  = lerp3D( tes_position[0], tes_position[1], tes_position[2] );
    vec3 in_normal    = lerp3D( tes_normal[0], tes_normal[1], tes_normal[2] );
    vec3 in_tangent   = lerp3D( tes_tangent[0], tes_tangent[1], tes_tangent[2] );
    vec3 in_bitangent = lerp3D( tes_bitangent[0], tes_bitangent[1], tes_bitangent[2] );
    vec3 in_texcoord  = lerp3D( tes_texcoord[0], tes_texcoord[1], tes_texcoord[2] );
    vec3 in_color     = lerp3D( tes_color[0].rgb, tes_color[1].rgb, tes_color[2].rgb );

    gl_Position = mvp * vec4( lerp3D( tes_position[0], tes_position[1], tes_position[2] ), 1.0 );

    vec4 pos = transform.model * vec4( in_position, 1.0 );
    pos /= pos.w;

    vec3 normal  = mat3( transform.worldNormal ) * in_normal;
    vec3 tangent = mat3( transform.model ) * in_tangent;

    vec3 eye = -transform.view[3].xyz * mat3( transform.view );

    out_position = vec3( pos );
    out_texcoord = in_texcoord;

    out_normal  = normal;
    out_tangent = tangent;

    out_viewVector  = vec3( eye - out_position );
    out_lightVector = getLightDirection( light, out_position );
    out_vertexcolor = in_color.rgb;
}
