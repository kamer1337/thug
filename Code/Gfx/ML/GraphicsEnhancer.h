// Machine Learning Graphics Enhancement
// Provides ML-based graphics improvements like upscaling, denoising, and anti-aliasing

#ifndef __GFX_ML_GRAPHICS_ENHANCER_H
#define __GFX_ML_GRAPHICS_ENHANCER_H

// Include THUG types if available, otherwise use standard types
#ifdef __THUG_BUILD__
#include <Core/Defines.h>
#else
#include <stdint.h>
typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int32_t sint32;
#endif

namespace Gfx
{
namespace ML
{

// Image data structure
struct ImageData
{
    int width;
    int height;
    int channels;
    float* data; // Normalized 0-1 range
};

// Enhancement types
enum EnhancementType
{
    ENHANCEMENT_UPSCALE_2X,      // 2x super-resolution
    ENHANCEMENT_UPSCALE_4X,      // 4x super-resolution
    ENHANCEMENT_DENOISE,         // Noise reduction
    ENHANCEMENT_SHARPEN,         // Edge sharpening
    ENHANCEMENT_ANTI_ALIAS,      // Post-process anti-aliasing
    ENHANCEMENT_COLOR_ENHANCE    // Color enhancement
};

// Model configuration
struct ModelConfig
{
    EnhancementType type;
    float strength;              // 0.0 to 1.0, controls enhancement intensity
    bool useGPU;                 // Use GPU acceleration if available
    int numThreads;              // Number of CPU threads to use
    
    ModelConfig()
        : type(ENHANCEMENT_UPSCALE_2X)
        , strength(1.0f)
        , useGPU(false)
        , numThreads(4)
    {}
};

// Graphics Enhancer Interface
class IGraphicsEnhancer
{
public:
    virtual ~IGraphicsEnhancer() {}
    
    // Initialize the enhancer with a specific model
    virtual bool Initialize(const ModelConfig& config) = 0;
    
    // Shutdown and cleanup
    virtual void Shutdown() = 0;
    
    // Apply enhancement to an image
    virtual bool Enhance(const ImageData& input, ImageData& output) = 0;
    
    // Check if the enhancer is ready
    virtual bool IsReady() const = 0;
    
    // Get model information
    virtual const char* GetModelName() const = 0;
    virtual const char* GetModelVersion() const = 0;
};

// Factory function to create enhancer
IGraphicsEnhancer* CreateEnhancer(EnhancementType type);

// Get the current active enhancer
IGraphicsEnhancer* GetEnhancer();

// Set the current active enhancer
void SetEnhancer(IGraphicsEnhancer* enhancer);

// Utility functions for image processing
namespace Utils
{
    // Allocate image data
    ImageData* AllocateImage(int width, int height, int channels);
    
    // Free image data
    void FreeImage(ImageData* image);
    
    // Convert RGBA8 to float image
    void ConvertToFloat(const uint8* rgba8, int width, int height, ImageData* output);
    
    // Convert float image to RGBA8
    void ConvertToRGBA8(const ImageData* input, uint8* rgba8);
    
    // Bilinear upscale (fallback when ML is not available)
    void BilinearUpscale(const ImageData* input, ImageData* output, int scale);
    
    // Simple sharpening filter
    void Sharpen(const ImageData* input, ImageData* output, float strength);
    
    // Gaussian blur
    void GaussianBlur(const ImageData* input, ImageData* output, float sigma);
}

} // namespace ML
} // namespace Gfx

#endif // __GFX_ML_GRAPHICS_ENHANCER_H
