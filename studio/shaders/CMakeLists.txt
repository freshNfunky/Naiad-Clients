#
# CMAKE project for Exotic Matter's Naiad Studio GLSL shaders
#
# Copyright (c) 2009-2010 Exotic Matter AB.  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of Exotic Matter AB nor its contributors may be used to
#   endorse or promote products derived from this software without specific
#   prior written permission.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT
#    LIMITED TO,  THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS
#    FOR  A  PARTICULAR  PURPOSE  ARE DISCLAIMED.  IN NO EVENT SHALL THE
#    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
#    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#    BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS  OR  SERVICES;
#    LOSS OF USE,  DATA,  OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER
#    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,  STRICT
#    LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN
#    ANY  WAY OUT OF THE USE OF  THIS SOFTWARE,  EVEN IF ADVISED OF  THE
#    POSSIBILITY OF SUCH DAMAGE.
#

project               (NAIAD_STUDIO_SHADERS)

install_files (/clients/nstudio/shaders .fs 
           iso 
           ghost 
               ghost-pass1
               ghost-pass2
               ghost-pass3
           big-blue 
           false-color 
           channel-color
           maldives
               mesh-lines
               mesh-solid-flat
               mesh-solid-smooth
               mesh-solid-wireframe-flat
               mesh-solid-wireframe-smooth
               mesh-wireframe
               tile-lines
           tile-surface
           field-scalar
           image-plane)

install_files (/clients/nstudio/shaders .vs 
           iso 
           ghost 
               ghost-pass1
               ghost-pass2
               ghost-pass3
           big-blue 
           false-color 
           channel-color
           maldives
               mesh-lines
               mesh-solid-flat
               mesh-solid-smooth
               mesh-solid-wireframe-flat
               mesh-solid-wireframe-smooth
               mesh-wireframe
               tile-lines
           tile-surface
           field-scalar
           image-plane)

