// Request GLSL 3.3
#version 330

// Attribute 0 is position
layout (location = 0) in vec2 inPosition;

// Attribute 1 is texture coordinate
layout(location = 1) in vec2 inTexCoord;

uniform mat4 uWorldTransform;
uniform mat4 uOrthoProj;
uniform vec2 uCameraPos;

uniform vec4 uTexRect;

// Any vertex outputs (other than position)
out vec2 fragTexCoord;

void main()
{
    vec4 worldPos = uWorldTransform * vec4(inPosition, 0.0, 1.0);

    worldPos.xy -= uCameraPos;

	gl_Position = uOrthoProj * worldPos;

    fragTexCoord = inTexCoord * uTexRect.zw + uTexRect.xy;
}