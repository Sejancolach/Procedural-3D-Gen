#pragma once
#include "GameObject.hpp"

class GameObject;

class Behaviour {

public:
    bool enabled;
    GameObject *gameobject = nullptr;

    /// <summary>
    /// Gets called whenever the Object status changes to Active
    /// </summary>
    /// <param name=""></param>
    virtual void Awake(void) { };

    /// <summary>
    /// Gets called when the Behaviour is loaded
    /// </summary>
    /// <param name=""></param>
    virtual void Start(void) { };

    /// <summary>
    /// Gets called every Frame before Rendering
    /// </summary>
    /// <param name=""></param>
    virtual void Update(void) { };

    /// <summary>
    /// Gets called at the end of every Frame after Rendering
    /// </summary>
    /// <param name=""></param>
    virtual void LateUpdate(void) { };

    /// <summary>
    /// Gets called a fixed time a Second 
    /// see Time::fixedTimeScale (default 1 / 30)
    /// </summary>
    /// <param name=""></param>
    virtual void FixedUpdate(void) { };

};

