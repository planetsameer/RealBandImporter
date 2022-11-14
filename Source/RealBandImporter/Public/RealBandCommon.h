// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <string>

using namespace std;

 #ifdef  REALBAND_EXPORTS 
    /*Enabled as "export" while compiling the dll project*/
    #define REALBAND __declspec(dllexport)  
 #else
    /*Enabled as "import" in the Client side for using already created dll file*/
    #define REALBAND __declspec(dllimport)  
 #endif

struct  UserPreference
{
    string FolderPath;
    string ConnectPath;
    string Project;
    uint16 ActiveTypeBitset;
    int Format;
    int  TexLevel;
    int MeshLevel;
    int TextureType;
};

enum SELECTOPTIONS
{
	FORMAT_GLM = 0x01,
    FORMAT_FBX,
    TWO_K,
    FOUR_K,
    EIGHT_K,
    HIGH,
    LOW,
    DIFFUSE,
    NORMAL
};


typedef struct UserPreference USRPREFERENCE;