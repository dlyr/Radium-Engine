layout( vertices = 3 ) out;

in vec3 v_position[];
in vec3 v_normal[];
in vec3 v_tangent[];
in vec3 v_bitangent[];
in vec3 v_texcoord[];
in vec4 v_color[];

out vec3 tcs_position[];
out vec3 tcs_normal[];
out vec3 tcs_tangent[];
out vec3 tcs_bitangent[];
out vec3 tcs_texcoord[];
out vec4 tcs_color[];

in gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
}
gl_in[];

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
}
gl_out[];

#define ID gl_InvocationID

void main() {
    gl_TessLevelOuter[0] = 4;
    gl_TessLevelOuter[1] = 4;
    gl_TessLevelOuter[2] = 4;
    gl_TessLevelInner[0] = 4;

    tcs_position[ID]  = v_position[ID];
    tcs_normal[ID]    = v_normal[ID];
    tcs_tangent[ID]   = v_tangent[ID];
    tcs_bitangent[ID] = v_bitangent[ID];
    tcs_texcoord[ID]  = v_texcoord[ID];
    tcs_color[ID]     = v_color[ID];
}
