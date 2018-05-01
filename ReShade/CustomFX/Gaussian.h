NAMESPACE_ENTER(CFX)

#include CFX_SETTINGS_DEF

#if USE_GAUSS

 /**
 * Copyright (C) 2012 Jorge Jimenez (jorge@iryoku.com). All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are 
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the copyright holders.
 */

/* This is my attempt to port the the GAUSSIAN shader by Boulotaur2024 to ReShade.
   Some settings from the original are missing and I have added some other settings to achieve certain looks.
   More info can be found at 
   http://reshade.me/forum/shader-presentation/27-gaussian-blur-bloom-unsharpmask */ 

#define CoefLuma_G            float3(0.2126, 0.7152, 0.0722)      // BT.709 & sRBG luma coefficient (Monitors and HD Television)
#define sharp_strength_luma_G (CoefLuma_G * SharpStrength + 0.2)
#define sharp_clampG        0.035

texture GBlurTex2Dping{ Width = BUFFER_WIDTH; Height = BUFFER_HEIGHT; Format = RGBA8; };
texture GBlurTex2Dpong{ Width = BUFFER_WIDTH; Height = BUFFER_HEIGHT; Format = RGBA8; };

sampler2D GBlurSamplerPing { Texture = GBlurTex2Dping; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Clamp; SRGBTexture = FALSE;};
sampler2D GBlurSamplerPong { Texture = GBlurTex2Dpong; MinFilter = Linear; MagFilter = Linear; MipFilter = Linear; AddressU = Clamp; SRGBTexture = FALSE;};

float4 HGaussianBlurPS(in float4 pos : SV_Position, in float2 texcoord : TEXCOORD) : COLOR
{
  
	float sampleOffsets[5] = { 0.0, 1.4347826, 3.3478260, 5.2608695, 7.1739130 };
	float sampleWeights[5] = { 0.16818994, 0.27276957, 0.11690125, 0.024067905, 0.0021112196 };

	float4 color = tex2D(RFX_backbufferColor, texcoord) * sampleWeights[0];
	[loop]
	for(int i = 1; i < 5; ++i) {
		color += tex2D(RFX_backbufferColor, texcoord + float2(sampleOffsets[i] * RFX_pixelSize.x, 0.0)) * sampleWeights[i];
		color += tex2D(RFX_backbufferColor, texcoord - float2(sampleOffsets[i] * RFX_pixelSize.x, 0.0)) * sampleWeights[i]; 
	}
	
	return color;
}

float4 VGaussianBlurPS(in float4 pos : SV_Position, in float2 texcoord : TEXCOORD) : COLOR
{
  
	float sampleOffsets[5] = { 0.0f, 1.4347826f, 3.3478260f, 5.2608695f, 7.1739130f };
	float sampleWeights[5] = { 0.16818994, 0.27276957, 0.11690125, 0.024067905, 0.0021112196 };

	float4 color = tex2D(GBlurSamplerPing, texcoord) * sampleWeights[0];
	[loop]
	for(int j = 1; j < 5; ++j) {
		color += tex2D(GBlurSamplerPing, texcoord + float2(0.0f, sampleOffsets[j] * RFX_pixelSize.y)) * sampleWeights[j];
		color += tex2D(GBlurSamplerPing, texcoord - float2(0.0f, sampleOffsets[j] * RFX_pixelSize.y)) * sampleWeights[j];
	}
	return color;
}

float4 H2GaussianBlurPS(in float4 pos : SV_Position, in float2 texcoord : TEXCOORD) : COLOR
{
  
	float sampleOffsets[5] = { 0.0, 1.4347826, 3.3478260, 5.2608695, 7.1739130 };
	float sampleWeights[5] = { 0.16818994, 0.27276957, 0.11690125, 0.024067905, 0.0021112196 };
	
	float4 color = tex2D(GBlurSamplerPong, texcoord) * sampleWeights[0];
	[loop]
	for(int i = 1; i < 5; ++i) {
		color += tex2D(GBlurSamplerPong, texcoord + float2(sampleOffsets[i] * RFX_pixelSize.x, 0.0)) * sampleWeights[i];
		color += tex2D(GBlurSamplerPong, texcoord - float2(sampleOffsets[i] * RFX_pixelSize.x, 0.0)) * sampleWeights[i]; 
	}
	return color;
}

