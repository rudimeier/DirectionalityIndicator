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

#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

#include <di/gui/events/CallbackEvent.h>

#include <di/gui/Application.h>

#include "MainWindow.h"

namespace di
{
    namespace gui
    {
        MainWindow::MainWindow( QString title, QWidget* parent ):
            QMainWindow( parent )
        {
            setWindowTitle( title );
            setObjectName( "MainWindow" );
        }

        MainWindow::~MainWindow()
        {
        }

        void MainWindow::loadStates()
        {
            restoreGeometry( Application::getSettings()->value( "gui/geometry" ).toByteArray() );
            restoreState( Application::getSettings()->value( "gui/windowState" ).toByteArray() );
        }

        void MainWindow::saveStates()
        {
            Application::getSettings()->setValue( "gui/geometry", saveGeometry() );
            Application::getSettings()->setValue( "gui/windowState", saveState() );
        }

        void MainWindow::closeEvent( QCloseEvent* event )
        {
            saveStates();

            emit( shutdown() );

            // also manually forward:
            Application::getInstance()->onShutdown();

            QMainWindow::closeEvent( event );
        }

        void MainWindow::addMenu()
        {
            addProjectMenu();
        }

        void MainWindow::addProjectMenu()
        {
            auto menu = menuBar();
            auto projectMenu = menu->addMenu( "Project" );
            auto loadAction = projectMenu->addAction( "Load" );
            auto saveAction = projectMenu->addAction( "Save" );

            projectMenu->addSeparator();
            auto saveCamAction = projectMenu->addAction( "Save Camera Only" );
            auto saveParamAction = projectMenu->addAction( "Save Parameters Only" );
            auto saveDataAction = projectMenu->addAction( "Save Data Only" );

            projectMenu->addSeparator();
            auto exitAction = projectMenu->addAction( "Quit" );

            loadAction->setShortcut( QKeySequence::Open );
            saveAction->setShortcut( QKeySequence::Save );
            exitAction->setShortcut( QKeySequence::Quit );

            connect( loadAction, SIGNAL( triggered( bool ) ), this, SLOT( loadProjectHandler() ) );
            connect( saveAction, SIGNAL( triggered( bool ) ), this, SLOT( saveProjectHandler() ) );

            connect( saveCamAction, SIGNAL( triggered( bool ) ), this, SLOT( saveCamHandler() ) );
            connect( saveDataAction, SIGNAL( triggered( bool ) ), this, SLOT( saveDataHandler() ) );
            connect( saveParamAction, SIGNAL( triggered( bool ) ), this, SLOT( saveParamHandler() ) );

            connect( exitAction, SIGNAL( triggered( bool ) ), this, SLOT( close() ) );
        }

        void MainWindow::loadProjectHandler()
        {
            QString lastPath =
                Application::getSettings()->value( "LastProjectFilePath", Application::getSettings()->value( "LastFilePath", "" ) ).toString();
            QString selected = QFileDialog::getOpenFileName( this, "Load File", lastPath, QString( "Project File (*.project)" ) );
            if( selected == "" )
            {
                return;
            }

            // Store the last path
            // QFileInfo fi( selected );
            Application::getSettings()->setValue( "LastProjectFilePath", selected );

            setEnabled( false );

            // Let the application programmer handle the remaining stuff
            try
            {
                LogI << "Loading project from " << selected.toStdString() << LogEnd;
                Application::getInstance()->loadProject( selected );
            }
            catch( std::exception& e )
            {
                QMessageBox::critical( this, "Load failed.", "Loading the project file has failed. Reason: \"" +
                                                                 QString::fromStdString( std::string( e.what() ) ) + "\"." );
            }
            catch( ... )
            {
                QMessageBox::critical( this, "Load failed.", "Loading the project file has failed. View the log for more details." );
            }

            setEnabled( true );
        }

        void MainWindow::saveProjectHandler( bool all, bool viewOnly, bool paramsOnly, bool dataOnly )
        {
            QString lastPath =
                Application::getSettings()->value( "LastProjectFilePath", Application::getSettings()->value( "LastFilePath", "" ) ).toString();
            QString selected = QFileDialog::getSaveFileName( this, "Save File", lastPath, QString( "Project File (*.project)" ) );
            if( selected == "" )
            {
                return;
            }

            // Store the last path
            QFileInfo fi( selected );
            if( fi.suffix() != "project" )
            {
                selected += ".project";
            }

            Application::getSettings()->setValue( "LastProjectFilePath", selected );

            setEnabled( false );

            // Let the application programmer handle the remaining stuff
            try
            {
                LogI << "Saving project to " << selected.toStdString() << LogEnd;
                Application::getInstance()->saveProject( selected, all, viewOnly, paramsOnly, dataOnly );
            }
            catch( std::exception& e )
            {
                QMessageBox::critical( this, "Save failed.",
                                       "Saving the project file has failed. Reason: \"" + QString::fromStdString( std::string( e.what() ) ) + "\"." );
                ;
            }
            catch( ... )
            {
                QMessageBox::critical( this, "Save failed.", "Saving the project file has failed. View the log for more details." );
            }

            setEnabled( true );
        }

        void MainWindow::saveParamHandler()
        {
            saveProjectHandler( false, false, true, false );
        }

        void MainWindow::saveDataHandler()
        {
            saveProjectHandler( false, false, false, true );
        }

        void MainWindow::saveCamHandler()
        {
            saveProjectHandler( false, true, false, false );
        }

        bool MainWindow::event( QEvent* event )
        {
            if( event->type() == QT_CALLBACK_EVENT )
            {
                CallbackEvent* ce = dynamic_cast< CallbackEvent* >( event );
                ce->call();
                return true;
            }
            return QMainWindow::event( event );
        }
    }
}
