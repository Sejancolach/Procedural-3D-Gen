#pragma once
#include <cmath>

namespace Mathf {
	//constants
	static const uint32_t BIT_NOISE1 = { 0xB5297A4Dui32 };
	static const uint32_t BIT_NOISE2 = { 0x68E31DA4ui32 };
	static const uint32_t BIT_NOISE3 = { 0x1B56C4E9ui32 };
	static const int32_t PRIME1		 = { 1073676287 };
	static const int32_t PRIME2		 = { 6542989 };
	static const int32_t PRIME3		 = { 16769023 };

	
	//Functions
	uint32_t Noise1D(int32_t x, uint32_t seed);
	float Noise1DF(int32_t x, uint32_t seed);
	float SmoothNoise1DF(float x1, uint32_t seed);

	uint32_t Noise2D(int32_t x, int32_t y, uint32_t seed);
	float Noise2DF(int32_t x, int32_t y, uint32_t seed);
	float SmoothNoise2DF(float x1, float y1, uint32_t seed);
	float SmoothOctaveNoise2D(float x1, float y1, uint32_t seed, uint16_t octaves, float lacunarity, float persistence);
	
	uint32_t Noise3D(int32_t x, int32_t y, int32_t z, uint32_t seed);
	float Noise3DF(int32_t x, int32_t y, int32_t z, uint32_t seed);
	
	uint32_t Noise4D(int32_t x, int32_t y, int32_t z, int32_t w, uint32_t seed);
	float Noise4DF(int32_t x, int32_t y, int32_t z, int32_t w, uint32_t seed);


	float NormalizeNoiseValue(uint32_t val);
	float easeInOutCubic(float x);
	float easeInOutCirc(float x);
}