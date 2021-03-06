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

#ifndef DI_PROGRAM_H
#define DI_PROGRAM_H

#include <initializer_list>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include <di/gfx/OpenGL.h>
#include <di/gfx/GLBindable.h>
#include <di/gfx/GLError.h>

#include <di/Types.h>
#include <di/GfxTypes.h>

namespace di
{
    namespace core
    {
        class Shader;

        /**
         * This class collects shaders and links the final program.
         */
        class Program: public GLBindable
        {
        public:
            /**
             * Create program, consisting of the given shaders.
             *
             * \param shaders the shaders to attach.
             */
            explicit Program( const std::vector< SPtr< Shader > >& shaders );

            /**
             * Create program, consisting of the given shaders.
             *
             * \param shaders the shaders to attach.
             */
            explicit Program( std::initializer_list< SPtr< Shader > > shaders );

            /**
             * Destructor.
             */
            virtual ~Program();

            /**
             * Create and link the program. Realizes the shaders if not yet done.
             *
             * \return true if successful.
             */
            bool realize() override;

            /**
             * Clean up. Delete Object.
             */
            void finalize() override;

            /**
             * Bind the program.
             */
            void bind() override;

            /**
             * Get the location of the given attribute. This basically forwards to glGetAttribLocation.
             *
             * \param name the attribute name
             *
             * \return the location
             */
            GLint getAttribLocation( const std::string& name ) const;

            /**
             * Get the location of the given uniform.  This basically forwards to glGetUniformLocation.
             *
             * \param name the uniform name
             *
             * \return the location
             */
            GLint getUniformLocation( const std::string& name ) const;

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             */
            void setUniform( const std::string name, const int& value )
            {
                // get location
                glUniform1i( getUniformLocation( name ), value );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             */
            void setUniform( const std::string name, const float& value )
            {
                // get location
                glUniform1f( getUniformLocation( name ), value );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             */
            void setUniform( const std::string name, const double& value )
            {
                // get location
                glUniform1f( getUniformLocation( name ), value );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             */
            void setUniform( const std::string name, const glm::mat4& value )
            {
                // get location
                glUniformMatrix4fv( getUniformLocation( name ), 1, GL_FALSE, glm::value_ptr( value ) );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             *
             * \return true if successful
             */
            void setUniform( const std::string name, const glm::mat3& value )
            {
                // get location
                glUniformMatrix3fv( getUniformLocation( name ), 1, GL_FALSE, glm::value_ptr( value ) );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             */
            void setUniform( const std::string name, const glm::mat2& value )
            {
                // get location
                glUniformMatrix2fv( getUniformLocation( name ), 1, GL_FALSE, glm::value_ptr( value ) );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             *
             * \return true if successful
             */
            void setUniform( const std::string name, const glm::vec4& value )
            {
                // get location
                glUniform4fv( getUniformLocation( name ), 1, glm::value_ptr( value ) );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             */
            void setUniform( const std::string name, const glm::vec3& value )
            {
                // get location
                glUniform3fv( getUniformLocation( name ), 1, glm::value_ptr( value ) );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform.
             *
             * \param name the name of the uniform
             * \param value the value to set
             */
            void setUniform( const std::string name, const glm::vec2& value )
            {
                // get location
                glUniform2fv( getUniformLocation( name ), 1, glm::value_ptr( value ) );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform. This version expects the vector to be adequately sized.
             *
             * \param name the name of the uniform
             * \param value the value to set
             */
            void setUniform( const std::string name, const std::vector< int >& values )
            {
                glUniform1iv( getUniformLocation( name ), values.size(), values.data() );
                logGLError();
            }

            /**
             * Set the given value to the specified uniform. This version expects an arbitrary sized vector and automatically takes the desired number
             * of elements and fills missing elements with a default value.
             *
             * \param name the name of the uniform
             * \param values the value array
             * \param size the array size of the GLSL uniform
             * \param defaultValue if to few items where specified, use this default
             */
            void setUniform( const std::string name, const std::vector< int >& values, size_t size, int defaultValue )
            {
                // ensure a proper sized vector first:
                std::vector< int > correct( size, defaultValue );

                if( values.size() >= size )
                {
                    std::copy( values.begin(), values.begin() + size, correct.begin() );
                }
                else
                {
                    std::copy( values.begin(), values.end(), correct.begin() );
                }

                // Forward
                setUniform( name, correct );
            }

            /**
             * Define a compile-time value with a given name. Bool version: set as defined only. No value.
             *
             * \param name the name of the definition
             * \param value the value. Needs to provide a << operator.
             */
            void setDefine( const std::string& name, bool value );

            /**
             * Define a compile-time value with a given name.
             *
             * \tparam ValueType the type of the value to set. Needs to provide operator<<.
             * \param name the name of the definition
             * \param value the value. Needs to provide a << operator.
             */
            template< typename ValueType >
            void setDefine( const std::string& name, ValueType value )
            {
                std::stringstream ss;
                ss << value;
                setDefine( name, ss.str().empty(), ss.str() );
            }

            /**
             * Unset any previously set define with the given name
             *
             * \param name the name to remove from the definitions list.
             */
            void unsetDefine( const std::string& name );

        protected:

            /**
             * Define a compile-time value with a given name.
             *
             * \tparam ValueType the type of the value to set. Needs to provide operator<<.
             * \param name the name of the definition
             * \param value the value as string.
             * \param defineOnly true to only define the name
             */
            void setDefine( const std::string& name, bool defineOnly, std::string value );

            /**
             * Compile and link the shader. Assumes the program ID was created previously.
             *
             * \return true if program was build properly.
             */
            virtual bool compileAndLink();

            /**
             * The shader is attached to this program?
             *
             * \param shader the shader to check
             *
             * \return true if attached
             */
            bool isAttached( SPtr< Shader > shader );

        private:
            /**
             * The shaders to attach.
             */
            std::vector< SPtr< Shader > > m_shaders;

            /**
             * Needs to re-compile?
             */
            bool m_needCompile = true;

            /**
             * Keep track of queried uniform locations to avoid this every frame.
             */
            mutable std::map< std::string, GLint > m_uniformLocationCache;

            /**
             * Collect all definitions that need to be applied when compiling. Similar to the -D flag of compilers.
             */
            std::map< std::string, std::pair< bool, std::string > > m_defines;
        };
    }
}

#endif  // DI_PROGRAM_H

