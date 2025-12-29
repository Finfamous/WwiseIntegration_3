#pragma once 

#include <iostream>
//Memory Manager Includes
#include <Ak/SoundEngine/Common/AkMemoryMgr.h>
#include <Ak/SoundEngine/Common/AkMemoryMgrModule.h>
//Streaming Manager includes
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AkFilePackageLowLevelIODeferred.h>

#include <Ak/SoundEngine/Common/AkSoundEngine.h>

//Wise info 
#include "D:/WwiseProjects/ITest/GeneratedSoundBanks/Wwise_IDs.h"


CAkFilePackageLowLevelIODeferred g_lowLevelIO;

constexpr auto WWISE_BANKS_PATH = L"D:/WwiseProjects/ITest/GeneratedSoundBanks/";


bool initSoundEngine() {
	// Init Memory Manager 
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		std::cout << "Could not create the memory manager." << std::endl;
		return false;
	}

	// Init Streaming Manager
	AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);

    // Customize the Stream Manager settings here.

    if (!AK::StreamMgr::Create(stmSettings))
    {
        std::cout << "Could not create the Streaming Manager" << std::endl;
        return false;

    }


    // Create a streaming device.
    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);


    if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
    {
        std::cout << "Could not create the streaming device and Low-Level I/O system" << std::endl;
        return false;
    }

    
    g_lowLevelIO.SetBasePath(WWISE_BANKS_PATH);

	//Create Sound Engine
    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
    {
        std::cout << "Could not initialize the Sound Engine." << std::endl;;
        return false;
    }


    return true;
	
}

void shutdownSoundEngine() {
    
    AK::SoundEngine::Term();

    g_lowLevelIO.Term();

    if (AK::IAkStreamMgr::Get()) AK::IAkStreamMgr::Get()->Destroy();

    AK::MemoryMgr::Term();
}

void ProcessAudio()
{
    AK::SoundEngine::RenderAudio();
}

int main() {
	if (initSoundEngine() == true)
	{
		std::cout << "SUCCESS! initialised Wwise." << std::endl;
	}
    else
    {
        std::cout << "FAILED to initialise Wwise." << std::endl;
    }

  
    //Load init and custom soundbank
    AkBankID initBankID;
    AkBankID soundBankID;
    AKRESULT eResult = AK::SoundEngine::LoadBank(L"Init.bnk", initBankID);
    if (eResult == AK_Success)
    {
        eResult = AK::SoundEngine::LoadBank(L"Init.bnk", soundBankID);
        std::cout << "SUCCESS! Loaded banks" << std::endl;
    }
    else
    {
        std::cout << "FAILED to load banks" << std::endl;
    }

    //Game object reference for post event
    const AkGameObjectID GameObj = 100;
    AK::SoundEngine::RegisterGameObj(GameObj);

    //post event
    AK::SoundEngine::PostEvent("Play_Z_Stun", GameObj);

    //Apparently needed every frame for audio to work, not sure if it's in the right place.
    ProcessAudio();

   //Errors at runtime if this isn't here.
   shutdownSoundEngine();


	return 0;
}