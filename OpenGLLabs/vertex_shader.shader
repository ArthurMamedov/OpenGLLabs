//#version 330 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//layout (location = 2) in vec2 aTexCoord;
//
//out vec3 ourColor;
//out vec2 TexCoord;
//
//void main()
//{
//    gl_Position = vec4(aPos, 1.0);
//    ourColor = aColor;
//    TexCoord = aTexCoord;
//}
#version 430 core
layout(location = 0) in vec3 position;
out vec4 vertexColor;
uniform mat4 transform;

void main() {
	gl_Position = transform * vec4(position, 1.0);
	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}