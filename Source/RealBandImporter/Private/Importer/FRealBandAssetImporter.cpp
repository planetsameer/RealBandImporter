
#include "FRealBandAssetImporter.h"
#include "GLTFAsset.h"
#include "GLTFReader.h"


bool FRealBandAssetGLTFImporter::OpenFile(const FString& InFileName)
{
	GLTFReader->ReadFile(InFileName, false, true, *GLTFAsset);
	const GLTF::FLogMessage* Found = GLTFReader->GetLogMessages().FindByPredicate(
		[](const GLTF::FLogMessage& Message) { return Message.Get<0>() == GLTF::EMessageSeverity::Error; });
	if (Found)
	{
		return false;
	}
	check(GLTFAsset->ValidationCheck() == GLTF::FAsset::Valid);

	GLTFAsset->GenerateNames(FPaths::GetBaseFilename(InFileName));

	// check extensions supported
	static const TArray<GLTF::EExtension> SupportedExtensions = { GLTF::EExtension::KHR_MaterialsPbrSpecularGlossiness,
																  GLTF::EExtension::KHR_MaterialsUnlit,
																  GLTF::EExtension::KHR_LightsPunctual,
																  GLTF::EExtension::KHR_MaterialsClearCoat,
																  GLTF::EExtension::KHR_MaterialsTransmission,
																  GLTF::EExtension::KHR_MaterialsSheen,
																  GLTF::EExtension::KHR_MaterialsVariants,
																  GLTF::EExtension::KHR_MaterialsSpecular,
																  GLTF::EExtension::KHR_MaterialsIOR };

	for (GLTF::EExtension Extension : GLTFAsset->ExtensionsUsed)
	{
		if (SupportedExtensions.Find(Extension) == INDEX_NONE)
		{
			LogMessages.Emplace(GLTF::EMessageSeverity::Warning, FString::Printf(TEXT("Extension is not supported: %s"), GLTF::ToString(Extension)));
		}
	}

	return true;
}