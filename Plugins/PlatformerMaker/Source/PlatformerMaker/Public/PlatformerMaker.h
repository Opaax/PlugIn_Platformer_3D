// 2023 Copyright Enguerran COBERT, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

PLATFORMERMAKER_API DECLARE_LOG_CATEGORY_EXTERN(LogPlatformerPlugin, Log, All);

class FPlatformerMakerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
