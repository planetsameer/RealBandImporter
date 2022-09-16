// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "RealBandPlugin/RealBandPluginGameModeBase.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeRealBandPluginGameModeBase() {}
// Cross Module References
	REALBANDPLUGIN_API UClass* Z_Construct_UClass_ARealBandPluginGameModeBase_NoRegister();
	REALBANDPLUGIN_API UClass* Z_Construct_UClass_ARealBandPluginGameModeBase();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_RealBandPlugin();
// End Cross Module References
	void ARealBandPluginGameModeBase::StaticRegisterNativesARealBandPluginGameModeBase()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ARealBandPluginGameModeBase);
	UClass* Z_Construct_UClass_ARealBandPluginGameModeBase_NoRegister()
	{
		return ARealBandPluginGameModeBase::StaticClass();
	}
	struct Z_Construct_UClass_ARealBandPluginGameModeBase_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_ARealBandPluginGameModeBase_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_RealBandPlugin,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_ARealBandPluginGameModeBase_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * \n */" },
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "RealBandPluginGameModeBase.h" },
		{ "ModuleRelativePath", "RealBandPluginGameModeBase.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_ARealBandPluginGameModeBase_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ARealBandPluginGameModeBase>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_ARealBandPluginGameModeBase_Statics::ClassParams = {
		&ARealBandPluginGameModeBase::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x009002ACu,
		METADATA_PARAMS(Z_Construct_UClass_ARealBandPluginGameModeBase_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_ARealBandPluginGameModeBase_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_ARealBandPluginGameModeBase()
	{
		if (!Z_Registration_Info_UClass_ARealBandPluginGameModeBase.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ARealBandPluginGameModeBase.OuterSingleton, Z_Construct_UClass_ARealBandPluginGameModeBase_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_ARealBandPluginGameModeBase.OuterSingleton;
	}
	template<> REALBANDPLUGIN_API UClass* StaticClass<ARealBandPluginGameModeBase>()
	{
		return ARealBandPluginGameModeBase::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(ARealBandPluginGameModeBase);
	struct Z_CompiledInDeferFile_FID_RealBandPlugin_Source_RealBandPlugin_RealBandPluginGameModeBase_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_RealBandPlugin_Source_RealBandPlugin_RealBandPluginGameModeBase_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_ARealBandPluginGameModeBase, ARealBandPluginGameModeBase::StaticClass, TEXT("ARealBandPluginGameModeBase"), &Z_Registration_Info_UClass_ARealBandPluginGameModeBase, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ARealBandPluginGameModeBase), 1716775791U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_RealBandPlugin_Source_RealBandPlugin_RealBandPluginGameModeBase_h_4167240653(TEXT("/Script/RealBandPlugin"),
		Z_CompiledInDeferFile_FID_RealBandPlugin_Source_RealBandPlugin_RealBandPluginGameModeBase_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_RealBandPlugin_Source_RealBandPlugin_RealBandPluginGameModeBase_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
