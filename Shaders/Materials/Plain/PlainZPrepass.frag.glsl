#include "Plain.glsl"

// if this is really needed, include "DefaultLight.glsl"
//uniform Light light;

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_texcoord;
layout (location = 2) in vec3 in_color;

layout (location = 0) out vec4 out_ambient;
layout (location = 1) out vec4 out_normal;
layout (location = 2) out vec4 out_diffuse;
layout (location = 3) out vec4 out_specular;

void main()
{
    if (material.tex.hasAlpha == 1 && texture(material.tex.alpha, in_texcoord.st).r < 0.1)
    {
        discard;
    }

    if (material.tex.hasKd == 1)
    {
        out_diffuse = vec4(texture(material.tex.kd, in_texcoord.st).rgb, 1);
    }
    else
    {
        out_diffuse = vec4(in_color.rgb, 1.0);
    }
}

