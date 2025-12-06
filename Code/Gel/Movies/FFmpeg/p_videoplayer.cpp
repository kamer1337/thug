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
**	File name:		p_videoplayer.cpp										**
**																			**
**	Created by:		PC Port													**
**																			**
**	Description:	FFmpeg-based video playback implementation				**
**																			**
*****************************************************************************/

/*****************************************************************************
**							  	  Includes									**
*****************************************************************************/

#include <gel/Movies/FFmpeg/p_videoplayer.h>
#include <core/defines.h>
#include <stdio.h>
#include <string.h>

/*****************************************************************************
**								   Defines									**
*****************************************************************************/

namespace Flx
{
namespace FFmpeg
{

#ifdef USE_FFMPEG_VIDEO

// Internal video player context
struct VideoPlayerContext
{
	AVFormatContext*	formatContext;
	AVCodecContext*		videoCodecContext;
	AVCodecContext*		audioCodecContext;
	AVFrame*			frame;
	AVFrame*			frameRGB;
	AVPacket*			packet;
	struct SwsContext*	swsContext;
	
	int					videoStreamIndex;
	int					audioStreamIndex;
	
	VideoInfo			info;
	VideoFrame			currentFrame;
	EVideoPlayerState	state;
	
	float				volume;
	bool				muted;
	uint32				flags;
	
	double				currentTime;
	double				frameTimer;
	double				frameLastPTS;
	
	VideoPlayerContext()
	{
		formatContext = NULL;
		videoCodecContext = NULL;
		audioCodecContext = NULL;
		frame = NULL;
		frameRGB = NULL;
		packet = NULL;
		swsContext = NULL;
		
		videoStreamIndex = -1;
		audioStreamIndex = -1;
		
		state = VIDEO_STATE_STOPPED;
		volume = 1.0f;
		muted = false;
		flags = 0;
		
		currentTime = 0.0;
		frameTimer = 0.0;
		frameLastPTS = 0.0;
	}
};

static VideoPlayerContext s_context;
static bool s_initialized = false;

// Helper function to get error string
static const char* GetFFmpegError(int errnum)
{
	static char errbuf[128];
	av_strerror(errnum, errbuf, sizeof(errbuf));
	return errbuf;
}

#endif // USE_FFMPEG_VIDEO

/*****************************************************************************
**							   Public Functions								**
*****************************************************************************/

// Initialize video player
void VideoPlayer_Init( void )
{
#ifdef USE_FFMPEG_VIDEO
	if (s_initialized)
		return;
	
	// Register all codecs and formats (deprecated in newer FFmpeg versions but safe)
	#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 9, 100)
	av_register_all();
	#endif
	
	printf("FFmpeg VideoPlayer: Initialized\n");
	printf("  AVCodec version: %s\n", av_version_info());
	
	s_initialized = true;
#else
	printf("FFmpeg VideoPlayer: Not compiled with FFmpeg support\n");
#endif
}

// Deinitialize video player
void VideoPlayer_Deinit( void )
{
#ifdef USE_FFMPEG_VIDEO
	if (!s_initialized)
		return;
	
	VideoPlayer_Stop();
	s_initialized = false;
	
	printf("FFmpeg VideoPlayer: Deinitialized\n");
#endif
}

// Load video file
bool VideoPlayer_Load( const char* filename )
{
#ifdef USE_FFMPEG_VIDEO
	if (!s_initialized)
		return false;
	
	// Clean up any existing video
	VideoPlayer_Stop();
	
	// Open video file
	int ret = avformat_open_input(&s_context.formatContext, filename, NULL, NULL);
	if (ret < 0)
	{
		printf("FFmpeg: Failed to open video file '%s': %s\n", filename, GetFFmpegError(ret));
		return false;
	}
	
	// Retrieve stream information
	ret = avformat_find_stream_info(s_context.formatContext, NULL);
	if (ret < 0)
	{
		printf("FFmpeg: Failed to find stream info: %s\n", GetFFmpegError(ret));
		avformat_close_input(&s_context.formatContext);
		return false;
	}
	
	// Find video stream
	s_context.videoStreamIndex = -1;
	for (unsigned int i = 0; i < s_context.formatContext->nb_streams; i++)
	{
		if (s_context.formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			s_context.videoStreamIndex = i;
			break;
		}
	}
	
	if (s_context.videoStreamIndex == -1)
	{
		printf("FFmpeg: No video stream found\n");
		avformat_close_input(&s_context.formatContext);
		return false;
	}
	
	// Find audio stream (optional)
	s_context.audioStreamIndex = -1;
	for (unsigned int i = 0; i < s_context.formatContext->nb_streams; i++)
	{
		if (s_context.formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			s_context.audioStreamIndex = i;
			break;
		}
	}
	
	// Get video codec parameters
	AVCodecParameters* codecParams = s_context.formatContext->streams[s_context.videoStreamIndex]->codecpar;
	
	// Find video decoder
	const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);
	if (!codec)
	{
		printf("FFmpeg: Codec not found\n");
		avformat_close_input(&s_context.formatContext);
		return false;
	}
	
