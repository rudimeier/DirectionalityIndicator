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

#include <algorithm>

#define LogTag "gui/OGLWidget"
#include "core/Logger.h"
#include "core/Filesystem.h"
#include "gfx/GL.h"

#include "Application.h"

#include "OGLWidget.h"

namespace di
{
    namespace gui
    {
        OGLWidget::OGLWidget( QWidget* parent ):
            QGLWidget( getDefaultFormat(), parent )
        {
            setAutoBufferSwap( true );

            m_redrawTimer = new QTimer();
            m_redrawTimer->setInterval( 100 );
            QObject::connect( m_redrawTimer, SIGNAL( timeout() ), this, SLOT( update() ), Qt::QueuedConnection );
        }

        OGLWidget::~OGLWidget()
        {
        }

        QGLFormat OGLWidget::getDefaultFormat()
        {
            // Specify an OpenGL 3.3 format using the Core profile.
            QGLFormat glFormat;
            glFormat.setVersion( 3, 3 );
            glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0

            return glFormat;
        }

        void OGLWidget::initializeGL()
        {
            // Get some version info here.
            const GLubyte* renderer = glGetString( GL_RENDERER );
            const GLubyte* version = glGetString( GL_VERSION );

            // Enable "experimental" for 3.3 functionality
            glewExperimental = true;
            GLenum err = glewInit();
            if( GLEW_OK != err )
            {
                LogE << "Error during GLEW initialization: " << glewGetErrorString( err ) << ". Expect problems." << LogEnd;
                exit( 1 );
            }

            logGLError();

            LogI << "GL Renderer: " << renderer << LogEnd;
            LogI << "GL Version: " << version << LogEnd;
            LogI << "GLEW Version: " << glewGetString( GLEW_VERSION ) << LogEnd;

            // Black background
            glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

            // We want depth testing
            glEnable( GL_DEPTH_TEST );
            // The commonly used default is to interpret smaller values as being closer:
            glDepthFunc( GL_LESS );

            // Create the full-screen quad for background rendering. Note: the camera is configured to project [-1,1] at each axis to the viewport.
            float points[] = {
             -1.0f,  1.0f,  0.0f,
              1.0f,  1.0f,  0.0f,
              1.0f, -1.0f,  0.0f,

              1.0f, -1.0f,  0.0f,
             -1.0f, -1.0f,  0.0f,
             -1.0f,  1.0f,  0.0f
            };

            // Create Vertex Array Object
            glGenVertexArrays( 1, &m_backgroundVAO );
            glBindVertexArray( m_backgroundVAO );

            // Create and fill the VBO for the BG
            glGenBuffers( 1, &m_backgroundVBO );
            glBindBuffer( GL_ARRAY_BUFFER, m_backgroundVBO );
            // Set the data
            glBufferData( GL_ARRAY_BUFFER, 2 * 9 * sizeof( float ), points, GL_STATIC_DRAW );

            std::string localShaderPath = core::getRuntimePath() + "/gfx/shaders/";
            m_bgVertexShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Vertex,
                                                               core::readTextFile( localShaderPath + "CameraEffectHorizon-vertex.glsl" ) );
            m_bgFragmentShader = std::make_shared< core::Shader >( core::Shader::ShaderType::Fragment,
                                                                 core::readTextFile( localShaderPath + "CameraEffectHorizon-fragment.glsl" ) );

            // Link them to build the program itself
            // m_bgShaderProgram = std::make_shared< di::core::Program >( { m_bgVertexShader, m_bgFragmentShader } );
            // NOTE: the above code does not compile on CLang.
            m_bgShaderProgram = SPtr< di::core::Program >( new di::core::Program( { m_bgVertexShader, m_bgFragmentShader } ) );
            m_bgShaderProgram->realize();

            // Allow all visualizations to prepare:
            Application::getProcessingNetwork()->visitVisualizations(
                []( SPtr< di::core::Visualization > vis )
                {
                    vis->prepare();
                }
            );

            m_redrawTimer->start();
        }

        void OGLWidget::resizeGL( int w, int h )
        {
            // Use a basic viewport setup. Equal to window size.
            // NOTE: avoid issues with zero-sized widgets. This sometimes happen during creation.
            glViewport( 0, 0, std::max( 1, w ), std::max( 1, h ) );
        }

        void OGLWidget::paintGL()
        {
            // Allow all visualizations to update:
            Application::getProcessingNetwork()->visitVisualizations(
                []( SPtr< di::core::Visualization > vis )
                {
                    vis->update();
                }
            );

            // Cleanup buffers
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

            // Draw background first:
            m_bgShaderProgram->bind();

            glBindVertexArray( m_backgroundVAO );
            glEnableVertexAttribArray( 0 );
            glBindBuffer( GL_ARRAY_BUFFER, m_backgroundVBO );

            glVertexAttribPointer(
                0, // Attribute number.
                3, // It is a vec3
                GL_FLOAT, // floats
                GL_FALSE, // not normalized
                0,        // no stride
                nullptr   // no buffer offset
            );

            // Draw the BG
            glDrawArrays( GL_TRIANGLES, 0, 6 ); // 3 indices starting at 0 -> 1 triangle

            glDisableVertexAttribArray( 0 );

            // Draw scenes
            // Allow all visualizations to render:
            Application::getProcessingNetwork()->visitVisualizations(
                []( SPtr< di::core::Visualization > vis )
                {
                    vis->render();
                }
            );
        }

        void OGLWidget::closeEvent( QCloseEvent* event )
        {
            m_redrawTimer->stop();

            // Allow all visualizations to finalize:
            Application::getProcessingNetwork()->visitVisualizations(
                []( SPtr< di::core::Visualization > vis )
                {
                    vis->finalize();
                }
            );

            // Clean up properly
            glDeleteBuffers( 1, &m_backgroundVBO );
            // glDeleteVertexArrays( 1, &m_backgroundVAO );
            // glDeleteProgram( m_backgroundShaderProgram );

            QGLWidget::closeEvent( event );
        }
    }
}

