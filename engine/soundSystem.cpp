#include "entity.h"
#include "soundSystem.h"
#include <iostream>
#include <cmath>

std::map<std::string, Sound*> SoundManager::sounds;

void SoundManager::AddSound(string name)
{
    if(!GetSound(name))
    {
        Sound *s = new Sound();

        string extension = name.substr(name.length()-3, 3);

        if(extension == "wav")
        {
            s->LoadFromWAV(name);

            alGenBuffers(1, &s->buffer);

            alBufferData(s->buffer, s->GetOALFormat(), s->GetData(),
                         s->GetSize(), (ALsizei)s->GetFrequency());
        }
        else
        {
            cout << "Invalid extension '" << extension << "'!" << endl;
        }

        sounds.insert(make_pair(name, s));
    }
}

void SoundManager::RemoveSounds()
{
    for(map<string, Sound*>::iterator i = sounds.begin(); i != sounds.end(); ++i)
    {
        delete i->second;
    }
}

Sound* SoundManager::GetSound(string name)
{
    map<string, Sound*>::iterator s = sounds.find(name);
    return (s != sounds.end() ? s->second : nullptr);
}


//Main Sound system

SoundSystem::SoundSystem() {}

SoundSystem::~SoundSystem() {
    clean();
}

void SoundSystem::initialise(SoundManager* sm, EntityManager* em) {
    soundManager = sm;
    entityManager = em;

    // Initialize OpenAL
    device = alcOpenDevice(nullptr);  // Use default device
    if (!device) {
        std::cerr << "Failed to open OpenAL device!" << std::endl;
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context || alcMakeContextCurrent(context) == ALC_FALSE) {
        std::cerr << "Failed to create OpenAL context!" << std::endl;
        if (context) alcDestroyContext(context);
        alcCloseDevice(device);
        return;
    }

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);

    std::cout << "SoundSystem initialized." << std::endl;
}

void checkOpenALError(const std::string& context) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "OpenAL Error in " << context << ": " << error << std::endl;
    }
}

void SoundSystem::update(float deltaTime) {
    updateListener();

    // Update timed sounds
    for (auto it = timedSounds.begin(); it != timedSounds.end();) {
        it->timeRemaining -= deltaTime;
        if (it->timeRemaining <= 0.0f) {
            playSound(it->soundName, it->loop, it->volume, it->pitch);
            it = timedSounds.erase(it);
        } else {
            ++it;
        }
    }

    // Update fading sounds
    for (auto it = fadingSounds.begin(); it != fadingSounds.end();) {
        ALfloat currentVolume;
        alGetSourcef(it->source, AL_GAIN, &currentVolume);

        float step = it->fadeSpeed * deltaTime;
        if (currentVolume < it->targetVolume) {
            currentVolume = std::min(it->targetVolume, currentVolume + step);
        } else {
            currentVolume = std::max(it->targetVolume, currentVolume - step);
        }
        alSourcef(it->source, AL_GAIN, currentVolume);

        if (std::abs(currentVolume - it->targetVolume) < 0.01f) {
            it = fadingSounds.erase(it);
        } else {
            ++it;
        }
    }

    // Iterate through entities with AudioEmitterComponent
    for (auto it = entityManager->audio_emitters.begin(); it != entityManager->audio_emitters.end(); ++it) {
        entity_id entityID = it->first;
        AudioEmitterComponent& emitter = it->second;

        // Retrieve sound from SoundManager
        Sound* sound = soundManager->GetSound(emitter.soundName);
        if (!sound) {
            std::cerr << "Sound not found: " << emitter.soundName << std::endl;
            continue;
        }

        // Attach buffer if not already attached
        ALint bufferAttached;
        alGetSourcei(emitter.source, AL_BUFFER, &bufferAttached);
        if (bufferAttached != static_cast<ALint>(sound->GetBuffer())) {
            alSourcei(emitter.source, AL_BUFFER, sound->GetBuffer());
        }

         if (entityManager->transforms.find(entityID) != entityManager->transforms.end()) {
            TransformComponent& transform = entityManager->transforms[entityID];
            ALfloat sourcePos[] = { transform.position.x, transform.position.y, transform.position.z };
            alSourcefv(emitter.source, AL_POSITION, sourcePos);

            std::cout << "Emitter Position: ("
                      << sourcePos[0] << ", " << sourcePos[1] << ", " << sourcePos[2]
                      << ")" << std::endl;
        }

        // Set OpenAL source properties
        alSourcef(emitter.source, AL_GAIN, emitter.volume);
        alSourcef(emitter.source, AL_PITCH, emitter.pitch);
        alSourcei(emitter.source, AL_LOOPING, emitter.loop ? AL_TRUE : AL_FALSE);


        // Manage playback state
        ALint state;
        alGetSourcei(emitter.source, AL_SOURCE_STATE, &state);

        if (emitter.isPlaying && state != AL_PLAYING) {
            alSourcePlay(emitter.source);
            checkOpenALError("alSourcePlay");
        } else if (!emitter.isPlaying && state == AL_PLAYING) {
            alSourcePause(emitter.source);
        }
    }
}

