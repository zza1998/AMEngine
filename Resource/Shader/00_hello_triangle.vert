#version 450

out gl_PerVertex{
    vec4 gl_Position;
};

vec3 vertices[] = {
    vec3(0, 0, 0),
    vec3(0, 0.5, 0),
    vec3(-0.5, 0, 0)
};

vec4 colors[] = {
    vec4(1, 0, 0, 1),
    vec4(0, 1, 0, 1),
    vec4(0, 0, 1, 1)
};

out layout(location=0) vec4 vertexColor;

void main(){
    vec3 in_pos = vertices[gl_VertexIndex];
    gl_Position = vec4(in_pos.x, -in_pos.y, in_pos.z, 1.f);
    vertexColor = colors[gl_VertexIndex];
}