	// Allocate codec context
	s_context.videoCodecContext = avcodec_alloc_context3(codec);
	if (!s_context.videoCodecContext)
	{
		printf("FFmpeg: Failed to allocate codec context\n");
		avformat_close_input(&s_context.formatContext);
		return false;
	}
	
	// Copy codec parameters to context
	ret = avcodec_parameters_to_context(s_context.videoCodecContext, codecParams);
	if (ret < 0)
	{
		printf("FFmpeg: Failed to copy codec parameters: %s\n", GetFFmpegError(ret));
		avcodec_free_context(&s_context.videoCodecContext);
		avformat_close_input(&s_context.formatContext);
		return false;
	}
	
	// Open codec
	ret = avcodec_open2(s_context.videoCodecContext, codec, NULL);
	if (ret < 0)
	{
		printf("FFmpeg: Failed to open codec: %s\n", GetFFmpegError(ret));
		avcodec_free_context(&s_context.videoCodecContext);
		avformat_close_input(&s_context.formatContext);
		return false;
	}
	
	// Allocate frames
	s_context.frame = av_frame_alloc();
	s_context.frameRGB = av_frame_alloc();
	
	if (!s_context.frame || !s_context.frameRGB)
	{
		printf("FFmpeg: Failed to allocate frames\n");
		if (s_context.frame) av_frame_free(&s_context.frame);
		if (s_context.frameRGB) av_frame_free(&s_context.frameRGB);
		avcodec_free_context(&s_context.videoCodecContext);
		avformat_close_input(&s_context.formatContext);
		return false;
	}
	
	// Allocate RGB frame buffer
	int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, 
											 s_context.videoCodecContext->width,
											 s_context.videoCodecContext->height, 1);
	s_context.currentFrame.data = (uint8_t*)av_malloc(numBytes);
	
	// Assign buffer to frame
	av_image_fill_arrays(s_context.frameRGB->data, s_context.frameRGB->linesize,
						 s_context.currentFrame.data, AV_PIX_FMT_RGBA,
						 s_context.videoCodecContext->width,
						 s_context.videoCodecContext->height, 1);
	
	// Initialize SWS context for color conversion
	s_context.swsContext = sws_getContext(
		s_context.videoCodecContext->width,
		s_context.videoCodecContext->height,
		s_context.videoCodecContext->pix_fmt,
		s_context.videoCodecContext->width,
		s_context.videoCodecContext->height,
		AV_PIX_FMT_RGBA,
		SWS_BILINEAR, NULL, NULL, NULL
	);
	
	// Allocate packet
	s_context.packet = av_packet_alloc();
	
	// Populate video info
	s_context.info.width = s_context.videoCodecContext->width;
	s_context.info.height = s_context.videoCodecContext->height;
	s_context.info.hasVideo = true;
	s_context.info.hasAudio = (s_context.audioStreamIndex >= 0);
	
	AVStream* videoStream = s_context.formatContext->streams[s_context.videoStreamIndex];
	s_context.info.frameRate = (float)av_q2d(videoStream->r_frame_rate);
	s_context.info.duration = (float)(s_context.formatContext->duration / (double)AV_TIME_BASE);
	
	// Set current frame dimensions
	s_context.currentFrame.width = s_context.info.width;
	s_context.currentFrame.height = s_context.info.height;
	
