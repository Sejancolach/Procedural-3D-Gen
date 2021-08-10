#pragma once
static class Time { 

public:
	/// <summary>
	/// Current Time scale for Physic Updates
	/// </summary>
	static float TimeScale;
	/// <summary>
	/// The Resolution of Fixed Time Functions
	/// (1 / how often a second)
	/// </summary>
	static float FixedTimeScale;
	/// <summary>
	/// last Frame Time
	/// </summary>
	static float deltaTime;
};
float Time::TimeScale = 1.0f;
float Time::FixedTimeScale = 1.0f / 30;
float Time::deltaTime = 1.0f;
