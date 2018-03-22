#include "assimp.h"

namespace libAi {
	mat4 toNativeType(const aiMatrix4x4& aiMat)
	{
		const float values[16] = {
			aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
			aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
			aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
			aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
		};

		return glm::make_mat4<float>(values);
	}

	vec3 toNativeType(const aiVector3D& aiVec)
	{
		return vec3(aiVec.x, aiVec.y, aiVec.z);
	}

	quat toNativeType(const aiQuaternion& aiQuat)
	{
		return quat(aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z);
	}

	void print(const aiMatrix4x4& aiMat)
	{
		using namespace std;

		auto fmt = [](float number) { return to_string(number).length(); };

		int aColMax = std::max({ fmt(aiMat.a1), fmt(aiMat.a2), fmt(aiMat.a3), fmt(aiMat.a4) });
		int bColMax = std::max({ fmt(aiMat.b1), fmt(aiMat.b2), fmt(aiMat.b3), fmt(aiMat.b4) });
		int cColMax = std::max({ fmt(aiMat.c1), fmt(aiMat.c2), fmt(aiMat.c3), fmt(aiMat.c4) });
		int dColMax = std::max({ fmt(aiMat.d1), fmt(aiMat.d2), fmt(aiMat.d3), fmt(aiMat.d4) });

		const int indent = 2;

		console::info("mat:");
		std::cout << "[";
		std::cout << std::setw(aColMax) << aiMat.a1;
		std::cout << std::setw(bColMax+indent) << aiMat.b1;
		std::cout << std::setw(cColMax+indent) << aiMat.c1;
		std::cout << std::setw(dColMax+indent) << aiMat.d1;
		std::cout << "]";
		std::cout << std::endl;
		std::cout << "[";
		std::cout << std::setw(aColMax) << aiMat.a2;
		std::cout << std::setw(bColMax+indent) << aiMat.b2;
		std::cout << std::setw(cColMax+indent) << aiMat.c2;
		std::cout << std::setw(dColMax+indent) << aiMat.d2;
		std::cout << "]";
		std::cout << std::endl;
		std::cout << "[";
		std::cout << std::setw(aColMax) << aiMat.a3;
		std::cout << std::setw(bColMax+indent) << aiMat.b3;
		std::cout << std::setw(cColMax+indent) << aiMat.c3;
		std::cout << std::setw(dColMax+indent) << aiMat.d3;
		std::cout << "]";
		std::cout << std::endl;
		std::cout << "[";
		std::cout << std::setw(aColMax) << aiMat.a4;
		std::cout << std::setw(bColMax+indent) << aiMat.b4;
		std::cout << std::setw(cColMax+indent) << aiMat.c4;
		std::cout << std::setw(dColMax+indent) << aiMat.d4;
		std::cout << "]";
		std::cout << std::endl;
	}

	void print(const aiVector3D& aiVec)
	{
		console::info("vec: %.2f %.2f %.2f", aiVec.x, aiVec.y, aiVec.z);
	}

    void print(const aiQuaternion& aiQuat)
	{
		console::info("quat: %.2f %.2f %.2f %.2f", aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
	}
}
