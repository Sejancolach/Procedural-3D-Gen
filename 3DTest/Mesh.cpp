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
#include "Timer.hpp"

void Mesh::SetVertices(const std::vector<GLfloat>& verts) {
	vertices.clear();
	vertices = verts;
	triangleCount = vertices.size() / 3;
	if(!CanUpdateBuffers) return;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
}

void Mesh::SetIndice(const std::vector<uint32_t>& tris, int idx) {
	indices.clear();
	indices[idx] = tris;
	if(!CanUpdateBuffers) return;
	free(indiceBuffer);
	indiceBuffer = new GLuint[indices.size()];
	glGenBuffers(indices.size(), indiceBuffer);
	for(int i = 0; i < indices.size(); i++) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(uint32_t), &indices[i][0], GL_STATIC_DRAW);
	}
}

void Mesh::SetIndices(const std::vector<std::vector<uint32_t>>& tris) { 
	indices.clear();
	indices = tris;
	if(!CanUpdateBuffers) return;
	free(indiceBuffer);
	indiceBuffer = new GLuint[indices.size()];
	glGenBuffers(indices.size(), indiceBuffer);
	for(int i = 0; i < indices.size(); i++) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(uint32_t), &indices[i][0], GL_STATIC_DRAW);
	}
}

void Mesh::AddIndices(const std::vector<uint32_t>& tris) { 
	indices.push_back(tris);
	if(!CanUpdateBuffers) return;
	free(indiceBuffer);
	indiceBuffer = new GLuint[indices.size()];
	glGenBuffers(indices.size(), indiceBuffer);
	for(int i = 0; i < indices.size(); i++) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(uint32_t), &indices[i][0], GL_STATIC_DRAW);
	}
}

void Mesh::SetNormals(const std::vector<glm::vec3>& normals) {
	this->normals.clear();
	this->normals = normals;
	if(!CanUpdateBuffers) return;
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals[0], GL_STATIC_DRAW);
}

void Mesh::SetUVs(const std::vector<glm::vec2>& uvs) { 
	this->uvs.clear();
	this->uvs = uvs;
	if(!CanUpdateBuffers) return;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, this->uvs.size() * sizeof(glm::vec2), &this->uvs[0], GL_STATIC_DRAW);
}

void Mesh::UpdateBuffers() { 
	if(vertices.size() != 0) {
		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(GLfloat), &this->vertices[0], GL_STATIC_DRAW);
	}
	if(indices.size() != 0) {
		free(indiceBuffer);
		indiceBuffer = new GLuint[indices.size()];
		glGenBuffers(indices.size(), indiceBuffer);
		for(int i = 0; i < indices.size(); i++) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(uint32_t), &indices[i][0], GL_STATIC_DRAW);
		}
	}
	if(normals.size() != 0) {
		glGenBuffers(1, &normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals[0], GL_STATIC_DRAW);
	}
	if(uvs.size() != 0) {
		glGenBuffers(1, &uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, this->uvs.size() * sizeof(glm::vec2), &this->uvs[0], GL_STATIC_DRAW);
	}
}

void Mesh::RecalculateNormals(void) {
	std::vector<glm::vec3> normals(triangleCount);
	for(int idx = 0; idx < indices.size(); idx++) {
		for(int i = 0, j = 0; i < indices[idx].size() / 3; i++) {
			glm::vec3 normalVector;
			glm::vec3 a = glm::vec3(vertices[indices[idx][j] * 3]	 , vertices[indices[idx][j] * 3 + 1]	, vertices[indices[idx][j] * 3 + 2]);
			glm::vec3 b = glm::vec3(vertices[indices[idx][j + 1] * 3], vertices[indices[idx][j + 1] * 3 + 1], vertices[indices[idx][j + 1] * 3 + 2]);
			glm::vec3 c = glm::vec3(vertices[indices[idx][j + 2] * 3], vertices[indices[idx][j + 2] * 3 + 1], vertices[indices[idx][j + 2] * 3 + 2]);
			normalVector = (glm::cross(a - b, c - b));
			normals[indices[idx][j]] += normalVector;
			normals[indices[idx][j + 1]] += normalVector;
			normals[indices[idx][j + 2]] += normalVector;
			j += 3;
		}
	}
	for(int i = 0; i < triangleCount; i++) {
		normals[i] = glm::normalize(normals[i]);
	}
	SetNormals(normals);
}

