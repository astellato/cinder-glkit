//
//  RenderShaders.h
//  IOSShaders
//
//  Created by Anthony Stellato on 6/25/13.
//
//

#ifndef IOSShaders_RenderShaders_h
#define IOSShaders_RenderShaders_h

#pragma once

#include <string>

#define STRINGIFY(A) #A

//const std::string shaderVertPassthru = STRINGIFY(
//                                                 //#version 120
//                                                 varying vec3 normal;
//                                                 varying vec3 position;
//                                                 varying vec2 uv;
//                                                 
//                                                 void main()
//                                                 {
//                                                     normal = normalize( gl_NormalMatrix * gl_Normal );
//                                                     vec4 v = vec4( gl_Vertex );
//                                                     
//                                                     position = v.xyz;
//                                                     gl_FrontColor = gl_Color;
//                                                     gl_Position = ftransform();
//                                                     gl_TexCoord[0] = gl_MultiTexCoord0;
//                                                     uv = gl_TexCoord[0].st;
//                                                 }
//                                                 );

const std::string shaderVertPassThru = STRINGIFY(
                                                 uniform mat4 u_mvp_matrix;
                                                 
                                                 attribute vec3 position;
                                                 
                                                 void main(){
                                                     gl_Position = u_mvp_matrix * vec4(position, 1.);
                                                 }

);
const std::string shaderFragDebug = STRINGIFY(
                                              void(main){
                                                  gl_FragColor = vec4(1., 0., 0., 1.);
                                              }
);

const std::string shaderFragLoadingBox = STRINGIFY(
                                                   precision highp float;

                                                   uniform float    iGlobalTime;
                                                   uniform vec3     iResolution;
                                                   
                                                   void main(void)
{
	vec2 glUV = gl_FragCoord.xy / iResolution.xy;
	vec4 cvSplashData = vec4(iResolution.x, iResolution.y, iGlobalTime, 0.0);
	vec2 InUV = glUV * 2.0 - 1.0;
	
	//////////////////////////////////////////////////////////////
	// End of ShaderToy Input Compat
	//////////////////////////////////////////////////////////////
	
	// Constants
	float TimeElapsed		= cvSplashData.z;
	float Brightness		= sin(TimeElapsed) * 0.1;
	vec2 Resolution		= vec2(cvSplashData.x, cvSplashData.y);
	float AspectRatio		= Resolution.x / Resolution.y;
	vec3 InnerColor		= vec3( 0.50, 0.50, 0.50 );
	vec3 OuterColor		= vec3( 0.00, 0.45, 0.00 );
	vec3 WaveColor		= vec3( 1.00, 1.00, 1.00 );
    
	// Input
	vec2 uv				= (InUV + 1.0) / 2.0;
    
	// Output
	vec4 outColor			= vec4(0.0, 0.0, 0.0, 0.0);
    
	// Positioning
	vec2 outerPos			= -0.5 + uv;
	outerPos.x				*= AspectRatio;
    
	vec2 innerPos			= InUV * ( 2.0 - Brightness );
	innerPos.x				*= AspectRatio;
    
	// "logic"
	float innerWidth		= length(outerPos);
	float circleRadius		= 0.24 + Brightness * 0.1;
	float invCircleRadius 	= 1.0 / circleRadius;
	float circleFade		= pow(length(2.0 * outerPos), 0.5);
	float invCircleFade		= 1.0 - circleFade;
	float circleIntensity	= pow(invCircleFade * max(1.1 - circleFade, 0.0), 2.0) * 40.0;
  	float circleWidth		= dot(innerPos,innerPos);
	float circleGlow		= ((1.0 - sqrt(abs(1.0 - circleWidth))) / circleWidth) + Brightness * 0.5;
	float outerGlow			= min( max( 1.0 - innerWidth * ( 1.0 - Brightness ), 0.0 ), 1.0 );
	float waveIntensity		= 0.0;
	
	// Inner circle logic
	if( innerWidth < circleRadius )
	{
		circleIntensity		*= pow(innerWidth * invCircleRadius, 24.0);
		
		float waveWidth		= 0.05;
		vec2 waveUV		= InUV;
        
		waveUV.y			+= 0.14 * cos(TimeElapsed + (waveUV.x * 2.0));
		waveIntensity		+= abs(1.0 / (130.0 * waveUV.y));
        
		waveUV.x			+= 0.14 * sin(TimeElapsed + (waveUV.y * 2.0));
		waveIntensity		+= abs(1.0 / (130.0 * waveUV.x));
        
		waveIntensity		*= 1.0 - pow((innerWidth / circleRadius), 3.0);
	}
    
	// Compose outColor
	outColor.rgb	= outerGlow * OuterColor;
	outColor.rgb	+= circleIntensity * InnerColor;
	outColor.rgb	+= circleGlow * InnerColor * (0.6 + Brightness * 1.2);
	outColor.rgb	+= WaveColor * waveIntensity;
	outColor.rgb	+= circleIntensity * InnerColor;
	outColor.a		= 1.0;
    
	// Fade in
	outColor.rgb	= clamp(outColor.rgb, 0.0, 1.0);
	outColor.rgb	*= min(TimeElapsed / 2.0, 1.0);
    
	//////////////////////////////////////////////////////////////
	// Start of ShaderToy Output Compat
	//////////////////////////////////////////////////////////////
    
	gl_FragColor = outColor;
}
                                                   );



#endif
