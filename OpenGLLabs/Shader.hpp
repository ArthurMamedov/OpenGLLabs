#pragma once
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Shader final {
private:
	unsigned int _id;
	auto compile_shader(unsigned int type, const std::string& source) -> unsigned int;
	auto create_shader(const std::string& vertex_shader, const std::string& fragment_shader) -> unsigned int;

public:
	Shader() :_id(0) {}
	Shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
	auto get_id() const -> unsigned int;
	auto select() const -> void;
	explicit operator unsigned int() const;
};
