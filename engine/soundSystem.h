#pragma once

#include <string>  // For std::string
#include <vector>  // For std::vector
#include <map>     // For std::map
#include <AL/al.h> // For OpenAL
#include <AL/alc.h>
#include "sound.h" // For Sound
#include "entityManager.h" // For EntityManager and entity_id


struct SoundManager
{
    static map<string, Sound*> sounds;

    static void AddSound(string path);
    static void RemoveSounds();
    static Sound* GetSound(string name);
};

struct TimedSound {
    std::string soundName;
    float timeRemaining;  // Time in seconds before the sound should play
    bool loop;
    float volume;
    float pitch;
};

struct FadingSound {
    ALuint source;
    float targetVolume;  // Volume to reach
    float fadeSpeed;     // Volume change per second
};

struct SoundSystem {
    EntityManager* entityManager;
    SoundManager* soundManager;
    entity_id listenerEntity;

    std::vector<TimedSound> timedSounds;
    std::vector<FadingSound> fadingSounds;

    // OpenAL context and device
    ALCdevice* device = nullptr;
    ALCcontext* context = nullptr;

    SoundSystem();
    ~SoundSystem();

    void initialise(SoundManager* soundManager, EntityManager* entityManager);
    void update(float deltaTime);

    void updateListener();
    void initializeListener();

    void clean();

    void playSound(const std::string& soundName, bool loop = false, float volume = 1.0f, float pitch = 1.0f);
    void stopSound(const std::string& soundName);
    void stopAll();

    void addTimedSound(const std::string& soundName, float delay, bool loop = false, float volume = 1.0f, float pitch = 1.0f);
    void fadeSound(const std::string& soundName, float targetVolume, float duration);

    void GenAndBindOpenALOptions(Sound* sound, AudioEmitterComponent& comp);
};

