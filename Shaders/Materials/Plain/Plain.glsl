// Assume that plain rendering is based on very simple "material", without lighting.
struct PlainTextures
{
    int hasKd;
    int hasAlpha;

    sampler2D kd;
    sampler2D alpha;
};

struct Material
{
    vec4 kd;
    float alpha;
    PlainTextures tex;
};

uniform Material material;
