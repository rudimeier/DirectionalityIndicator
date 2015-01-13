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

#define LogTag "algorithms/DataInject"
#include "core/Logger.h"

#include "DataInject.h"

namespace di
{
    namespace algorithms
    {
        DataInject::DataInject():
            Algorithm( "Data Inject",
                       "This algorithm simply injects data into the processing network. It does not process or load anything." )
        {
            // We require some output.

            // 1: the injection output
            m_dataOutput = addOutput< di::core::AlgorithmDataCompatible >(
                    "Data",
                    "The data that has been injected."
            );
        }

        DataInject::~DataInject()
        {
            // nothing to clean up so far
        }

        void DataInject::process()
        {
            std::lock_guard<std::mutex> lock( m_injectionDataMutex );
            m_dataOutput->setData( m_injectionData );
        }

        void DataInject::inject( ConstSPtr< di::core::AlgorithmDataCompatible > data )
        {
            std::lock_guard<std::mutex> lock( m_injectionDataMutex );
            m_injectionData = data;
        }
    }
}

