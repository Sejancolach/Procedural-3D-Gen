#include "Mesh.hpp"
#include <GL/glew.h>
#include <math.h>
#include <glm/geometric.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <set>
#include <unordered_set>
#include <map>

void Mesh::SetVertices(const std::vector<GLfloat>& verts) {
	vertices.clear();
	vertices = verts;
	triangleCount = vertices.size() / 3;
	if(!CanUpdateBuffers) return;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4, &vertices[0], GL_STATIC_DRAW);
}

void Mesh::SetIndices(const std::vector<unsigned short>& tris) {
	indices.clear();
	indices = tris;
	if(!CanUpdateBuffers) return;
	glGenBuffers(1, &indiceBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
}

void Mesh::SetNormals(const std::vector<glm::vec3>& normals) {
	this->normals.clear();
	this->normals = normals;
	if(!CanUpdateBuffers) return;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals[0], GL_STATIC_DRAW);
}

void Mesh::UpdateBuffers() { 
	if(vertices.size() > 0) {
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * 4, &vertices[0], GL_STATIC_DRAW);
	}
	if(indices.size() > 0) {
		glGenBuffers(1, &indiceBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
	}
	if(normals.size() > 0) {
		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals[0], GL_STATIC_DRAW);
	}
}

void Mesh::RecalculateNormals(void) {
	std::vector<glm::vec3> normals(triangleCount);
	for(int i = 0, j = 0; i < indices.size() / 3; i++) {
		glm::vec3 normalVector;
		glm::vec3 a = glm::vec3(vertices[indices[j] * 3], vertices[indices[j] * 3 + 1], vertices[indices[j] * 3 + 2]);
		glm::vec3 b = glm::vec3(vertices[indices[j + 1] * 3], vertices[indices[j + 1] * 3 + 1], vertices[indices[j + 1] * 3 + 2]);
		glm::vec3 c = glm::vec3(vertices[indices[j + 2] * 3], vertices[indices[j + 2] * 3 + 1], vertices[indices[j + 2] * 3 + 2]);
		normalVector = (glm::cross(b - a, c - a));
		normals[indices[j]] += normalVector;
		normals[indices[j + 1]] += normalVector;
		normals[indices[j + 2]] += normalVector;
		j += 3;
	}
	for(int i = 0; i < triangleCount; i++) {
		normals[i] = glm::normalize(normals[i]);
	}
	SetNormals(normals);
}

void Mesh::SmoothNormals(void) { 
	static const float oneOverThree = 1.f / 3.f;
	std::vector<glm::vec3> newNormals(normals);
	for(int k = 0; k < 3; k++) {
		for(int i = 0, j = 0; i < indices.size() / 3; i++) {
			glm::vec3 smoothedNormal{0,0,0};
			smoothedNormal += newNormals[indices[j]];
			smoothedNormal += newNormals[indices[j + 1]];
			smoothedNormal += newNormals[indices[j + 2]];
			smoothedNormal *= oneOverThree;
			newNormals[indices[j]] += smoothedNormal;
			newNormals[indices[j + 1]] += smoothedNormal;
			newNormals[indices[j + 2]] += smoothedNormal;
			newNormals[indices[j]] = glm::normalize(newNormals[indices[j]]);
			newNormals[indices[j+1]] = glm::normalize(newNormals[indices[j+1]]);
			newNormals[indices[j+2]] = glm::normalize(newNormals[indices[j+2]]);
			j += 3;
		}
	}
	for(int i = 0; i < triangleCount; i++) {
		newNormals[i] = glm::normalize(newNormals[i]);
	}
	SetNormals(newNormals);
}

