#version 330

// Input from vertex shader
in vec2 fragTexCoord;
in vec4 fragColor;

// Output
out vec4 finalColor;

void main()
{
    // Simple emissive shader for light visualization
    finalColor = vec4(1.0, 1.0, 0.5, 1.0); // Bright yellow color
}