	printf("FFmpeg: Loaded video '%s'\n", filename);
	printf("  Resolution: %dx%d\n", s_context.info.width, s_context.info.height);
	printf("  Frame rate: %.2f fps\n", s_context.info.frameRate);
	printf("  Duration: %.2f seconds\n", s_context.info.duration);
	printf("  Has audio: %s\n", s_context.info.hasAudio ? "Yes" : "No");
	
	s_context.state = VIDEO_STATE_STOPPED;
	return true;
#else
	return false;
#endif
}

// Play video
bool VideoPlayer_Play( uint32 flags )
{
#ifdef USE_FFMPEG_VIDEO
	if (!s_initialized || !s_context.formatContext)
		return false;
	
	s_context.flags = flags;
	s_context.state = VIDEO_STATE_PLAYING;
	s_context.currentTime = 0.0;
	s_context.frameTimer = 0.0;
	s_context.frameLastPTS = 0.0;
	
	printf("FFmpeg: Playing video\n");
	return true;
#else
	return false;
#endif
}

// Pause video
void VideoPlayer_Pause( void )
{
#ifdef USE_FFMPEG_VIDEO
	if (s_context.state == VIDEO_STATE_PLAYING)
	{
		s_context.state = VIDEO_STATE_PAUSED;
		printf("FFmpeg: Video paused\n");
	}
#endif
}

// Resume video
void VideoPlayer_Resume( void )
{
#ifdef USE_FFMPEG_VIDEO
	if (s_context.state == VIDEO_STATE_PAUSED)
	{
		s_context.state = VIDEO_STATE_PLAYING;
		printf("FFmpeg: Video resumed\n");
	}
#endif
}

// Stop video
void VideoPlayer_Stop( void )
{
#ifdef USE_FFMPEG_VIDEO
	if (!s_initialized)
		return;
	
	s_context.state = VIDEO_STATE_STOPPED;
	
	// Free resources
	if (s_context.currentFrame.data)
	{
		av_free(s_context.currentFrame.data);
		s_context.currentFrame.data = NULL;
	}
	
	if (s_context.packet)
	{
		av_packet_free(&s_context.packet);
	}
	
	if (s_context.swsContext)
	{
		sws_freeContext(s_context.swsContext);
		s_context.swsContext = NULL;
	}
	
	if (s_context.frame)
	{
		av_frame_free(&s_context.frame);
	}
	
	if (s_context.frameRGB)
	{
		av_frame_free(&s_context.frameRGB);
	}
	
	if (s_context.videoCodecContext)
	{
		avcodec_free_context(&s_context.videoCodecContext);
	}
	
	if (s_context.audioCodecContext)
	{
		avcodec_free_context(&s_context.audioCodecContext);
	}
	
	if (s_context.formatContext)
	{
		avformat_close_input(&s_context.formatContext);
	}
	
	s_context.videoStreamIndex = -1;
	s_context.audioStreamIndex = -1;
	
	printf("FFmpeg: Video stopped\n");
#endif
}

// Seek to position
void VideoPlayer_Seek( float timeSeconds )
{
#ifdef USE_FFMPEG_VIDEO
	if (!s_initialized || !s_context.formatContext)
		return;
	
	int64_t timestamp = (int64_t)(timeSeconds * AV_TIME_BASE);
	int ret = av_seek_frame(s_context.formatContext, -1, timestamp, AVSEEK_FLAG_BACKWARD);
	
	if (ret >= 0)
	{
		s_context.currentTime = timeSeconds;
		printf("FFmpeg: Seeked to %.2f seconds\n", timeSeconds);
	}
	else
	{
		printf("FFmpeg: Seek failed: %s\n", GetFFmpegError(ret));
	}
#endif
}

// Get current state
EVideoPlayerState VideoPlayer_GetState( void )
{
#ifdef USE_FFMPEG_VIDEO
	return s_context.state;
#else
	return VIDEO_STATE_STOPPED;
#endif
}

// Check if playing
bool VideoPlayer_IsPlaying( void )
{
#ifdef USE_FFMPEG_VIDEO
	return (s_context.state == VIDEO_STATE_PLAYING);
#else
	return false;
#endif
}

// Check if paused
bool VideoPlayer_IsPaused( void )
{
#ifdef USE_FFMPEG_VIDEO
	return (s_context.state == VIDEO_STATE_PAUSED);
#else
	return false;
#endif
}

// Get current time
float VideoPlayer_GetCurrentTime( void )
{
#ifdef USE_FFMPEG_VIDEO
	return (float)s_context.currentTime;
#else
	return 0.0f;
#endif
}