float4 V2GaussianBlurPS(in float4 pos : SV_Position, in float2 texcoord : TEXCOORD) : COLOR
{
  
	float sampleOffsets[5] = { 0.0, 1.4347826, 3.3478260, 5.2608695, 7.1739130 };
	float sampleWeights[5] = { 0.16818994, 0.27276957, 0.11690125, 0.024067905, 0.0021112196 };
	
	float4 color = tex2D(GBlurSamplerPing, texcoord) * sampleWeights[0];
	[loop]
	for(int j = 1; j < 5; ++j) {
		color += tex2D(GBlurSamplerPing, texcoord + float2(0.0, sampleOffsets[j] * RFX_pixelSize.y)) * sampleWeights[j];
		color += tex2D(GBlurSamplerPing, texcoord - float2(0.0, sampleOffsets[j] * RFX_pixelSize.y)) * sampleWeights[j];
	}
	float4 orig = tex2D(RFX_backbufferColor, texcoord); //Original Image
	#if (USE_Sharpening == 1) // Sharpening
	float3 sharp;
		sharp = orig.rgb - color.rgb;
		float sharp_luma1 = dot(sharp, sharp_strength_luma_G);
		sharp_luma1 = clamp(sharp_luma1, -sharp_clampG, sharp_clampG);
		orig = orig + sharp_luma1;
	#endif

	#if USE_Blur == 1
		orig = lerp(orig, color, BlurStrength);
	#endif
	return saturate(orig);
}

float3 FilmicTonemap(float3 x) {
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    return ((x*(A*x+C*B)+D*E) / (x*(A*x+B)+D*F))- E / F;
}

float3 DoToneMap(float3 color) {
	/*
    #if TONEMAP_OPERATOR == 1 //TONEMAP_LINEAR
    return exposure * color;
    #elif TONEMAP_OPERATOR == 2 //TONEMAP_EXPONENTIAL
    color = 1.0 - exp2(-exposure * color);
    return color;
    #elif TONEMAP_OPERATOR == 3 //TONEMAP_EXPONENTIAL_HSV
    color = rgb2hsv(color);
    color.b = 1.0 - exp2(-exposure * color.b);
    color = hsv2rgb(color);
    return color;
    #elif TONEMAP_OPERATOR == 5 //TONEMAP_REINHARD
    color = xyz2Yxy(rgb2xyz(color));
    float L = color.r;
    L *= exposure;
    float LL = 1 + L / (burnout * burnout);
    float L_d = L * LL / (1 + L);
    color.r = L_d;
    color = xyz2rgb(Yxy2xyz(color));
    return color;
    #else // TONEMAP_FILMIC
	*/
    color = 2.0f * FilmicTonemap(2.00 * color);
    float3 whiteScale = 1.0f / FilmicTonemap(11.2);
    color *= whiteScale;
    return color;
}

float4 GlareDetectionPS(in float4 pos : SV_Position, in float2 texcoord : TEXCOORD) : COLOR
{

    float2 offsets[5] = { 
        float2( 0.0,  0.0), 
        float2(-1.0,  0.0), 
        float2( 1.0,  0.0), 
        float2( 0.0, -1.0),
        float2( 0.0,  1.0),
    };
	float4 color;
    //float4 color = 1e100;
    for (int i = 0; i < 5; i++) 
        color = tex2D(RFX_backbufferColor,  texcoord + offsets[i] * RFX_pixelSize), color;
		#if (USE_Bloom == 1)	
			color.rgb *= GaussExposure;
		#endif

	#if (USE_Bloom == 1)
		color = float4(max(color.rgb - GaussThreshold / (1.0 - GaussThreshold), 0.0), color.a);
	#endif
	return color;
}

float4 FinalBloomPS(in float4 pos : SV_Position, in float2 texcoord : TEXCOORD) : COLOR
{

	float sampleOffsets[5] = { 0.0, 1.4347826, 3.3478260, 5.2608695, 7.1739130 };
	float sampleWeights[5] = { 0.16818994, 0.27276957, 0.11690125, 0.024067905, 0.0021112196 };
	
	float4 color = tex2D(GBlurSamplerPing, texcoord) * sampleWeights[0];
	[loop]
	for(int j = 1; j < 5; ++j) {
		color += tex2D(GBlurSamplerPing, texcoord + float2(0.0, sampleOffsets[j] * RFX_pixelSize.y)) * sampleWeights[j];
		color += tex2D(GBlurSamplerPing, texcoord - float2(0.0, sampleOffsets[j] * RFX_pixelSize.y)) * sampleWeights[j];
	}
	float4 orig = tex2D(RFX_backbufferColor, texcoord); //Original Image
			color.rgb = DoToneMap(color.rgb) * float3(BloomRed, BloomGreen, BloomBlue);
			#if (GaussBloomWarmth == 0)
				orig = lerp(orig, color, BloomStrength);                                        // Neutral
			#elif (GaussBloomWarmth == 1)
				orig = lerp(orig, max(orig + (color *1.6) - 1.0, 0.0), BloomStrength);          // Warm and cheap
			#else
				orig = lerp(orig, (1.0 - ((1.0 - orig) * (1.0 - color *1.0))), BloomStrength);  // Foggy bloom
			#endif
			/*	// Bloom
		#if (GaussBloomWarmth == 0)
			orig = lerp(orig, color *4, BloomStrength);                                     // Neutral
		#elif (GaussBloomWarmth == 1)
			orig = lerp(orig, max(orig *1.8 + (color *5) - 1.0, 0.0), BloomStrength);       // Warm and cheap
		#else
			orig = lerp(orig, (1.0 - ((1.0 - orig) * (1.0 - color *1.0))), BloomStrength);  // Foggy bloom
		#endif */
		
	return saturate(orig);
}

