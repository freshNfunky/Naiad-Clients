#version 130

// -----------------------------------------------------------------------------
//
// maldives.fs
//
// Fragment Shader for Naiad Studio's "Maldives: I wish I was there right now."
//
// Copyright (c) 2010 Exotic Matter AB.  All rights reserved.
//
// This file is part of Naiad Studio.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of Exotic Matter AB nor its contributors may be used to
// endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// -----------------------------------------------------------------------------

uniform sampler2D envSampler;

uniform vec3  cameraPos;
uniform int   normGrad;
uniform vec3  lightDir;
uniform float ambient;

in vec3 fragGradient;
in vec3 fragVertex;

out vec4 color;

void main(void)
{    
     float PI=3.14;

    //color=vec4(102.f/255.f, 
    //           193.f/255.f,
	//       218.f/255.f,
	  //     1.f);

    vec3 grad=fragGradient;

    if(normGrad==1)
	grad=normalize(grad);

    vec3 V=normalize(cameraPos - fragVertex);
    vec3 R=V-grad*2.f*dot(V,grad);

    vec2 uv=vec2((atan(R.x/R.z)+PI)/(2.f*PI), (asin(R.y)+PI/2.f)/PI);

    vec4 envColor = texture2D(envSampler, uv);

    color = envColor*(ambient+dot(lightDir,grad));
}
