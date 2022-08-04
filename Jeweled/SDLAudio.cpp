#include "SDLAudio.h"

#include <SDL.h>
#include <SDL_Mixer.h>
#include <assert.h>

////////////////////////////////////////////////////
// SDLAudio
//  A simple *generic* SDL Audio class.
//  This class is a singleton and is responsible
//  for handling all audio requests and playback of
//  all sound and music.
////////////////////////////////////////////////////

SDLAudio::SDLAudio()
{
}

SDLAudio::~SDLAudio()
{
	Mix_CloseAudio();
}

SDLAudio* SDLAudio::Get()
{
	static SDLAudio* pInstance = new SDLAudio();
	return pInstance;
}

void SDLAudio::Init()
{
	printf("SDLAudio::Init()\n");
	SDL_Init(SDL_INIT_AUDIO);

	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 512);

	AddMusicFile("Music1", "..\\Resources\\1980-Autonomus.s3m");
	//AddMusicFile("Music2", "..\\Resources\\1981-DataJack.s3m");
	AddMusicFile("Music3", "..\\Resources\\1984-Network.s3m");

	//AddSoundFile("BadMove", "..\\Resources\\bad2.ogg");
	//AddSoundFile("Click", "..\\Resources\\click2.ogg"); // unknown...
	AddSoundFile("Combo2", "..\\Resources\\combo22.ogg"); // various combos
	//AddSoundFile("Combo3", "..\\Resources\\combo32.ogg");
	//AddSoundFile("Combo4", "..\\Resources\\combo42.ogg");
	//AddSoundFile("Combo5", "..\\Resources\\combo52.ogg");
	//AddSoundFile("Combo6", "..\\Resources\\combo62.ogg");
	//AddSoundFile("Combo7", "..\\Resources\\combo72.ogg");
	//AddSoundFile("Excellent", "..\\Resources\\excellent1.ogg");
	//AddSoundFile("Explode", "..\\Resources\\explode2.ogg"); // gameover
	//AddSoundFile("GameOver", "..\\Resources\\gameover1.ogg"); // on newlevel
	AddSoundFile("GemOnGem", "..\\Resources\\gemongem2.ogg"); // 'clink'
	AddSoundFile("Go", "..\\Resources\\go1.ogg"); // on new level
	//AddSoundFile("Goodbye", "..\\Resources\\goodbye1.ogg"); // on exit
	AddSoundFile("GotSet", "..\\Resources\\gotset2.ogg"); // on match
	//AddSoundFile("GotSetBig", "..\\Resources\\gotsetbig2.ogg"); //on big match
	//AddSoundFile("LevelUp", "..\\Resources\\levelup1.ogg"); // on level up
	//AddSoundFile("MenuClick", "..\\Resources\\menuclick.ogg");
	AddSoundFile("Select", "..\\Resources\\Select.ogg");
	//AddSoundFile("Sidebar", "..\\Resources\\sidebarclick2.ogg");
	//AddSoundFile("Warning", "..\\Resources\\warning1.ogg"); // time nearly up
	//AddSoundFile("Welcome", "..\\Resources\\welcome1.ogg"); // unused?
	//AddSoundFile("Whirlpool", "..\\Resources\\whirlpool1.ogg"); // on level up
}

void SDLAudio::PlaySFX(const std::string& soundID, float volume)
{
	if (!IsSoundPlaying(soundID))
	{
		//printf("SDLAudio::PlaySound(%s)\n", soundID.c_str());

		// one-shot audio (FAF)
		if (m_soundEffectFiles[soundID] == NULL)
		{
			assert(!"Name not found... this sound file does not exist!");
		}

		Mix_VolumeChunk(m_soundEffectFiles[soundID], (int)(volume * 128 * m_globalVolume));
		Mix_PlayChannel(-1, m_soundEffectFiles[soundID], 0);

		//	update the 'last played' object
		m_lastUsed[soundID] = SDL_GetTicks();
	}
}

void SDLAudio::SetMusic(const std::string& soundID, float volume)
{
	printf("SDLAudio::SetMusic(%s)\n", soundID.c_str());

	// Set music first to avoid jumping
	// volume is out of 128 (?!) so accept a float and convert on-the-fly
	Mix_VolumeMusic((int)(127 * volume * m_globalVolume));

	// continuous, managed audio
	if (m_musicFiles[soundID] == nullptr)
	{
		assert(!"Refname not found... this sound file does not exist!");
	}

	//Mix_FadeInMusic( m_musicFiles[refName], -1, 1 );
	Mix_PlayMusic(m_musicFiles[soundID], -1);
}

void SDLAudio::AddSoundFile(const std::string& soundID, const std::string& fileLocation)
{
	printf("SDLAudio::AddSoundFile(%s)\n", soundID.c_str());

	Mix_Chunk* newFile = Mix_LoadWAV(fileLocation.c_str());
	printf("%s", Mix_GetError());
	assert(newFile != nullptr); // could not load this file!

	m_soundEffectFiles[soundID] = newFile;
}

void SDLAudio::AddMusicFile(const std::string& soundID, const std::string& fileLocation)
{
	printf("SDLAudio::AddMusicFile(%s)\n", soundID.c_str());

	Mix_Music* newFile = Mix_LoadMUS(fileLocation.c_str());
	assert(newFile != NULL); // could not load this file!

	m_musicFiles[soundID] = newFile;
}

void SDLAudio::StopAll()
{
	printf("SDLAudio::StopAll()\n");

	Mix_HaltMusic();
	Mix_HaltChannel(-1);
}

void SDLAudio::SetGlobalVolume(float newVol)
{
	printf("SDLAudio::SetGlobalVolume(%1.1f)\n", newVol);

	m_globalVolume = newVol;

	Mix_Volume(-1, (int)(128 * m_globalVolume));
	Mix_VolumeMusic((int)(128 * m_globalVolume));
}

bool SDLAudio::IsSoundPlaying(const std::string& refName)
{
	// Arbitrary length between sounds being able to be replayed
	const unsigned int REPLAY_TICK_LIMIT = 20;

	if (m_lastUsed[refName] != 0)
	{
		// the sound exists in our 'last used' db
		if (SDL_GetTicks() > m_lastUsed[refName] + REPLAY_TICK_LIMIT)
		{
			// sound has finished playing
			m_lastUsed[refName] = 0;
			return false;
		}
		else
		{
			// sound is still playing
			return true;
		}
	}

	// sound has never been played
	return false;
}