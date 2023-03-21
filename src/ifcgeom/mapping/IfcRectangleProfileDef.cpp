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

#include "../profile_helper.h"

taxonomy::item* mapping::map_impl(const IfcSchema::IfcRectangleProfileDef* inst) {
	const double x = inst->XDim() / 2.0f * length_unit_;
	const double y = inst->YDim() / 2.0f * length_unit_;

	const double tol = conv_settings_.getValue(ConversionSettings::GV_PRECISION);

	if (x < tol || y < tol) {
		Logger::Message(Logger::LOG_NOTICE, "Skipping zero sized profile:", inst);
		return nullptr;
	}

	Eigen::Matrix4d m4;
	bool has_position = true;
#ifdef SCHEMA_IfcParameterizedProfileDef_Position_IS_OPTIONAL
	has_position = !!inst->Position();
#endif
	if (has_position) {
		taxonomy::matrix4 m = as<taxonomy::matrix4>(map(inst->Position()));
		m4 = m.ccomponents();
	}
	
	return profile_helper(m4, {
		{{-x,-y}},
		{{x,-y}},
		{{x,y}},
		{{-x,y}}
	});
}
