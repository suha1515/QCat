#pragma once
#include "QCat/Renderer/Buffer.h"

#define LEAF_ELEMENT_TYPESGL \
	X( Float ) \
	X( Float2 ) \
	X( Float3 ) \
	X( Float4 ) \
	X( Mat3 ) \
	X( Mat4 ) \
	X( Int ) \
	X( Int2 ) \
	X( Int3 ) \
	X( Int4 ) \
	X( Bool )

namespace QCat
{
	template<ShaderDataType type>
	struct MapGL
	{
		static constexpr bool valid = false;
	};
	template<> struct MapGL<ShaderDataType::Float>
	{
		using SystemType = float;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "f1";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Float2>
	{
		using SystemType = glm::vec2;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "f2";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Float3>
	{
		using SystemType = glm::vec3;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "f3";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Float4>
	{
		using SystemType = glm::vec4;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "f4";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Mat3>
	{
		using SystemType = glm::mat3;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "m3";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Mat4>
	{
		using SystemType = glm::mat4;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "m4";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Int>
	{
		using SystemType = int;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "i1";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Int2>
	{
		using SystemType = glm::ivec2;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "i2";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Int3>
	{
		using SystemType = glm::ivec3;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "i3";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Int4>
	{
		using SystemType = glm::ivec4;
		static constexpr size_t size = sizeof(SystemType);
		static constexpr const char* code = "i4";
		static constexpr bool valid = true;
	};
	template<> struct MapGL<ShaderDataType::Bool>
	{
		using SystemType = bool;
		static constexpr size_t size = 4u;
		static constexpr const char* code = "bl";
		static constexpr bool valid = true;
	};
	// ensures that every leaf type in master list has an entry in the static attribute map
	#define X(el) static_assert(MapGL<ShaderDataType::el>::valid,"Missing map implementation for " #el);
	LEAF_ELEMENT_TYPESGL
	#undef X
	// Reverse
	// enables reverse lookup from SysType to leaf type
		template<typename T>
	struct ReverseMapGL
	{
		static constexpr bool valid = false;
	};
	/*template<> struct ReverseMapGL<typename MapGL<ShaderDataType::Float>::SystemType>
	{
		static constexpr ShaderDataType type = ShaderDataType::Float; 
		static constexpr bool valid = true; 
	};*/
#define X(el) \
	template<> struct ReverseMapGL<typename MapGL<ShaderDataType::el>::SystemType> \
	{ \
		static constexpr ShaderDataType type = ShaderDataType::el; \
		static constexpr bool valid = true; \
	};
	LEAF_ELEMENT_TYPESGL
#undef X

}