technique Gaussian_Tech <bool enabled = RFX_Start_Enabled; int toggle = Gaussian_ToggleKey; >
{
#if USE_Sharpening == 1 || USE_Blur == 1

	pass H1
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = HGaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}

#if GaussQuality == 1 || GaussQuality == 2 || GaussQuality == 3 || GaussQuality == 4 || GaussQuality == 5 ||  GaussQuality == 6 || GaussQuality == 7 || GaussQuality == 8 || GaussQuality == 9 || GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12
	pass V1
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H2
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 2 || GaussQuality == 3 || GaussQuality == 4 || GaussQuality == 5 ||  GaussQuality == 6 || GaussQuality == 7 || GaussQuality == 8 || GaussQuality == 9 || GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12
	pass V2
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H3
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 3 || GaussQuality == 4 || GaussQuality == 5 ||  GaussQuality == 6 || GaussQuality == 7 || GaussQuality == 8 || GaussQuality == 9 || GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12	
	pass V3
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
		
	pass H4
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 4 || GaussQuality == 5 ||  GaussQuality == 6 || GaussQuality == 7 || GaussQuality == 8 || GaussQuality == 9 || GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12	
	pass V4
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}	
	
	pass H5
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 5 ||  GaussQuality == 6 || GaussQuality == 7 || GaussQuality == 8 || GaussQuality == 9 || GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12	
	pass V5
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H6
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 6 || GaussQuality == 7 || GaussQuality == 8 || GaussQuality == 9 || GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12	
	pass V6
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H7
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 7 || GaussQuality == 8 || GaussQuality == 9 || GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12	
	pass V7
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H8
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 8 || GaussQuality == 9 || GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12	
	pass V8
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H9
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 9 || GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12
	pass V9
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H10
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 10 || GaussQuality == 11 || GaussQuality == 12	
	pass V10
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H11
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 11 || GaussQuality == 12	
	pass V11
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H12
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussQuality == 12
	pass V12
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H13
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif
	pass VFinal
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = V2GaussianBlurPS;
	}
#endif
#if USE_Bloom == 1

	pass GD
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = GlareDetectionPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H1Bloom
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
		
#if GaussBloomQuality == 1 || GaussBloomQuality == 2 || GaussBloomQuality == 3 || GaussBloomQuality == 4 || GaussBloomQuality == 5 ||  GaussBloomQuality == 6 || GaussBloomQuality == 7 || GaussBloomQuality == 8 || GaussBloomQuality == 9 || GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12
	pass V1
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H2
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 2 || GaussBloomQuality == 3 || GaussBloomQuality == 4 || GaussBloomQuality == 5 ||  GaussBloomQuality == 6 || GaussBloomQuality == 7 || GaussBloomQuality == 8 || GaussBloomQuality == 9 || GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12
	pass V2
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H3
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 3 || GaussBloomQuality == 4 || GaussBloomQuality == 5 ||  GaussBloomQuality == 6 || GaussBloomQuality == 7 || GaussBloomQuality == 8 || GaussBloomQuality == 9 || GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12	
	pass V3
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
		
	pass H4
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 4 || GaussBloomQuality == 5 ||  GaussBloomQuality == 6 || GaussBloomQuality == 7 || GaussBloomQuality == 8 || GaussBloomQuality == 9 || GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12	
	pass V4
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}	
	
	pass H5
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 5 ||  GaussBloomQuality == 6 || GaussBloomQuality == 7 || GaussBloomQuality == 8 || GaussBloomQuality == 9 || GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12	
	pass V5
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H6
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 6 || GaussBloomQuality == 7 || GaussBloomQuality == 8 || GaussBloomQuality == 9 || GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12	
	pass V6
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H7
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 7 || GaussBloomQuality == 8 || GaussBloomQuality == 9 || GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12	
	pass V7
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H8
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 8 || GaussBloomQuality == 9 || GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12	
	pass V8
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H9
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 9 || GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12
	pass V9
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H10
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 10 || GaussBloomQuality == 11 || GaussBloomQuality == 12	
	pass V10
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H11
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 11 || GaussBloomQuality == 12	
	pass V11
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H12
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif	
#if GaussBloomQuality == 12
	pass V12
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = VGaussianBlurPS;
		RenderTarget = GBlurTex2Dpong;
	}
	
	pass H13
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = H2GaussianBlurPS;
		RenderTarget = GBlurTex2Dping;
	}
#endif
	
	pass VFinal
	{
		VertexShader = RFX_VS_PostProcess;
		PixelShader = FinalBloomPS;
		//RenderTarget = GBlurTex2Dpong;
		//RenderTarget = frameTex2D;
	}
#endif
}

#endif

#include CFX_SETTINGS_UNDEF

NAMESPACE_LEAVE()