void Mesh::Optimize(void) { 
	std::vector<GLfloat> nVert;
	std::unordered_set <unsigned short> duplicated;
	std::vector<unsigned short> nIndices(indices);

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	for(int i = 0, c = 0; i < vertices.size(); i += 3) {
		bool hasDuplicate = false;
		for(int j = i + 3; j < vertices.size(); j += 3) {
			if(duplicated.count(i) > 0) {
				hasDuplicate = true;
				break;
			}
			else if (duplicated.count(j) > 0)
				continue;
			if(vertices[i] != vertices[j] or vertices[i + 1] != vertices[j + 1] or vertices[i + 2] != vertices[j + 2])
				continue;

			if(!hasDuplicate) {
				nVert.push_back(vertices[i]);
				nVert.push_back(vertices[i + 1]);
				nVert.push_back(vertices[i + 2]);
				c += 3;
				hasDuplicate = true;
			}
			for(int k = 0; k < nIndices.size(); k++) {
				if(nIndices[k] == j/3) {
					nIndices[k] = i/3;
				}
			}
			duplicated.insert(j);
		}
		if(!hasDuplicate) {
			nVert.push_back(vertices[i]);
			nVert.push_back(vertices[i + 1]);
			nVert.push_back(vertices[i + 2]);
		}
	}
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	unsigned short hInd = 0;
	std::vector<bool> mInd(nIndices.size());
	std::vector<unsigned int> duplicatedSet;
	for(int i = 0; i < nIndices.size(); i++) {
		mInd[nIndices[i]] = true;
		if(hInd < nIndices[i]) hInd = nIndices[i];
	}

	for(int i = 0; i < hInd; i++) {
		if(!mInd[i]) {
			duplicatedSet.push_back(i);
		}
	}

	for(int i = 0; i < nIndices.size(); i++) {
		for(int j = duplicatedSet.size(); j >= 0; j--) {
			if(nIndices[i] > duplicatedSet[j])
				nIndices[i]--;
		}
	}

	//std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
	//std::cout << " time 1 -> " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "mrs\t"<<
	//	std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" <<std::endl;
	//std::cout << " time 2 -> " << std::chrono::duration_cast<std::chrono::microseconds>(end2 - end).count() << "mrs\t" <<
	//	std::chrono::duration_cast<std::chrono::milliseconds>(end2 - end).count() << "ms" << std::endl;

	//for(int i = 0; i < nIndices.size(); i+=3) {
	//	printf(" %d -> %d %d %d\n", i/3, nIndices[i], nIndices[i + 1], nIndices[i + 2]);
	//	printf("   %d: %f %f %f\n", nIndices[i], nVert[nIndices[i] * 3], nVert[nIndices[i] * 3] + 1, nVert[nIndices[i] * 3] + 2);
	//	printf("   %d: %f %f %f\n", nIndices[i+1], nVert[nIndices[i+1] * 3], nVert[nIndices[i+1] * 3] + 1, nVert[nIndices[i+1] * 3] + 2);
	//	printf("   %d: %f %f %f\n", nIndices[i+2], nVert[nIndices[i+2] * 3], nVert[nIndices[i+2] * 3] + 1, nVert[nIndices[i+2] * 3] + 2);
	//}

	SetVertices(nVert);
	SetIndices(nIndices);
	RecalculateNormals();
}

