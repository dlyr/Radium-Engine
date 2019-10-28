/*****
*
*   Implementation of the interface to acces interpolated attributes from vertex attributes.
*   This inteface MUST be used ONLY on fragment shaders.
*
*****/

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_texcoord;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_viewVector;
layout (location = 5) in vec3 in_lightVector;
layout (location = 6) in vec3 in_vertexColor;

//------------------- VertexAttrib interface ---------------------
vec4 getWorldSpacePosition() {
    return vec4(in_position, 1.0);
}

vec3 getWorldSpaceNormal() {
    if (length(in_normal.xyz) < 0.0001) {
        vec3 X = dFdx(in_position);
        vec3 Y = dFdy(in_position);
        return normalize(cross(X, Y));
    } else {
        return normalize(in_normal);
    }
}

vec3 getWorldSpaceTangent() {
    vec3 res;
    // if tangent is not set, teh value should be (0, 0, 0, 1).
    // So, test for the length of in_tangent.xyz to be very small and compute the tangent from geo derivatives
    if (length(in_tangent.xyz) < 0.0001) {
        // assume tangents are not there
        res =  dFdx(in_position);
    } else {
        res = in_tangent;
    }
    return normalize(res);
}

vec3 getWorldSpaceBiTangent() {
    return normalize(cross(in_normal, in_tangent));
}

vec3 getPerVertexTexCoord() {
    return in_texcoord;
}

vec4 getPerVertexBaseColor() {
    return vec4(in_vertexColor, 1.0);
}
