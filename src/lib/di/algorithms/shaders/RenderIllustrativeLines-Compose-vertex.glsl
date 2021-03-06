//---------------------------------------------------------------------------------------
//
// Project: DirectionalityIndicator
//
// Copyright 2014-2015 Sebastian Eichelbaum (http://www.sebastian-eichelbaum.de)
//           2014-2015 Max Planck Research Group "Neuroanatomy and Connectivity"
//
// This file is part of DirectionalityIndicator.
//
// DirectionalityIndicator is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DirectionalityIndicator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DirectionalityIndicator. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------------------

#version 330

// Uniforms
uniform vec2 u_viewportScale = vec2( 1.0 );
uniform mat4 u_ViewMatrix;
uniform vec3 u_bbSize;

// Attribute data
in vec3 position;

// Varying out
out vec2 v_texCoord;
out float v_zoom;

void main()
{

    vec4 scaled = u_ViewMatrix * length( u_bbSize ) * normalize( vec4( 1.0, 1.0, 1.0, 0.0 ) );
    v_zoom = length( scaled );

    v_texCoord = u_viewportScale * 0.5 * ( vec2( 1.0, 1.0 ) + position.xy );
    gl_Position = vec4( vec3( position.xy, 0.0 ), 1.0 );
}

