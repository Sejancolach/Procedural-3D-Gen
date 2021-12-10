#include "Mathf.hpp"
#include <glm/geometric.hpp>

namespace Mathf {
	uint32_t Noise1D(int32_t x, uint32_t seed) {
		x *= BIT_NOISE1;
		x += seed;
		x ^= x >> 8;
		x += BIT_NOISE2;
		x ^= x << 8;
		x *= BIT_NOISE3;
		x ^= x >> 8;
		return x;
	}

	float Noise1DF(int32_t x, uint32_t seed) {
		return NormalizeNoiseValue(Noise1D(x,seed));
	}

	float SmoothNoise1DF(float x1, uint32_t seed) {
		float x2 = std::ceil(x1);
		if(x1 == x2) return Noise1DF((uint32_t)x1, seed);
		float s1 = Noise1DF((uint32_t)std::floorf(x1), seed);
		float s2 = Noise1DF((uint32_t)x2, seed);
		float dif = x2 - x1;
		float r = std::lerp(s2, s1, dif);
		r = (r + 1) / 2;
		r = (easeInOutCubic(r) * 2) - 1;
		return r;
	}
	
	uint32_t Noise2D(int32_t x, int32_t y, uint32_t seed) {
		return Noise1D(x + (PRIME1 * y), seed);
	}

	float Noise2DF(int32_t x, int32_t y, uint32_t seed) {
		uint32_t n = Noise2D(x, y, seed);
		return NormalizeNoiseValue(n);
	}

	float SmoothNoise2DF(float x1, float y1, uint32_t seed) {
		float x2 = std::ceil(x1);
		float y2 = std::ceil(y1);

		float xS1 = Noise2DF(std::floorf(x1), std::floorf(y1),seed);
		float xS2 = Noise2DF(x2, std::floorf(y1), seed);
		float xDif = x2 - x1;
		float xr = std::lerp(xS2, xS1, xDif);

		float yS1 = Noise2DF(std::floorf(x1), y2, seed);
		float yS2 = Noise2DF(x2, y2, seed);
		float yDif = y2 - y1;
		float yr = std::lerp(yS2, yS1, xDif);

		float r = std::lerp(yr, xr, yDif);
		r = (r + 1) / 2;
		r = (easeInOutCubic(r) * 2) - 2;

		//r = (easeInOutCirc(r) * 2) - 2;
		return r;
	}

	float SmoothOctaveNoise2D(float x1, float y1, uint32_t seed, uint16_t octaves, float lacunarity, float persistence) {
		float total = 0.0f;
		float detail = 1.0f;
		float amplitude = 1.0f;
		float offsetX = 0.0f;
		float offsetY = 0.0f;

		for(int i = 0; i < octaves; i++) {
			total += SmoothNoise2DF((x1+offsetX) * detail, (y1+offsetY) * detail, seed) * amplitude;
			detail *= lacunarity;
			amplitude *= persistence;
			offsetX += Noise1DF(detail * PRIME1,seed)*512;
			offsetY += Noise1DF(detail * PRIME2,seed)*512;
		}
		total /= octaves;
		return total;
	}

	float FastSmoothOctaveNoise2D(float x1, float y1, uint32_t seed, uint16_t octaves, float lacunarity, float persistence, bool useEase) {
		float total = 0.0f;
		float detail = 1.0f;
		float amplitude = 1.0f;
		float offsetX = 0.0f;
		float offsetY = 0.0f;

		auto l = [](float x, float y, float t) -> float { return x + t * (y - x); };
		auto xyr = [&](float _fx, float _cfy, float _x2, float _x1) -> float {
			float S1 = Noise2DF(_fx, _cfy, seed);
			float S2 = Noise2DF(_x2, _cfy, seed);
			return l(S2, S1, _x2 - _x1);
		};

		for(int i = 0; i < octaves; i++) {
			float _x = (x1 + offsetX) * detail;
			float _y = (y1 + offsetY) * detail;
			float _cy = std::ceilf(_y);
			float _cx = std::ceilf(_x);
			float _fx = std::floorf(_x);
			float _fy = std::floorf(_y);
			float t = l(xyr(_fx, _cy, _cx,_x),
						xyr(_fx, _fy, _cx,_x),
						_cy - _y);
			total += (useEase ? FastEaseInOutCubic(t) : t) * amplitude;
			detail *= lacunarity;
			amplitude *= persistence;
			offsetX += Noise1DF(detail * PRIME1, seed) * 512;
			offsetY += Noise1DF(detail * PRIME2, seed) * 512;
		}
		total /= octaves;
		return total;
	}
	
	uint32_t Noise3D(int32_t x, int32_t y, int32_t z, uint32_t seed) {
		return Noise1D(x + (PRIME1 * y) + (PRIME2 * z), seed);
	}

	float Noise3DF(int32_t x, int32_t y, int32_t z, uint32_t seed) {
		return NormalizeNoiseValue(Noise3D(x, y, z, seed));
	}
	
	uint32_t Noise4D(int32_t x, int32_t y, int32_t z, int32_t w, uint32_t seed) {
		return Noise1D(x + (PRIME1 * y) + (PRIME2 * z) + (PRIME3 * w), seed);
	}

	float Noise4DF(int32_t x, int32_t y, int32_t z, int32_t w, uint32_t seed) {
		return NormalizeNoiseValue(Noise4D(x, y, z, w, seed));
	}

	float CellularNoise2D(float px, float py, float scale, uint32_t seed) {
		glm::vec2 point = glm::vec2(px, py) * scale;
		glm::vec2 ipoint = glm::floor(point);
		glm::vec2 fpoint = glm::fract(point);
		float mdist = 1.0f;
		for(int y = -1; y <= 1; y++) {
			for(int x = -1; x <= 1; x++) {
				glm::vec2 neighbor = glm::vec2(x, y);
				glm::vec2 rpoint = RandCell2DVector(ipoint + neighbor, seed);
				glm::vec2 diff = neighbor + rpoint - fpoint;
				float dist = glm::length(diff);
				mdist = std::min(mdist, dist);
			}
		}
		return mdist;
	}

	glm::vec2 RandCell2DVector(int32_t x, int32_t y, uint32_t seed) {
		return glm::fract(glm::vec2(
			sin(glm::dot(glm::vec2(x, y), glm::vec2(127.1f, 311.7f))),
			sin(glm::dot(glm::vec2(x, y), glm::vec2(269.5f, 183.3f)))
			) * 43758.5453f);
	}

	glm::vec2 RandCell2DVector(glm::vec2 point, uint32_t seed) {
		return RandCell2DVector(point.x, point.y, seed);
	}

	float NormalizeNoiseValue(uint32_t val) {
		constexpr float OOM = 1 / (float)INT32_MAX;
		return (val * OOM) * 2 - 1;
	}

	float easeInOutCubic(float x) {
		return x < 0.5f ?
			4 * std::powf(x,3) :
			1 - std::powf(-2 * x + 2, 3) / 2;
	}
	
	float FastEaseInOutCubic(float x) {
		return x < 0.5f ?
			4 * x * x * x :
			1 - ((-2 * x + 2) * (-2 * x + 2) * (-2 * x + 2)) * .5f;
	}

	float easeInOutCirc(float x) {
		return x < 0.5f ?
			(1 - std::sqrtf(1 - std::pow(2 * x, 2))) / 2 :
			(std::sqrtf(1 - pow(-2 * x + 2, 2)) + 1) / 2;
	}

	float Sigmoid(float x) {
		return 1 / (1 + exp(-x));
	}

}	