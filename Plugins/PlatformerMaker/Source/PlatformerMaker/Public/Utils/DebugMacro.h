#pragma once

#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

/////////// DRAW //////////////
#define DRAW_SPHERE_AT(Location, Duration) if(GetWorld())\
{\
	DrawDebugSphere(GetWorld(),Location,20.f,12,FColor::Red, false, Duration, 0U, 1.f);\
}

#define DRAW_ARROW(Start, End, Color) if(GetWorld())\
{\
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(),Start,End, 2.f,Color, 5.f, 4.f);\
}

#define DRAW_ARROW_TIME(Start, End, Color, Time) if(GetWorld())\
{\
	UKismetSystemLibrary::DrawDebugArrow(GetWorld(),Start,End, 2.f,Color, Time, 4.f);\
}

//////////// LOG ///////////////////
#define DEBUG_ERROR(Format,...) UE_LOG(LogTemp,Error,Format,__VA_ARGS__)
#define DEBUG_WARNING(Format,...) UE_LOG(LogTemp,Warning,Format,__VA_ARGS__)
#define DEBUG_LOG(Format,...) UE_LOG(LogTemp,Log,Format,__VA_ARGS__)

/****************************** LOG SCREEN *******************************/
#define DEBUG_LOG_SCREEN(Key, DisplayTime, Color, Format, ...) if(GEngine){\
	GEngine->AddOnScreenDebugMessage(Key, DisplayTime, Color, FString::Printf(Format, __VA_ARGS__));\
}

/////////// Class ////////////
/*
*Current Class where this is called
*/
#define CURRENT_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))))

/*
*Current Function Name where this is called
*/
#define CURRENT_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2))

/*
*Current Line Number in the code where this is called
* Could be weird as this state especially on 'if/else' check, if you use it on else it will outlog the line on else not the if condition
*/
#define CURRENT_LINE  (TEXT("Line") + FString::FromInt(__LINE__))