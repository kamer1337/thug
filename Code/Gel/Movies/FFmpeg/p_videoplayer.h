/*****************************************************************************
**																			**
**					   	  Neversoft Entertainment							**
**																		   	**
**				   Copyright (C) 1999 - All Rights Reserved				   	**
**																			**
******************************************************************************
**																			**
**	Project:		GEL (Game Engine Library)								**
**																			**
**	Module:			Movies - FFmpeg Backend									**
**																			**
**	File name:		p_videoplayer.h											**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	FFmpeg-based video playback system						**
**																			**
*****************************************************************************/

#ifndef __GEL_MOVIES_FFMPEG_P_VIDEOPLAYER_H
#define __GEL_MOVIES_FFMPEG_P_VIDEOPLAYER_H

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#ifndef __CORE_DEFINES_H
#include <core/defines.h>
#endif

#ifdef USE_FFMPEG_VIDEO
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#endif

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Flx
{
namespace FFmpeg
{

// Video player state
enum EVideoPlayerState
{
	VIDEO_STATE_STOPPED = 0,
	VIDEO_STATE_PLAYING,
	VIDEO_STATE_PAUSED,
	VIDEO_STATE_ERROR,
};

// Playback options
enum EVideoPlaybackFlags
{
	VIDEO_FLAG_LOOP			= (1 << 0),	// Loop video playback
	VIDEO_FLAG_MUTED		= (1 << 1),	// Mute audio
	VIDEO_FLAG_FULLSCREEN	= (1 << 2),	// Play in fullscreen
	VIDEO_FLAG_SKIPPABLE	= (1 << 3),	// Allow skipping with input
};

/*****************************************************************************
**							     Structures									**
*****************************************************************************/

struct VideoInfo
{
	uint32			width;				// Video width
	uint32			height;				// Video height
	float			frameRate;			// Frames per second
	float			duration;			// Total duration in seconds
	bool			hasAudio;			// Has audio track
	bool			hasVideo;			// Has video track
	
	VideoInfo()
	{
		width = 0;
		height = 0;
		frameRate = 0.0f;
		duration = 0.0f;
		hasAudio = false;
		hasVideo = false;
	}
};

struct VideoFrame
{
	uint8_t*		data;				// Frame pixel data (RGBA format)
	uint32			width;				// Frame width
	uint32			height;				// Frame height
	double			timestamp;			// Presentation timestamp
	
	VideoFrame()
	{
		data = NULL;
		width = 0;
		height = 0;
		timestamp = 0.0;
	}
};

/*****************************************************************************
**							   Public Prototypes							**
*****************************************************************************/

// Initialization
void	VideoPlayer_Init( void );
void	VideoPlayer_Deinit( void );

// Playback control
bool	VideoPlayer_Load( const char* filename );
bool	VideoPlayer_Play( uint32 flags = 0 );
void	VideoPlayer_Pause( void );
void	VideoPlayer_Resume( void );
void	VideoPlayer_Stop( void );
void	VideoPlayer_Seek( float timeSeconds );

// State queries
EVideoPlayerState	VideoPlayer_GetState( void );
bool				VideoPlayer_IsPlaying( void );
bool				VideoPlayer_IsPaused( void );
float				VideoPlayer_GetCurrentTime( void );
float				VideoPlayer_GetDuration( void );
const VideoInfo*	VideoPlayer_GetInfo( void );

// Frame access (for rendering)
const VideoFrame*	VideoPlayer_GetCurrentFrame( void );
bool				VideoPlayer_Update( float deltaTime );

// Volume control
void	VideoPlayer_SetVolume( float volume );
float	VideoPlayer_GetVolume( void );
void	VideoPlayer_SetMuted( bool muted );
bool	VideoPlayer_IsMuted( void );

// Rendering (for integration with game engine)
void	VideoPlayer_RenderToScreen( int x, int y, int width, int height );
void	VideoPlayer_RenderToTexture( void* textureData, int textureWidth, int textureHeight );

} // namespace FFmpeg
} // namespace Flx

#endif // __GEL_MOVIES_FFMPEG_P_VIDEOPLAYER_H
