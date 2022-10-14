#pragma once

#include "CoreMinimal.h"
#include "GLTFLogger.h"

namespace GLTF
{
	class FFileReader;
	struct FAsset;
	class FMeshFactory;
	class FMaterialFactory;
}

class FRealBandAssetGLTFImporter : FNoncopyable
{
	/** Returns the loaded asset from the last scene file. */
	const GLTF::FAsset& GetAsset() const { return *GLTFAsset; }
	/** Open and load a scene file. */
	bool OpenFile(const FString& InFileName);
	/** Returns any logged messages and clears them afterwards. */
	const TArray<GLTF::FLogMessage>& GetLogMessages() const;


private:
	TUniquePtr<GLTF::FFileReader>               GLTFReader;
	TUniquePtr<GLTF::FAsset>                    GLTFAsset;
	mutable TArray<GLTF::FLogMessage>           LogMessages;

};