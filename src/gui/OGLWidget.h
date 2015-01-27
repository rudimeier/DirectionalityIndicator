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

#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include "gfx/GL.h"

#include <QTimer>
#include <QWidget>
// NOTE: QGLWidget is obsolete in Qt5, but the replacement QOpenGLWidget is only available in Qt 5.4+ - we keep QGLWidget for now.
// #include <QOpenGLWidget>
#include <QGLWidget>

#include "GfxTypes.h"

namespace di
{
    namespace gui
    {
        /**
         * A wrapper around the Qt OpenGL widget. This specific widget implements the basic interaction and visualization functionalities of this
         * application.
         */
        class OGLWidget: public QGLWidget, public core::View
        {
            Q_OBJECT
        public:
            /**
             * Create the OpenGL widget.
             *
             * \param parent the parent widget.
             */
            explicit OGLWidget( QWidget* parent = nullptr );

            /**
             * Destroy and clean up.
             */
            virtual ~OGLWidget();

            /**
             * Get a default GL format. In our case, this is a OpenGL 3.3 core format.
             *
             * \return the format
             */
            static QGLFormat getDefaultFormat();

            /**
             * Get the origin of the viewport.
             *
             * \return the origin
             */
            glm::vec2 getViewportOrigin() const override;

            /**
             * Gets the size of the viewport in x and y directions. Keep in mind that the physical pixel coordinate of the upper-right corner will be
             * origin + size - (1,1). Alternatively, use \ref getViewport() to query this directly.
             *
             * \return
             */
            glm::vec2 getViewportSize() const override;

            /**
             * Get the camera of this view.
             *
             * \return the camera.
             */
            const core::Camera& getCamera() const override;
        protected:
            /**
             * Do the necessary setup.
             */
            void initializeGL() override;

            /**
             * Resize. Needed for camera setup, viewports and similar.
             *
             * \param w the width
             * \param h the height
             */
            void resizeGL( int w, int h ) override;

            /**
             * Paint. This iterates the visualizations and lets them draw.
             */
            void paintGL() override;

            /**
             * Widget is going to be destroyed.
             *
             * \param event the event
             */
            void closeEvent( QCloseEvent* event ) override;

            /**
             * Mouse button pressed.
             *
             * \param event the event
             */
            void mousePressEvent( QMouseEvent* event ) override;

            /**
             * Mouse button released.
             *
             * \param event the event
             */
            void mouseReleaseEvent( QMouseEvent* event ) override;

            /**
             * Mouse move.
             *
             * \param event the move event
             */
            void mouseMoveEvent( QMouseEvent* event ) override;

            /**
             * Mouse wheel - used for zoom
             *
             * \param event the wheel event
             */
            void wheelEvent( QWheelEvent* event ) override;

            /**
             * Key events. Used to implement interactions with cam and so on.
             *
             * \param event the event
             */
            void keyReleaseEvent( QKeyEvent* event ) override;

            /**
             * Convert the given mouse coordinates to normalized screen coordinates.
             *
             * \param x x mouse coordinate
             * \param y y mouse coordinate
             *
             * \return screen coordinate
             */
            glm::vec3 toScreenCoord( double x, double y );

        private:
            /**
             * The VBO used for the background.
             */
            GLuint m_backgroundVBO = 0;

            /**
             * The Vertex Attribute Array Object (VAO) used for the background.
             */
            GLuint m_backgroundVAO = 0;

            /**
             * The shader used for rendering
             */
            SPtr< di::core::Program > m_bgShaderProgram = nullptr;

            /**
             * Vertex shader.
             */
            SPtr< di::core::Shader > m_bgVertexShader = nullptr;

            /**
             * Fragment shader.
             */
            SPtr< di::core::Shader > m_bgFragmentShader = nullptr;

            /**
             * Redraw periodically. Only temporary solution.
             */
            QTimer* m_redrawTimer;

            /**
             * Use the arcball functionality.
             */
            int m_arcballState = 0;

            /**
             * Previous position.
             */
            glm::vec3 m_arcballPrevPos;

            /**
             * Current position.
             */
            glm::vec3 m_arcballCurrPos;

            /**
             * Camera axis to use.
             */
            glm::vec3 m_arcballCamAxis = glm::vec3( 0.0f, 1.0f, 0.0f );

            /**
             * Roll speed.
             */
            GLfloat m_arcballRollSpeed = 0.10;

            /**
             * Current angle.
             */
            GLfloat m_arcballAngle = 0.0;

            /**
             * Arcball on X axis.
             */
            bool m_arcballXAxis = true;

            /**
             * Arcball on Y axis.
             */
            bool m_arcballYAxis = true;

            /**
             * The current arcball matrix.
             */
            glm::mat4 m_arcballMatrix;

            /**
             * The previous arcball matrix.
             */
            glm::mat4 m_arcballPrevMatrix;

            /**
             * Camera zoom.
             */
            double m_zoom = 1.0;

            /**
             * The camera of the view.
             */
            core::Camera m_camera;
        };
    }
}

#endif  // OGLWIDGET_H

