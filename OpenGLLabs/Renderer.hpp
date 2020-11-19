#pragma once
#include <vector>
#include "Shader.hpp"
#include "Texture.hpp"

class Renderer final {
private:
	unsigned int _VBO;
	unsigned int _VAO;
	unsigned int _dim;
	unsigned int _tex_dim;
	const Shader* _shader;
	const Texture* _texture;
	const std::vector<float>* _vertices;

public:
	Renderer(const std::vector<float>* data, unsigned int tex_dim, unsigned int dim, const Shader* shader, const Texture* texture);
	template<class Fn, class... Args>
	auto render(const Fn& func, Args... args) -> void;
	auto change_texture(const Texture* new_texture) -> void;
	auto change_shader(const Shader* new_shader) -> void;
	auto change_object(const std::vector<float>* vertices) -> void;
};

template<class Fn, class ...Args>
inline auto Renderer::render(const Fn& func, Args ...args) -> void {
	glActiveTexture(GL_TEXTURE0);
	_texture->select();
	_shader->select();
	func((unsigned int)(*_shader), args...);
	glBindVertexArray(_VAO);
	glDrawArrays(GL_TRIANGLES, 0, _vertices->size() / (_dim + _tex_dim));
}
