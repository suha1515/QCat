#pragma once
#include "Float16.h"

namespace QCat
{
	class f16Vec4
	{
	public:
		f16Vec4() = default;
		f16Vec4(float x, float y, float z, float w)
			:x(x),y(y),z(z),w(w)
		{}
		
		f16Vec4& operator=(glm::vec4 vec){
			x = vec.r;
			y = vec.g;
			z = vec.b;
			w = vec.a;

			return *this;
		}

		Float16 x=0.0f, y=0.0f, z=0.0f, w=0.0f;
	};

	namespace Utils
	{
		static float Lerp(float a, float b, float alpha)
		{
			return a * (1 - alpha) + b * alpha;
		}
	}
	
}
