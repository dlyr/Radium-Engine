
layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
}
gl_in[3];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

layout( location = 0 ) in vec3 tes_position[3];
layout( location = 1 ) in vec3 tes_normal[3];
layout( location = 2 ) in vec3 tes_texcoord[3];
layout( location = 3 ) in vec3 tes_vertexcolor[3];
layout( location = 4 ) in vec3 tes_tangent[3];
layout( location = 5 ) in vec3 tes_viewVector[3];
layout( location = 6 ) in vec3 tes_lightVector[3];
layout( location = 7 ) in vec3 tes_patchDistance[3];

// in vec3 tePatchDistance[3];
// out vec3 gFacetNormal;
// out vec3 gPatchDistance;
// out vec3 gTriDistance;
#include "TransformStructs.glsl"

layout( location = 0 ) out vec3 g_position;
layout( location = 1 ) out vec3 g_normal;
layout( location = 2 ) out vec3 g_texcoord;
layout( location = 3 ) out vec3 g_vertexcolor;
#ifndef DONT_USE_INPUT_TANGENT
layout( location = 4 ) out vec3 g_tangent;
#endif

layout( location = 5 ) out vec3 g_viewVector;
layout( location = 6 ) out vec3 g_lightVector;
layout( location = 7 ) out vec3 g_patchDistance;
layout( location = 8 ) out vec3 g_dist;

void copy_data( int id ) {
    g_position    = tes_position[id];
    g_normal      = tes_normal[id];
    g_texcoord    = tes_texcoord[id];
    g_vertexcolor = tes_vertexcolor[id];
    g_tangent     = tes_tangent[id];
    g_viewVector  = tes_viewVector[id];
    g_lightVector = tes_lightVector[id];
}

void main() {
    vec3 A = tes_position[2] - tes_position[0];
    vec3 B = tes_position[1] - tes_position[0];
    // gFacetNormal = NormalMatrix * normalize( cross( A, B ) );

    g_patchDistance = tes_patchDistance[0];
    g_dist          = vec3( 1, 0, 0 );
//    g_normal        = vec3( 1, 1, 1 );
    copy_data( 0 );
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    g_patchDistance = tes_patchDistance[1];
    g_dist          = vec3( 0, 1, 0 );
    //  g_normal=vec3(0,1,1);
    copy_data( 1 );
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    g_patchDistance = tes_patchDistance[2];
    g_dist          = vec3( 0, 0, 1 );
    copy_data( 2 );
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}