void SoundSystem::clean() {
    // Clean up OpenAL resources
    if (context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
    }
    if (device) {
        alcCloseDevice(device);
    }

    std::cout << "SoundSystem cleaned up." << std::endl;
}

void SoundSystem::playSound(const std::string& soundName, bool loop, float volume, float pitch) {
    // Retrieve sound from SoundManager
    Sound* sound = soundManager->GetSound(soundName);
    if (!sound) {
        std::cerr << "Sound not found: " << soundName << std::endl;
        return;
    }

    // Generate a temporary source
    ALuint source;
    alGenSources(1, &source);
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "Failed to generate OpenAL source for sound: " << soundName << std::endl;
        return;
    }

    // Attach buffer and set properties
    alSourcei(source, AL_BUFFER, sound->GetBuffer());
    alSourcef(source, AL_GAIN, volume);
    alSourcef(source, AL_PITCH, pitch);
    alSourcei(source, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);

    // Play the sound
    alSourcePlay(source);

    // Automatically delete the source after playback (non-looping sounds)
    if (!loop) {
        alSourceStop(source);
        alDeleteSources(1, &source);
    }
}

void SoundSystem::stopSound(const std::string& soundName) {
    for (auto it = entityManager->audio_emitters.begin(); it != entityManager->audio_emitters.end(); ++it) {
        AudioEmitterComponent& emitter = it->second;
        if (emitter.soundName == soundName) {
            alSourceStop(emitter.source);
        }
    }
}

void SoundSystem::stopAll() {
    for (auto it = entityManager->audio_emitters.begin(); it != entityManager->audio_emitters.end(); ++it) {
        AudioEmitterComponent& emitter = it->second;
        alSourceStop(emitter.source);
    }
}

void SoundSystem::updateListener() {
    if (entityManager->transforms.find(listenerEntity) != entityManager->transforms.end()) {
        TransformComponent& transform = entityManager->transforms[listenerEntity];

        // Set listener position
        ALfloat listenerPos[] = { transform.position.x, transform.position.y, transform.position.z };
        alListenerfv(AL_POSITION, listenerPos);

        // Set listener orientation (forward and up vectors)
        ALfloat listenerOri[] = {
            transform.forward.x, transform.forward.y, transform.forward.z, // Forward vector
            transform.up.x, transform.up.y, transform.up.z                // Up vector
        };
        alListenerfv(AL_ORIENTATION, listenerOri);

        std::cout << "Listener Position: ("
                  << listenerPos[0] << ", " << listenerPos[1] << ", " << listenerPos[2]
                  << ")" << std::endl;
    }
}

void SoundSystem::fadeSound(const std::string& soundName, float targetVolume, float duration) {
    Sound* sound = soundManager->GetSound(soundName);
    if (sound) {
        ALuint source;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, sound->GetBuffer());
        fadingSounds.push_back({ source, targetVolume, std::abs(targetVolume - 1.0f) / duration });
    }
}

void SoundSystem::addTimedSound(const std::string& soundName, float delay, bool loop, float volume, float pitch) {
    timedSounds.push_back({ soundName, delay, loop, volume, pitch });
}