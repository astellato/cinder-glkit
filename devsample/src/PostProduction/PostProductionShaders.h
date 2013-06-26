//
//  PostProductionShaders.h
//  InstaSpace
//
//  Created by Anthony Stellato on 2/11/13.
//
//  Some shaders from:

// Bilateral adapted from // http://image.inha.ac.kr/oopgpu/index.php?page=glsl

/*
 
 Cinder Deferred Shading and Post Production Utilities
 
 
 SSAO Shader coming from:
 
 http://www.gamedev.net/topic/556187-the-best-ssao-ive-seen/
 http://www.gamedev.net/page/reference/index.html/_/reference/programming/140/lighting-and-shading/a-simple-and-practical-approach-to-ssao-r2753
 
 Compositing and Colorimetry Shaders coming from:
 
 http://mouaif.wordpress.com/2009/01/05/photoshop-math-with-glsl-shaders/
 
 
 Copyright (c) 2011, Simon Geilfus
 All rights reserved.
 
 http://antivj.com | http://naninteractive.com | http://kinesis.be
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef InstaSpace_PostProductionShaders_h
#define InstaSpace_PostProductionShaders_h

#pragma once

#include <string>

#define STRINGIFY(A) #A

const std::string shaderVertPassthru = STRINGIFY(
                                            //#version 120
                                            varying vec3 normal;
                                            varying vec3 position;
                                            varying vec2 uv;
                                              
                                            void main()
                                            {
                                                normal = normalize( gl_NormalMatrix * gl_Normal );
                                                vec4 v = vec4( gl_Vertex );
                                                  
                                                position = v.xyz;
                                                gl_FrontColor = gl_Color;
                                                gl_Position = ftransform();
                                                gl_TexCoord[0] = gl_MultiTexCoord0;
                                                uv = gl_TexCoord[0].st;
                                            }
);

const std::string shaderFragPassthru = STRINGIFY(
                                            //#version 120
                                            uniform sampler2D tex0;
                                            void main(void)
                                            {
                                            vec4 texcolor = texture2D(tex0, gl_TexCoord[0].xy);
                                                gl_FragColor.rgb = texcolor.rgb;
                                                gl_FragColor.a = texcolor.a;
                                            }
);

//Stereo Anaglyph - Use pass thru vert
//From libcinder.org, Stereoscopic Rendering Example
const std::string shaderAnaglyphFrag = STRINGIFY(
                                            //#version 120
                                           
                                            uniform sampler2D	tex0;
                                            uniform vec4		clr_left;
                                            uniform vec4		clr_right;
                                           
                                            // find the correct half of the texture to sample (side-by-side FBO)
                                            const vec2			halfOffset = vec2(0.5, 1.0);
                                            const vec2			offset = vec2(0.5, 0.0);
                                           
                                            void main()
                                            {	
                                                vec2 st = gl_TexCoord[0].st * halfOffset;
    
                                                gl_FragColor = clr_left * texture2D( tex0, st ) + clr_right * texture2D( tex0, st + offset );
                                            }

);

const std::string shaderFragQuickBlur = STRINGIFY(
                                            uniform sampler2D	tex0;
                                            uniform vec2		sampleOffset;
                                            
                                            float weights[21];
                                            
                                            void main()
                                            {
                                                weights[0] = 0.009167927656011385;
                                                weights[1] = 0.014053461291849008;
                                                weights[2] = 0.020595286319257878;
                                                weights[3] = 0.028855245532226279;
                                                weights[4] = 0.038650411513543079;
                                                weights[5] = 0.049494378859311142;
                                                weights[6] = 0.060594058578763078;
                                                weights[7] = 0.070921288047096992;
                                                weights[8] = 0.079358891804948081;
                                                weights[9] = 0.084895951965930902;
                                                weights[10] = 0.086826196862124602;
                                                weights[11] = 0.084895951965930902;
                                                weights[12] = 0.079358891804948081;
                                                weights[13] = 0.070921288047096992;
                                                weights[14] = 0.060594058578763092;
                                                weights[15] = 0.049494378859311121;
                                                weights[16] = 0.038650411513543079;
                                                weights[17] = 0.028855245532226279;
                                                weights[18] = 0.020595286319257885;
                                                weights[19] = 0.014053461291849008;
                                                weights[20] = 0.009167927656011385;
                                                
                                                
                                                vec3 sum = vec3( 0.0, 0.0, 0.0 );
                                                vec2 offset = vec2( 0.0, 0.0 );
                                                vec2 baseOffset = -10.0 * sampleOffset;
                                                
                                                for( int s = 0; s < 21; ++s ) {
                                                    sum += texture2D( tex0, gl_TexCoord[0].st + baseOffset + offset ).rgb * weights[s];
                                                    offset += sampleOffset;
                                                } 
                                                
                                                gl_FragColor.rgb = sum;
                                                gl_FragColor.a = 1.0;
                                            }

);

//FIX ME!!!!
const std::string shaderVertPassthruBlur = STRINGIFY(
                                                     varying vec2 uv;
                                                     
                                                     // remember that you should draw a screen aligned quad
                                                     void main(void)
                                                     {
                                                         gl_Position = ftransform();;
                                                         
                                                         // Clean up inaccuracies
                                                         vec2 pos;
                                                         pos = sign(gl_Vertex.xy);
                                                         
                                                         gl_Position = vec4(pos, 0.0, 1.0);
                                                         // Image-space
                                                         uv = pos + 0.5;//  + 0.5;
                                                         //uv = gl_TexCoord[0].st;
                                                     }
);

const std::string shaderFragBlurVert = STRINGIFY(
                                                 //#version 120
                                                 uniform vec2		strength;
                                                 uniform sampler2D	tex0;
                                                 varying vec2 uv;
                                                 
                                                 void main(void)
                                                 {
                                                     vec4 sum = vec4(0.0);
                                                     
                                                     // blur in y (vertical)
                                                     // take nine samples, with the distance blurSize between them
                                                     sum += texture2D(tex0, vec2(uv.x - 4.0*strength.x, uv.y)) * 0.05;
                                                     sum += texture2D(tex0, vec2(uv.x - 3.0*strength.x, uv.y)) * 0.09;
                                                     sum += texture2D(tex0, vec2(uv.x - 2.0*strength.x, uv.y)) * 0.12;
                                                     sum += texture2D(tex0, vec2(uv.x - strength.x, uv.y)) * 0.15;
                                                     sum += texture2D(tex0, vec2(uv.x, uv.y)) * 0.16;
                                                     sum += texture2D(tex0, vec2(uv.x + strength.x, uv.y)) * 0.15;
                                                     sum += texture2D(tex0, vec2(uv.x + 2.0*strength.x, uv.y)) * 0.12;
                                                     sum += texture2D(tex0, vec2(uv.x + 3.0*strength.x, uv.y)) * 0.09;
                                                     sum += texture2D(tex0, vec2(uv.x + 4.0*strength.x, uv.y)) * 0.05;
                                                     
                                                     gl_FragColor = sum;
                                                 }
);

const std::string shaderFragBlurHorz = STRINGIFY(
                                                //#version 120
                                                uniform float strength;
                                                uniform sampler2D tex0;
                                                void main(){
                                                    vec4 color = vec4(0.0);
                                                    vec2 uv = gl_TexCoord[0].xy;
                                                    uv.y = 1.0 - uv.y;
                                                    color = texture2D(tex0, uv) * 0.2270270270;
                                                    color += texture2D(tex0, uv + strength*vec2(1.3846153846, 0.0)) * 0.3162162162;
                                                    color += texture2D(tex0, uv - strength*vec2(1.3846153846, 0.0)) * 0.3162162162;
                                                    color += texture2D(tex0, uv + strength*vec2(3.2307692308, 0.0)) * 0.0702702703;
                                                    color += texture2D(tex0, uv - strength*vec2(3.2307692308, 0.0)) * 0.0702702703;
                                                    gl_FragColor = color;
                                                }
);

const std::string shaderFragInvert = STRINGIFY(
                                               //#version 120
                                               uniform sampler2D	tex0;
                                               void main(void)
                                               {
                                                   vec4 texcolor = texture2D(tex0, gl_TexCoord[0].st);
                                                   gl_FragColor.rgb = 1.0 - texcolor.rgb;
                                                   gl_FragColor.a = texcolor.a;
                                               }
);

const std::string shaderFragPixelate = STRINGIFY(
                                                //#version 120
                                                uniform sampler2D	tex0;
                                                uniform vec2 sampleDivisor;
                                                 
                                                void main( void )
                                                {
                                                    vec2 samplePos = gl_TexCoord[0].st - mod( gl_TexCoord[0].st, sampleDivisor );
                                                    gl_FragColor = texture2D( tex0, samplePos );
                                                    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                                                }
);

// prewitt edge detect, use passthru vert
const std::string shaderFragPrewitt = STRINGIFY(
                                                uniform sampler2D	tex0;
                                                uniform vec2		size;
                                                
                                                void main(void)
                                                {
                                                    vec4 sample[9];
                                                    vec2 offset[9];
                                                    float xCoord = 1.0 / size.x;
                                                    float yCoord = 1.0 / size.y;
                                                    offset[0] = vec2(-xCoord, -yCoord);	offset[1] = vec2(0.0, -yCoord);	offset[2] = vec2(xCoord, -yCoord);
                                                    offset[3] = vec2(-xCoord, 0.0);	offset[4] = vec2(0.0, 0.0);		offset[5] = vec2(xCoord, 0.0);
                                                    offset[6] = vec2(-xCoord, yCoord);	offset[7] = vec2(0.0, yCoord);		offset[8] = vec2(xCoord, yCoord);
                                                    
                                                    for (int i = 0; i < 9; i++)
                                                    {
                                                        sample[i] = texture2D(tex0,
                                                                              gl_TexCoord[0].st + offset[i]);
                                                    }
                                                    
                                                    vec4 horizEdge = sample[2] + sample[5] + sample[8] -
                                                    (sample[0] + sample[3] + sample[6]);
                                                    
                                                    vec4 vertEdge = sample[0] + sample[1] + sample[2] -
                                                    (sample[6] + sample[7] + sample[8]);
                                                    
                                                    gl_FragColor.rgb = sqrt((horizEdge.rgb * horizEdge.rgb) +
                                                                            (vertEdge.rgb * vertEdge.rgb));
                                                    gl_FragColor.a = 1.0;
                                                }
);

// light scatter, use passthru vert
const std::string shaderFragScatter = STRINGIFY(
                                                //#version 120
                                                uniform float exposure;
                                                uniform float decay;
                                                uniform float density;
                                                uniform float weight;
                                                uniform vec2 lightPositionOnScreen;
                                                uniform sampler2D tex0;
                                                uniform int nSamples;
                                                
                                                void main()
                                                {
                                                    vec2 deltaTextCoord = vec2( gl_TexCoord[0].st - lightPositionOnScreen.xy );
                                                    vec2 textCoo = gl_TexCoord[0].st;
                                                    deltaTextCoord *= 1.0 /  float(nSamples) * density;
                                                    float illuminationDecay = 1.0;
                                                    
                                                    for(int i=0; i < nSamples ; i++)
                                                    {
                                                        textCoo -= deltaTextCoord;
                                                        vec4 sample = texture2D(tex0, textCoo );
                                                        
                                                        sample *= illuminationDecay * weight;
                                                        
                                                        gl_FragColor += sample;
                                                        
                                                        illuminationDecay *= decay;
                                                    }
                                                    
                                                    gl_FragColor *= exposure;
                                                }
);

// radial blur shader
// this vertex shader is from AMD RenderMonkey
const std::string shaderVertBlurRadial = STRINGIFY(
                                                   varying vec2  uv;
                                                   
                                                   void main(void)
                                                   {
                                                       gl_Position = vec4( gl_Vertex.xy, 0.0, 1.0 );
                                                       gl_Position = sign( gl_Position );
                                                       
                                                       // Texture coordinate for screen aligned (in correct range):
                                                       uv = (vec2( gl_Position.x, - gl_Position.y ) + vec2( 1.0 ) ) / vec2( 2.0 );
                                                   }
);

const std::string shaderFragBlurRadial = STRINGIFY(
                                                   uniform sampler2D tex0;
                                                   
                                                   // texture coordinates passed from the vertex shader
                                                   varying vec2 uv;
                                                   
                                                   // some const, tweak for best look
                                                   const float sampleDist = 1.0;
                                                   const float sampleStrength = 2.2;
                                                   
                                                   void main(void)
                                                   {
                                                       // some sample positions
                                                       float samples[10] =
                                                       float[](-0.08,-0.05,-0.03,-0.02,-0.01,0.01,0.02,0.03,0.05,0.08);
                                                       
                                                       // 0.5,0.5 is the center of the screen
                                                       // so substracting uv from it will result in
                                                       // a vector pointing to the middle of the screen
                                                       vec2 dir = 0.5 - uv;
                                                       
                                                       // calculate the distance to the center of the screen
                                                       float dist = sqrt(dir.x*dir.x + dir.y*dir.y);
                                                       
                                                       // normalize the direction (reuse the distance)
                                                       dir = dir/dist;
                                                       
                                                       // this is the original colour of this fragment
                                                       // using only this would result in a nonblurred version
                                                       vec4 color = texture2D(tex0,uv);
                                                       
                                                       vec4 sum = color;
                                                       
                                                       // take 10 additional blur samples in the direction towards
                                                       // the center of the screen
                                                       for (int i = 0; i < 10; i++)
                                                       {
                                                           sum += texture2D( tex0, uv + dir * samples[i] * sampleDist );
                                                       }
                                                       
                                                       // we have taken eleven samples
                                                       sum *= 1.0/11.0;
                                                       
                                                       // weighten the blur effect with the distance to the
                                                       // center of the screen ( further out is blurred more)
                                                       float t = dist * sampleStrength;
                                                       t = clamp( t ,0.0,1.0); //0 &lt;= t &lt;= 1
                                                       
                                                       //Blend the original color with the averaged pixels
                                                       gl_FragColor = mix( color, sum, t );
                                                   }

);

//const std::string shaderFragBilateral = STRINGIFY(
//                                                  //#version 120
//                                                  
//                                                  uniform sampler2D	tex0;
//                                                  uniform vec2		size;
//                                                  //uniform float domain_sigma = 18.0;
//                                                  //uniform float range_sigma = 0.07;
//                                                  //uniform int bilateral_win_size = 5;
//                                                  uniform float domain_sigma;
//                                                  uniform float range_sigma;
//                                                  uniform int bilateral_win_size;
//                                                  
//                                                  const float pi = 3.14159;
//                                                  
//                                                  void main()
//                                                  {
//                                                      int i,j;
//                                                      float xInc = 1.0 / size.x;
//                                                      float yInc = 1.0 / size.y;
//                                                      
//                                                      vec3 graySum = vec3(0.0);
//                                                      vec3 intenSum = vec3(0.0);
//                                                      
//                                                      vec3 inten = vec3(0.0);
//                                                      vec3 winRGB = vec3(0.0);
//                                                      
//                                                      vec3 buf = vec3(0.0);
//                                                      
//                                                      float domain_sigma_2 = domain_sigma*domain_sigma;
//                                                      float range_sigma_2 = range_sigma*range_sigma;
//                                                      
//                                                      float dist;
//                                                      
//                                                      vec3 curRGB = texture2D(tex0, gl_TexCoord[0].st).rgb;
//                                                      
//                                                      int start_id = -bilateral_win_size/2;
//                                                      int stop_id = -start_id;
//                                                      
//                                                      for ( i = start_id ; i <= stop_id; i++ )
//                                                      {
//                                                          for(j = start_id; j <= stop_id; j++ )
//                                                          {
//                                                              vec2 offset;
//                                                              float fX = float(i);
//                                                              float fY = float(j);
//                                                              offset.x = fX * xInc;
//                                                              offset.y = fY * yInc;
//                                                              winRGB = texture2D(tex0, gl_TexCoord[0].st + offset ).rgb;
//                                                              dist = float(i*i) + float(j*j);
//                                                              
//                                                              inten = exp( (-1.0 * dist / (2.0 * domain_sigma_2))) / ( 2.0 * pi * domain_sigma_2) * exp( (-1.0 * (curRGB - winRGB)*(curRGB - winRGB) / (range_sigma_2))) ;
//                                                              intenSum += inten ;
//                                                              graySum += inten * winRGB;
//                                                          }
//                                                      }
//                                                      buf = graySum / intenSum;
//                                                      gl_FragColor = vec4(buf, 1.0);
//                                                  }
//                                                  
//);

#endif
