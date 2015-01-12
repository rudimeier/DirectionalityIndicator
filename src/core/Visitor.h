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

#ifndef VISITOR_H
#define VISITOR_H

namespace di
{
    namespace core
    {
        /**
         * Base for all visitors.
         */
        class BaseVisitor
        {
        public:
            /**
             * Destructor.
             */
            virtual ~BaseVisitor() {};
        };

        /**
         * Visit implementation for an arbitrary visitable class.
         *
         * \tparam VisitableType type of the class to actually visit.
         */
        template< class VisitableType >
        class Visitor
        {
        public:
            /**
             * Visit the given visitable. Abstract. Implement in your visitor.
             *
             * \param visitable the visitable
             */
            virtual void visit( VisitableType& visitable ) = 0;
        };

        /**
         * Base type for all classes that can be "visited".
         *
         * \tparam Visitable the visitable class. This is the class that derives from BaseVisible to utilize the CRTP pattern
         */
        template< class VisitableType >
        class BaseVisitable
        {
        public:
            /**
             * The method allows calling a visitor on the deriving class.
             *
             * \tparam VisitorType the type of the visitor.
             * \param visitor the visitor to use
             */
            template< typename VisitorType >
            void accept( VisitorType& visitor)
            {
                visitor.visit( static_cast< VisitableType& >( *this ) );
            }
        };
    }
}

#endif  // VISITOR_H

