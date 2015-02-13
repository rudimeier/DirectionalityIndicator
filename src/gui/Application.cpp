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

#include <QApplication>
#include <QDockWidget>
#include <QToolBox>
#include <QWidget>

#include "core/ProcessingNetwork.h"
#include "core/Connection.h"
#include "core/Filesystem.h"

#include "algorithms/SurfaceLIC.h"
#include "algorithms/RenderTriangles.h"

#include "OGLWidget.h"
#include "DataWidget.h"
#include "MainWindow.h"
#include "AlgorithmStrategies.h"
#include "AlgorithmStrategy.h"
#include "AlgorithmWidget.h"

#include "Application.h"

namespace di
{
    namespace gui
    {
        Application* Application::m_instance = nullptr;

        Application::Application( int argc, char** argv ):
            m_argc( argc ),
            m_argv( argv )
        {
            // Init the singleton ...
            m_instance = this;
        }

        Application::~Application()
        {
            // all the cleanup was done during run().
        }

        int Application::run()
        {
            // Initialize logger

            // Create QApplication
            QApplication application( m_argc, m_argv, true );

            // We need this for shader loading and others.
            di::core::initRuntimePath( QCoreApplication::applicationDirPath().toStdString() );

            // Load settings
            m_settings = new QSettings( "SE", "DirectionalityIndicator" );

            // Create the QMainWindow
            m_mainWindow = new MainWindow();
            m_mainWindow->resize( 1024, 768 );

            // Create the GL output:
            m_mainGLWidget = new OGLWidget( m_mainWindow );
            m_mainWindow->setCentralWidget( m_mainGLWidget );

            // Create the data widget:
            m_dataWidget = new DataWidget( m_mainWindow );
            m_mainWindow->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, m_dataWidget );

            // The dock with all the parameters and stuff
            QDockWidget* tbDock = new QDockWidget( "Algorithm Parameters", m_mainWindow );
            m_algorithmStrategies = new AlgorithmStrategies( tbDock );
            tbDock->setWidget( m_algorithmStrategies );
            tbDock->setObjectName( "AlgorithmParameters" );    // needed for persistent GUI states
            // avoid closable docks.
            tbDock->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
            m_mainWindow->addDockWidget( Qt::DockWidgetArea::RightDockWidgetArea, tbDock );

            // Create the strategies:
            AlgorithmStrategy* s = m_algorithmStrategies->addStrategy( new AlgorithmStrategy( "Surface LIC" ) );
            s->addAlgorithm( new AlgorithmWidget( SPtr< di::algorithms::SurfaceLIC >( new di::algorithms::SurfaceLIC ) ) );

            s = m_algorithmStrategies->addStrategy( new AlgorithmStrategy( "Surface Rendering" ) );
            s->addAlgorithm( new AlgorithmWidget( SPtr< di::algorithms::RenderTriangles >( new di::algorithms::RenderTriangles ) ) );

            // restore stored states/sizes
            m_mainWindow->loadStates();

            // start the processing container
            m_processingNetwork = SPtr< core::ProcessingNetwork >( new core::ProcessingNetwork() );
            m_processingNetwork->start();

            // Tell the data widget that the processing network is ready.
            m_dataWidget->prepareProcessingNetwork();
            m_algorithmStrategies->prepareProcessingNetwork();

            // Hard-code a connection here. This should be done by a GUI or nice "use-case" class or something. For now, we need to get a VIS up and
            // running.
            m_dataWidget->connectDataToStrategies( m_algorithmStrategies );

            // Finally, show the UI
            m_mainWindow->show();

            // run.
            int retVal = application.exec();

            // Stop if not yet done already.
            m_processingNetwork->stop();

            // Clean up and return.
            return retVal;
        }

        QSettings* Application::getSettings()
        {
            return Application::getInstance()->m_settings;
        }

        Application* Application::getInstance()
        {
            return m_instance;
        }

        di::SPtr< di::core::ProcessingNetwork > Application::getProcessingNetwork()
        {
            return Application::getInstance()->m_processingNetwork;
        }
    }
}
