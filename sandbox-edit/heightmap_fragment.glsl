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

    if ((h >= 0.0f) && (h < 0.6f))
    {
        FragColor = texture(texture1, TexCoord);
    }
    if ((h >= 0.6f) && (h < 0.8f))
    {
        FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);

    }

    if ((h >= 0.8f) && (h < 1.2f))
    {
        FragColor = texture(texture2, TexCoord);
    }

    if ((h >= 1.2f) && (h < 1.5f))
    {
        FragColor = mix(texture(texture2, TexCoord), texture(texture3, TexCoord), 0.5);

    }
    if ((h >= 1.5f) && (h <= 2f))
    {
        FragColor = texture(texture3, TexCoord);
    }

}