in vec4 vertex;
out vec2 TexCoords;
uniform mat4 text_matrix;

void main()
{
	gl_Position = text_matrix * vec4(vertex.xy, -0.1, 1.0);
	TexCoords = vertex.zw;
}