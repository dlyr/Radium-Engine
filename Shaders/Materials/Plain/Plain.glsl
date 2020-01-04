#ifndef GLSL_PLAIN_MATERIAL
#define GLSL_PLAIN_MATERIAL
// Assume that plain rendering is based on very simple "material"
// This material could be used with or without lighting.
// When used with lighting, behave like a pure lambertian material.
// When used without lighting, render as a plain, flat color
struct PlainTextures
{
    int hasColor;
    int hasMask;

    sampler2D color;
    sampler2D mask;
};

struct Material
{
    vec4 color;
    PlainTextures tex;

    int perVertexColor;
    int shaded;
};


//------------------- VertexAttrib interface ---------------------
vec4 getPerVertexBaseColor();
vec3 getWorldSpaceNormal();

//----------------------------------------------------------------
const float Pi = 3.141592653589793;

vec4 getBaseColor(Material material, vec2 texCoord)
{
    vec4 dc = vec4 (material.color.rgb, 1);

    if (material.perVertexColor == 1)
    {
        dc.rgb = getPerVertexBaseColor().rgb;
    }
    if (material.tex.hasColor == 1)
    {
        dc.rgb = texture(material.tex.color, texCoord).rgb;
    }

    if (material.tex.hasMask == 1 && texture(material.tex.mask, texCoord).r < 0.1) {
        dc.a = 0;
    }
    return dc;
}

// diffuseColor is basecolor
vec4 getDiffuseColor(Material material, vec2 texCoord) {
    return getBaseColor(material, texCoord);
}

// specular color is black
vec3 getSpecularColor(Material material, vec2 texCoord) {
    return vec3(0);
}

// wi (light direction) and wo (view direction) are in local frame
// wi dot N is then wi.z ...
vec3 evaluateBSDF(Material material, vec2 texC, vec3 l, vec3 v) {
    return max(l.z, 0.0) * getDiffuseColor(material, texC).rgb / Pi;
}

// Return the world-space normal computed according to the microgeometry definition`
// As no normal map is defined, return N
vec3 getNormal(Material material, vec2 texCoord, vec3 N, vec3 T, vec3 B) {
    return N;
}

// return true if the fragment must be condidered as transparent (either fully or partially)
bool toDiscard(Material material, vec4 color) {
    return (color.a < 0.1);
}

uniform Material material;

#endif