void Mesh::SmoothNormals(int repeat, float factor) { 
	static const float oneOverThree = 1.f / 3.f;
	std::vector<glm::vec3> newNormals(normals);
	for(int k = 0; k < repeat; k++) {
		for(int idx = 0; idx < indices.size(); idx++) {
			for(int i = 0, j = 0; i < indices[idx].size() / 3; i++) {
				glm::vec3 smoothedNormal{ 0,0,0 };
				smoothedNormal += newNormals[indices[idx][j]];
				smoothedNormal += newNormals[indices[idx][j + 1]];
				smoothedNormal += newNormals[indices[idx][j + 2]];
				smoothedNormal *= oneOverThree * factor;
				newNormals[indices[idx][j]] += smoothedNormal;
				newNormals[indices[idx][j + 1]] += smoothedNormal;
				newNormals[indices[idx][j + 2]] += smoothedNormal;
				newNormals[indices[idx][j]]		= glm::normalize(newNormals[indices[idx][j]]);
				newNormals[indices[idx][j + 1]] = glm::normalize(newNormals[indices[idx][j + 1]]);
				newNormals[indices[idx][j + 2]] = glm::normalize(newNormals[indices[idx][j + 2]]);
				j += 3;
			}
		}
	}
	for(int i = 0; i < triangleCount; i++) {
		newNormals[i] = glm::normalize(newNormals[i]);
	}
	SetNormals(newNormals);
}
/*
void Mesh::Optimize(void) { 
	std::vector<GLfloat> nVert;
	std::unordered_set <uint32_t> duplicated;
	std::vector<std::vector<uint32_t>> nIndices(indices);

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

	uint32_t hInd = 0;
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
*/
void Mesh::Optimize(void) { return; }

Mesh Mesh::CreatePrimitiveBox(float size) {
	Mesh mesh;
	size = size / 2;
	const std::vector<GLfloat> vert = {-size,size,-size,size,size,-size,-size,-size,-size,size,-size,-size,-size,size,size,size,size,size,-size,-size,size,size,-size,size };
	//const std::vector<uint32_t> ind = { 0,1,2,2,1,3,4,0,6,6,0,2,7,5,6,6,5,4,3,1,7,7,1,5,4,5,0,0,5,1,3,7,2,2,7,6 };
	const std::vector<std::vector<uint32_t>> ind = { { 2,1,0,3,1,2,6,0,4,2,0,6,6,5,7,4,5,6,7,1,3,5,1,7,0,5,4,1,5,0,2,7,3,6,7,2 } };
	mesh.SetVertices(vert);
	mesh.SetIndices(ind);
	mesh.RecalculateNormals();
	return mesh;
}

/*
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
	std::vector<uint32_t> nIndices;
	float* v0, * v1, * v2, * v3, * v4, * v11;
	uint32_t idx = 0;
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
}*/


Mesh Mesh::CreateFromAlgorithm(int dimension, int size, int detailLevel, std::function<float(float, float)> func, bool canUpdateBuffer) {
	Mesh mesh;
	mesh.CanUpdateBuffers = canUpdateBuffer;
	bool dx = false;
	float dl = 1.0f / detailLevel;
	int i = 0;
	int ox = 0;
	float noiseScale = 1.0f / size;
	int oxsize = dimension * detailLevel;
	std::vector<float> vertices((oxsize + 1) * (oxsize + 1) * 3);
	std::vector<std::vector<uint32_t>> indices(1);
	indices[0] = std::vector<uint32_t>(oxsize * oxsize * 6);
	std::vector<glm::vec3> normals(vertices.size() / 3);

	for(float x = 0; x <= dimension; x += dl) {
		for(float z = 0; z <= dimension; z += dl) {
			float rx = x * noiseScale, rz = z * noiseScale, rdl = dl * noiseScale;
			vertices[i] = x;
			vertices[i + 1] = func(rx, rz);
			vertices[i + 2] = z;
			i += 3;
		}
	}
	
	for(int ti = 0, vi = 0, y = 0; y < oxsize; y++, vi++) {
		for(int x = 0; x < oxsize; x++, ti += 6, vi++) {
			indices[0][ti] = vi;
			indices[0][ti + 3] = indices[0][ti + 2] = vi + 1;
			indices[0][ti + 4] = indices[0][ti + 1] = vi + oxsize + 1;
			indices[0][ti + 5] = vi + oxsize + 2;
		}
	}

	int nsize = sqrt( normals.size());
	for(int i = 0; i < nsize; i++) {
		const int npow = nsize * nsize;
		normals[i] = glm::vec3(0, .05f, 0);
		normals[npow - i - 1] = glm::vec3(0, .05f, 0);
		normals[i * nsize] = glm::vec3(0, .05f, 0);
		normals[(i+1) * nsize -1] = glm::vec3(0, .05f, 0);
	}
	for(int i = 0, j = 0; i < indices[0].size() / 3; i++) {
		glm::vec3 normalVector;
		glm::vec3 a = glm::vec3(vertices[indices[0][j] * 3]	   , vertices[indices[0][j] * 3 + 1]	, vertices[indices[0][j] * 3 + 2]);
		glm::vec3 b = glm::vec3(vertices[indices[0][j + 1] * 3], vertices[indices[0][j + 1] * 3 + 1], vertices[indices[0][j + 1] * 3 + 2]);
		glm::vec3 c = glm::vec3(vertices[indices[0][j + 2] * 3], vertices[indices[0][j + 2] * 3 + 1], vertices[indices[0][j + 2] * 3 + 2]);
		normalVector = (glm::cross(a - b, c - b));
		normals[indices[0][j]] += normalVector;
		normals[indices[0][j + 1]] += normalVector;
		normals[indices[0][j + 2]] += normalVector;
		j += 3;
	}
	for(int i = 0; i < vertices.size() / 3; i++) {
		normals[i] = glm::normalize(normals[i]);
	}
	//printf("vrts: %d inds: %d ns: %d\n", vertices.size(), indices.size(), normals.size());
	//for(int i = 0; i < indices.size(); i += 3) {
	//	printf(" %d -> %d %d %d\n", i / 3, indices[0][i], indices[0][i + 1], indices[0][i + 2]);
	//	printf("   %d: %f %f %f\n", indices[0][i]	 , vertices[indices[0][i] * 3]	  , vertices[indices[0][i] * 3 + 1]	   , vertices[indices[0][i] * 3 + 2]);
	//	printf("   %d: %f %f %f\n", indices[0][i + 1], vertices[indices[0][i + 1] * 3], vertices[indices[0][i + 1] * 3 + 1], vertices[indices[0][i + 1] * 3 + 2]);
	//	printf("   %d: %f %f %f\n", indices[0][i + 2], vertices[indices[0][i + 2] * 3], vertices[indices[0][i + 2] * 3 + 1], vertices[indices[0][i + 2] * 3 + 2]);
	//}
	//for(int i = 0; i < vertices.size(); i+=3) {
	//	printf(" %d -> (x: %f y: %f z: %f)\n", i/3, vertices[i], vertices[i + 1], vertices[i + 2]);
	//}

	mesh.SetVertices(vertices);
	mesh.SetIndices(indices);
	mesh.SetNormals(normals);
	//mesh.RecalculateNormals();

	return mesh;
}