Mesh Mesh::CreatePrimitiveIcoSphere(float radius, uint8_t subdivision) {
	Mesh mesh;
	constexpr float PI = 3.1415926f;
	const float HANGLE = PI / 180 * 72;
	const float VANGLE = atanf(1.0f / 2);

	std::vector<float> vertices(12 * 3);
	int i1, i2;
	float z, xy;
	float hAngle1 = -PI / 2 - HANGLE / 2;
	float hAngle2 = -PI / 2;

	vertices[0] = 0; vertices[1] = 0; vertices[2] = radius;

	for(int i = 1; i < 6; i++) {
		i1 = i * 3;
		i2 = (i + 5) * 3;

		z = radius * sinf(VANGLE);
		xy = radius * cosf(VANGLE);

		vertices[i1]	 = xy * cosf(hAngle1);
		vertices[i1 + 1] = xy * sinf(hAngle1);
		vertices[i1 + 2] = z;
		vertices[i2]	 = xy * cosf(hAngle2);
		vertices[i2 + 1] = xy * sinf(hAngle2);
		vertices[i2 + 2] = -z;

		hAngle1 += HANGLE;
		hAngle2 += HANGLE;
	}

	i1 = 11 * 3;
	vertices[i1] = 0;
	vertices[i1+1] = 0;
	vertices[i1+2] = -radius;

	std::vector<float> nVert;
	std::vector<uint16_t> nIndices;
	float* v0, * v1, * v2, * v3, * v4, * v11;
	uint16_t idx = 0;
	v0 = &vertices[0];
	v11 = &vertices[11 * 3];
	for(int i = 0; i <= 5; i++, idx+=12) {
		v1 = &vertices[i * 3];
		v2 = (i < 5) ? &vertices[(i + 1) * 3] : &vertices[3];
		v3 = &vertices[(i + 5) * 3];
		v4 = (i < 5) ? &vertices[(i + 6) * 3] : &vertices[6 * 3];

		AddVertices(nVert, v0, v1, v2);
		AddIndices(nIndices, idx, idx + 1,idx + 2);
		AddVertices(nVert, v1, v3, v2);
		AddIndices(nIndices, idx + 3, idx + 4, idx + 5);
		AddVertices(nVert, v2, v3, v4);
		AddIndices(nIndices, idx + 6, idx + 7, idx + 8);
		AddVertices(nVert, v3, v11, v4);
		AddIndices(nIndices, idx + 9, idx + 10, idx + 11);
	}

	mesh.SetVertices(nVert);
	mesh.SetIndices(nIndices);

	return mesh;
}

void AddIndices(std::vector<unsigned short>& nIndices, uint16_t i1, uint16_t i2, uint16_t i3) {
	nIndices.push_back(i1);
	nIndices.push_back(i2);
	nIndices.push_back(i3);
}

void AddVertices(std::vector<GLfloat>& nVert, float* v0, float* v1, float* v2) {
	nVert.push_back(v0[0]);
	nVert.push_back(v0[1]);
	nVert.push_back(v0[2]);

	nVert.push_back(v1[0]);
	nVert.push_back(v1[1]);
	nVert.push_back(v1[2]);

	nVert.push_back(v2[0]);
	nVert.push_back(v2[1]);
	nVert.push_back(v2[2]);
}

Mesh Mesh::CreateFromAlgorithm(int dimension, int size, int detailLevel, std::function<float(float, float)> func, bool canUpdateBuffer) {
	Mesh mesh;
	mesh.CanUpdateBuffers = canUpdateBuffer;
	std::vector<float> vertices(dimension * dimension * detailLevel * detailLevel * 12);
	std::vector<uint16_t> indices;

	bool dx = false;
	float dl = 1.0f / detailLevel;
	int i = 0;
	int ox = 0;
	float ovsize = 1.0f / size;
	for(float x = 0; x < dimension; x += dl) {
		for(float z = 0; z < dimension; z += dl) {
			float rx = x * ovsize, rz = z * ovsize, rdl = dl * ovsize;
			vertices[i] = x;
			vertices[i + 1] = func(rx, rz);
			vertices[i + 2] = z;

			vertices[i + 3] = x;
			vertices[i + 4] = func(rx, rz + rdl);
			vertices[i + 5] = z + dl;

			vertices[i + 6] = x + dl;
			vertices[i + 7] = func(rx + rdl, rz);
			vertices[i + 8] = z;

			vertices[i + 9] = x + dl;
			vertices[i + 10] = func(rx + rdl, rz + rdl);
			vertices[i + 11] = z + dl;

			AddIndices(indices, ox, ox + 1, ox + 2);
			AddIndices(indices, ox +1, ox + 3, ox +2);
			ox += 4;
			i += 12;
		}
	}

	mesh.SetVertices(vertices);
	mesh.SetIndices(indices);
	mesh.RecalculateNormals();

	return mesh;
}
