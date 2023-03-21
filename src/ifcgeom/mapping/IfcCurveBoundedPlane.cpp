/********************************************************************************
 *                                                                              *
 * This file is part of IfcOpenShell.                                           *
 *                                                                              *
 * IfcOpenShell is free software: you can redistribute it and/or modify         *
 * it under the terms of the Lesser GNU General Public License as published by  *
 * the Free Software Foundation, either version 3.0 of the License, or          *
 * (at your option) any later version.                                          *
 *                                                                              *
 * IfcOpenShell is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 * Lesser GNU General Public License for more details.                          *
 *                                                                              *
 * You should have received a copy of the Lesser GNU General Public License     *
 * along with this program. If not, see <http://www.gnu.org/licenses/>.         *
 *                                                                              *
 ********************************************************************************/

#include "mapping.h"
#define mapping POSTFIX_SCHEMA(mapping)
using namespace ifcopenshell::geometry;

taxonomy::item* mapping::map_impl(const IfcSchema::IfcCurveBoundedPlane* inst) {
	taxonomy::plane pl = as<taxonomy::plane>(map(inst->BasisSurface()));
	auto f = new taxonomy::face;
	f->children.push_back(map(inst->OuterBoundary()));
	
	IfcSchema::IfcCurve::list::ptr boundaries = inst->InnerBoundaries();

	for (IfcSchema::IfcCurve::list::it it = boundaries->begin(); it != boundaries->end(); ++it) {
		f->children.push_back(map(*it));
	}
	f->matrix = pl.matrix;

	return f;
}