#include <fstream>
#include <sstream>

//Can only load Triangulated meshes!!!!
Mesh Mesh::LoadOBJ(std::string path) {
	Mesh mesh;
	std::ifstream file(path);
	if(!file) {
		printf("Could not load %s \n", path.c_str());
		return mesh;
	}
	
	std::vector<GLfloat> vertices;
	std::vector<std::vector<uint32_t>> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	std::string line;
	uint32_t triIndex = 0;
	uint16_t IndiceIdx = 0;
	while(std::getline(file, line)) {
		std::stringstream ssl(line);
		char _;
		ssl >> std::noskipws;
		ssl >> _;
		switch(_) {
		case 'v':
		{
			float con[3]{};
			ssl >> _;
			switch(_) {
			case 't': // UV Data		-> vt XXX XXX
				ssl >> _ >> con[0] >> _ >> con[1];
				uvs.push_back({ con[0], con[1] });
				break;
			case 'n': // Vertex Normal	-> vn XXX XXX XXX
				ssl >> _ >> con[0] >> _ >> con[1] >> _ >> con[2];
				normals.push_back({ con[0],con[1],con[2] });
				break;
			default: // Vertex Data	-> v XXX XXX XXX
				ssl >> con[0] >> _ >> con[1] >> _ >> con[2];
				vertices.insert(vertices.end(), { con[0],con[1],con[2] });
				break;
			}
			break;
		}
		case 'f' :
		{  // Indices		-> f V1/U1/N1 V2/U2/N2 V3/U3/N3
			std::string s = ssl.str();
			std::string _d;
			std::replace(s.begin(), s.end(), '/', ' ');
			ssl.str(s);
			uint32_t _in[3]{};
			ssl >> _ >> _ >> _in[0] >> _ >> _d >> _ >> _d >> _ >> _in[1] >> _ >> _d >> _ >> _d >> _ >> _in[2];
			_in[0]--; _in[1]--; _in[2]--;
			indices[IndiceIdx].insert(indices[IndiceIdx].end(), { _in[2],_in[1],_in[0] });
			//uvIndices.insert();
			//normalIndices.insert();
			triIndex++;
			break;
		}
		case 'u':
		{ // usemtl 'Name'
			std::string _d;
			ssl >> _d;
			if(strcmp(_d.c_str(), "semtl") == 0) {
				ssl >> _ >> _d;
				printf("Material %s found at index %d\n", _d.c_str(),triIndex);
				IndiceIdx += triIndex != 0 ? 1 : 0;
				indices.push_back(std::vector<uint32_t>(0));
				// TODO: Support this in the MeshRenderer
			}
		}
		}
	}
	mesh.CanUpdateBuffers = false;
	mesh.SetVertices(vertices);
	mesh.SetNormals(normals);
	mesh.SetIndices(indices);
	mesh.SetUVs(uvs);
	mesh.UpdateBuffers();
	mesh.CanUpdateBuffers = true;
	return mesh;
}
