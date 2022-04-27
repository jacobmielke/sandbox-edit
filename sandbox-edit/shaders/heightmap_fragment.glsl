#version 330 core
out vec4 FragColor;

in float Height;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

void main()
{
    float h = (Height + 16)/32.0f;	// shift and scale the height in to a grayscale value
    FragColor = texture(texture1, TexCoord); //vec4(h, h, h, 1.0);
    float range0 = 0.0f; // Min
    float range1 = 0.2f; // Layer 1
    float range2 = 0.4f; // Mix layer 1 and 2
    float range3 = 0.6f; // Layer 2
    float range4 = 1.0f; // Mix layer 2 and 3
    float range5 = 1.0f; // Layer 3

    vec4 dirt = texture(texture1, TexCoord);
    vec4 grass = texture(texture2, TexCoord);
    vec4 stone = texture(texture3, TexCoord);

    if(h < range1)
    {
        FragColor = dirt;
    }
    else if(h < range2)
    {
        FragColor = mix(dirt, grass, (h - range1) / (range2 - range1));
    }
    else if(h < range3)
    {
        FragColor = grass;
    }
    else if(h < range4)
    {
        FragColor = mix(grass, stone, (h - range3) / (range4 - range3));
    }
    else
    {
        FragColor = texture(texture3, TexCoord);
    }
}