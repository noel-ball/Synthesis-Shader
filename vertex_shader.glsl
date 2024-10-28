#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

uniform vec2 screenSize;

out vec3 fragColor;

void main() {
    vec2 scaledPos = (position / (screenSize * 0.5)) - 1.0; // Convert to NDC
    gl_Position = vec4(scaledPos, 0.0, 1.0);
    fragColor = color;
}
