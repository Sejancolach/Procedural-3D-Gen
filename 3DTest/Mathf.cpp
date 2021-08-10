#include "Mathf.hpp"

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
		return NormalizeNoiseValue(Noise2D(x, y, seed));
	}

	float SmoothNoise2DF(float x1, float y1, uint32_t seed) {
		float x2 = std::ceil(x1);
		float y2 = std::ceil(y1);
		float xS1 = Noise2DF(std::floorf(x1), std::floorf(y1),seed);
		float xS2 = Noise2DF(x2, std::floorf(y1), seed);
		float yS1 = Noise2DF(std::floorf(x1), y2, seed);
		float yS2 = Noise2DF(x2, y2, seed);
		float xDif = x2 - x1;
		float yDif = y2 - y1;
		float xr = std::lerp(xS2, xS1, xDif);
		float yr = std::lerp(yS2, yS1, xDif);
		float r = std::lerp(yr, xr, yDif);
		r = (r + 1) / 2;
		r = (easeInOutCirc(r) * 2) - 2;
		return r;
	}

	float SmoothOctaveNoise2D(float x1, float y1, uint32_t seed, uint16_t octaves, float lacunarity, float persistence) {
		float total = 0.0f;
		float detail = 1.0f;
		float amplitude = 1.0f;

		for(int i = 0; i < octaves; i++) {
			total += SmoothNoise2DF(x1 * detail, y1 * detail, seed) * amplitude;
			detail *= lacunarity;
			amplitude *= persistence;
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

	float NormalizeNoiseValue(uint32_t val) {
		constexpr uint32_t NORMALIZE = 0x30000000;
		static const float norm = *(float*)&NORMALIZE; 
		//multiplies given value by smallest float value to return a value between (inclusive)-1 and (exclusive)1
		return ((float)val * norm) * 2 - 1; 
	}

	float easeInOutCubic(float x) {
		return x < 0.5f ?
			4 * std::powf(x,3) :
			1 - std::powf(-2 * x + 2, 3) / 2;
	}

	float easeInOutCirc(float x) {
		return x < 0.5f ?
			(1 - std::sqrtf(1 - std::pow(2 * x, 2))) / 2 :
			(std::sqrtf(1 - pow(-2 * x + 2, 2)) + 1) / 2;
	}

}