// Get duration
float VideoPlayer_GetDuration( void )
{
#ifdef USE_FFMPEG_VIDEO
	return s_context.info.duration;
#else
	return 0.0f;
#endif
}

// Get video info
const VideoInfo* VideoPlayer_GetInfo( void )
{
#ifdef USE_FFMPEG_VIDEO
	return &s_context.info;
#else
	return NULL;
#endif
}

// Get current frame
const VideoFrame* VideoPlayer_GetCurrentFrame( void )
{
#ifdef USE_FFMPEG_VIDEO
	return &s_context.currentFrame;
#else
	return NULL;
#endif
}

// Update video (decode next frame)
bool VideoPlayer_Update( float deltaTime )
{
#ifdef USE_FFMPEG_VIDEO
	if (s_context.state != VIDEO_STATE_PLAYING || !s_context.formatContext)
		return false;
	
	s_context.currentTime += deltaTime;
	
	// Read frames
	int ret = av_read_frame(s_context.formatContext, s_context.packet);
	if (ret < 0)
	{
		// End of file or error
		if (s_context.flags & VIDEO_FLAG_LOOP)
		{
			// Loop video
			VideoPlayer_Seek(0.0f);
			return true;
		}
		else
		{
			// Stop playback
			s_context.state = VIDEO_STATE_STOPPED;
			return false;
		}
	}
	
	// Is this a video packet?
	if (s_context.packet->stream_index == s_context.videoStreamIndex)
	{
		// Send packet to decoder
		ret = avcodec_send_packet(s_context.videoCodecContext, s_context.packet);
		if (ret < 0)
		{
			printf("FFmpeg: Error sending packet to decoder: %s\n", GetFFmpegError(ret));
			av_packet_unref(s_context.packet);
			return false;
		}
		
		// Receive decoded frame
		ret = avcodec_receive_frame(s_context.videoCodecContext, s_context.frame);
		if (ret == 0)
		{
			// Convert frame to RGBA
			sws_scale(
				s_context.swsContext,
				(const uint8_t* const*)s_context.frame->data,
				s_context.frame->linesize,
				0,
				s_context.videoCodecContext->height,
				s_context.frameRGB->data,
				s_context.frameRGB->linesize
			);
			
			// Update frame timestamp
			s_context.currentFrame.timestamp = s_context.currentTime;
		}
	}
	
	av_packet_unref(s_context.packet);
	return true;
#else
	return false;
#endif
}

// Set volume
void VideoPlayer_SetVolume( float volume )
{
#ifdef USE_FFMPEG_VIDEO
	s_context.volume = volume;
	if (s_context.volume < 0.0f) s_context.volume = 0.0f;
	if (s_context.volume > 1.0f) s_context.volume = 1.0f;
#endif
}

// Get volume
float VideoPlayer_GetVolume( void )
{
#ifdef USE_FFMPEG_VIDEO
	return s_context.volume;
#else
	return 0.0f;
#endif
}

// Set muted
void VideoPlayer_SetMuted( bool muted )
{
#ifdef USE_FFMPEG_VIDEO
	s_context.muted = muted;
#endif
}

// Check if muted
bool VideoPlayer_IsMuted( void )
{
#ifdef USE_FFMPEG_VIDEO
	return s_context.muted;
#else
	return false;
#endif
}

// Render to screen (platform-specific)
void VideoPlayer_RenderToScreen( int x, int y, int width, int height )
{
#ifdef USE_FFMPEG_VIDEO
	// This would integrate with the game's rendering system
	// For now, it's a placeholder
	printf("FFmpeg: Render to screen at (%d,%d) size %dx%d\n", x, y, width, height);
#endif
}

// Render to texture
void VideoPlayer_RenderToTexture( void* textureData, int textureWidth, int textureHeight )
{
#ifdef USE_FFMPEG_VIDEO
	if (!s_context.currentFrame.data || !textureData)
		return;
	
	// Copy frame data to texture
	// This is a simple copy - proper implementation would handle scaling/filtering
	memcpy(textureData, s_context.currentFrame.data, 
		   s_context.currentFrame.width * s_context.currentFrame.height * 4);
#endif
}

} // namespace FFmpeg
} // namespace Flx
