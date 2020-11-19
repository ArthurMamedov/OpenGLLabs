#include "Renderer.hpp"

Renderer::Renderer(const std::vector<float>* data, unsigned int tex_dim, unsigned int dim, const Shader* shader, const Texture* texture) : _dim(dim), _vertices(data), _shader(shader), _tex_dim(tex_dim), _texture(texture) {
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, _vertices->size() * sizeof(float), _vertices->data(), GL_STATIC_DRAW);

	auto stride = (_tex_dim + _dim) * sizeof(float);

	glVertexAttribPointer(0, _dim, GL_FLOAT, false, stride, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, _tex_dim, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

auto Renderer::change_texture(const Texture* new_texture) -> void {
	_texture = new_texture;
}

auto Renderer::change_shader(const Shader* new_shader) -> void {
	_shader = new_shader;
}

auto Renderer::change_object(const std::vector<float>* vertices) -> void {
	_vertices = vertices;
}