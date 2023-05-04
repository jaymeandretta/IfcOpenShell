#ifdef WITH_USD

#include "USDSerializer.h"

#include "../ifcparse/utils.h"

#include "pxr/usd/sdf/path.h"
#include "pxr/usd/gf/vec3f.h"
#include "pxr/usd/gf/rotation.h"
#include "pxr/usd/tf/token.h"
#include "pxr/usd/usdGeom/xform.h"
#include "pxr/usd/usdGeom/xformOp.h"
#include "pxr/usd/usdGeom/mesh.h"
#include "pxr/usd/usdGeom/tokens.h"
#include "pxr/usd/usdGeom/scope.h"
#include "pxr/usd/usdLux/distantLight.h"
#include "pxr/usd/usdShade/material.h"
#include "pxr/usd/usdShade/shader.h"
#include "pxr/usd/usdShade/tokens.h"
#include "pxr/usd/usdShade/materialBindingAPI.h"
#include "pxr/usd/usdShade/connectableAPI.h"


#include <vector>
#include <string>
#include <sstream>


USDSerializer::USDSerializer(const std::string& filename, const SerializerSettings& settings)
	: WriteOnlyGeometrySerializer(settings)
	, filename_(filename)
	, settings_(settings)
{
  // create a new stage
  stage_ = pxr::UsdStage::CreateNew(filename + ".usda");
  if( !settings().get(SerializerSettings::USE_Y_UP) )
    stage_->SetDefaultPrim(pxr::UsdGeomXform::Define(stage_, pxr::SdfPath("/"), pxr::UsdGeomTokens->z));
  else
    stage_->SetDefaultPrim(pxr::UsdGeomXform::Define(stage_, pxr::SdfPath("/"), pxr::UsdGeomTokens->y));
  // create root xform prim 'World'
  pxr::UsdGeomXform::Define(stage_, pxr::SdfPath("/World"));
  pxr::UsdGeomScope::Define(stage_, pxr::SdfPath("/Looks"));
  createLighting();
  ready_ = true;
}

USDSerializer::~USDSerializer() {
	
}

void USDSerializer::createLighting() {
  // create a distant light
  const std::string& light_path = "/World/defaultLight";
  pxr::UsdLuxDistantLight::Define(stage_, pxr::SdfPath(light_path));
  // set the light's orientation
  pxr::UsdGeomXform xform(stage_->GetPrimAtPath(pxr::SdfPath(light_path)));
  pxr::GfVec3f light_direction(0.0f, 0.0f, -1.0f);
  xform.AddRotateOp(pxr::UsdGeomXformOp::PrecisionFloat, pxr::UsdGeomXformOp::TypeRotateXYZ).Set(pxr::GfRotation(pxr::GfVec3f(0.0f, 1.0f, 0.0f), light_direction).GetQuaternion());
  // set the light's color
  pxr::UsdLuxDistantLight light(stage_->GetPrimAtPath(pxr::SdfPath(light_path)));
  light.CreateIntensityAttr().Set(1000.0f);
  light.CreateColorAttr().Set(pxr::GfVec3f(1.0f, 1.0f, 1.0f));
}

void USDSerializer::writeMaterial(const pxr::UsdGeomMesh& mesh,const IfcGeom::Material& style) {
  std::stringstream ss;
  ss << "/Looks/" << style.original_name();
  pxr::UsdShadeMaterial::Define(stage_, pxr::SdfPath(ss.str()));
  pxr::UsdShadeMaterial material(stage_->GetPrimAtPath(pxr::SdfPath(ss.str())));
  pxr::UsdShadeShader shader = pxr::UsdShadeShader::Define(stage_, pxr::SdfPath(ss.str() + "/Shader"));
  shader.CreateIdAttr().Set(TfToken("UsdPreviewSurface"));

  float rgba[4] { 0.18f, 0.18f, 0.18f, 1.0f };
  if (style.hasDiffuse())
		for (int i = 0; i < 3; ++i) rgba[i] = static_cast<float> style.diffuse()[i];
  shader.CreateInput(pxr::TfToken("diffuseColor"), pxr::SdfValueTypeNames->Color3f).Set(pxr::GfVec3f(rgba[0], rgba[1], rgba[2]));

  if(style.hasTransparency())
    rgba[3] = 1.0f - style.transparency();
  shader.CreateInput(pxr::TfToken("opacity"), pxr::SdfValueTypeNames->Float).Set(rgba[3]);

  if(style.hasSpecular()) {
    for (int i = 0; i < 3; ++i) rgba[i] = static_cast<float> style.specular()[i];
    shader.CreateInput(pxr::TfToken("useSpecularWorkflow"), pxr::SdfValueTypeNames->Int).Set(1);
  } else {
    shader.CreateInput(pxr::TfToken("useSpecularWorkflow"), pxr::SdfValueTypeNames->Int).Set(0);
  }
  shader.CreateInput(pxr::TfToken("specularColor"), pxr::SdfValueTypeNames->Color3f).Set(pxr::GfVec3f(rgba[0], rgba[1], rgba[2]));

  material.CreateSurfaceOutput().ConnectToSource(shader, pxr::TfToken("surface"));
  pxr::UsdShadeMaterialBindingAPI(mesh).Bind(material);
}

bool USDSerializer::ready() {
	return ready_;
}

void USDSerializer::writeHeader() {
  std::stringstream ss;
  ss << "File generated by IfcOpenShell " << IFCOPENSHELL_VERSION;
  stage_->GetRootLayer()->SetComment(ss.str()); 
}

void USDSerializer::write(const IfcGeom::TriangulationElement* o) {
	if ( o->geometry().material_ids().empty() )
		return;
  
  std:sstringstream ss("/World/");
  ss << o->geometry().id();
  pxr::UsdGeomMesh::Define(stage_, pxr::SdfPath(ss.str()));
  const IfcGeom::Representation::Triangulation& mesh = o->geometry();
  const std::vector<double>& m = o->transformation().matrix().data();

  const int vcount = (int)mesh.verts().size() / 3;
  for ( std::vector<double>::const_iterator it = mesh.verts().begin(); it != mesh.verts().end(); ) {
    const double x = *(it++);
    const double y = *(it++);
    const double z = *(it++);
		
		
	}

}

void USDSerializer::finalize() {
  stage_->GetRootLayer()->Save();
}

#endif // WITH_USD