
in vec3 tcs_position[];
in vec3 tcs_normal[];
in vec3 tcs_texcoord[];
in vec3 tcs_vertexcolor[];
in vec3 tcs_tangent[];
in vec3 tcs_viewVector[];
in vec3 tcs_lightVector[];

layout( vertices = 3 ) out;

out vec3 tes_position[];
out vec3 tes_normal[];
out vec3 tes_texcoord[];
out vec3 tes_vertexcolor[];
out vec3 tes_tangent[];
out vec3 tes_viewVector[];
out vec3 tes_lightVector[];

void main() {
    gl_TessLevelOuter[0] = 0;
    gl_TessLevelOuter[1] = 0;
    gl_TessLevelOuter[2] = 0;
    gl_TessInnerLevel[0] = 0;

    tes_position[gl_InvocationID]    = tcs_position[gl_InvocationID];
    tes_normal[gl_InvocationID]      = tcs_normal[gl_InvocationID];
    tes_texcoord[gl_InvocationID]    = tcs_texcoord[gl_InvocationID];
    tes_vertexcolor[gl_InvocationID] = tcs_vertexcolor[gl_InvocationID];
    tes_tangent[gl_InvocationID]     = tcs_tangent[gl_InvocationID];
    tes_viewVector[gl_InvocationID]  = tcs_viewVector[gl_InvocationID];
    tes_lightVector[gl_InvocationID] = tcs_lightVector[gl_InvocationID];
}
