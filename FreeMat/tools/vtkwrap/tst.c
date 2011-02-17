// FreeMat wrapper for vtkRenderer object
//
#define VTK_STREAMS_FWD_ONLY
#include "vtkRenderer.h"

//@@Signature
//gfunction @vtkRenderer:vtkRenderer vtkRendererConstructorFunction
//input a
//output p
ArrayVector vtkRendererConstructorFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0) {
    Array ret(MakeVTKPointer(vtkRenderer::New()));
    ret.structPtr().setClassPath(StringVector() << "vtkRenderer");
    return ret;
  } else if (arg[0].className() == "vtkRenderer") {
    return arg[0];
  } else {
    vtkObjectBase *p = GetVTKPointer<vtkObjectBase>(arg[0]);
    vtkRenderer*q = dynamic_cast<vtkRenderer*>(p);
    if (!q)
      throw Exception("Unable to type convert supplied object to an instance of type vtkRenderer");
    Array ret(arg[0]);
    ret.structPtr().setClassPath(StringVector() << "vtkRenderer");
    return ret;
  }
}
//@@Signature
//gfunction @vtkRenderer:GetClassName vtkRendererGetClassNameFunction
//input a0 
//output y
ArrayVector vtkRendererGetClassNameFunction(int nargout, const ArrayVector& arg) {
  /* Signature const char *GetClassName (); */
  if (arg.size() < 1) 
    throw Exception("Function GetClassName for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  const char    *temp20;
  temp20 = (vtk_pointer)->GetClassName();
  retval = Array(QString(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:IsA vtkRendererIsAFunction
//input a0 a1 
//output y
ArrayVector vtkRendererIsAFunction(int nargout, const ArrayVector& arg) {
  /* Signature int IsA (const char *name); */
  if (arg.size() < 2) 
    throw Exception("Function IsA for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  char    *temp0;
  int      temp20;
  char tmp_string0[1024];
  strcpy(tmp_string0,qPrintable(arg[1].asString())); 
  temp0 = tmp_string0;
  temp20 = (vtk_pointer)->IsA(temp0);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:NewInstance vtkRendererNewInstanceFunction
//input a0 
//output y
ArrayVector vtkRendererNewInstanceFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkRenderer *NewInstance (); */
  if (arg.size() < 1) 
    throw Exception("Function NewInstance for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkRenderer  *temp20;
  temp20 = (vtk_pointer)->NewInstance();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SafeDownCast vtkRendererSafeDownCastFunction
//input a0 a1 
//output y
ArrayVector vtkRendererSafeDownCastFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkRenderer *SafeDownCast (vtkObject* o); */
  if (arg.size() < 2) 
    throw Exception("Function SafeDownCast for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkObject  *temp0;
  vtkRenderer  *temp20;
  temp0 = GetVTKPointer<vtkObject>(arg[1]);
  temp20 = (vtk_pointer)->SafeDownCast(temp0);
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:AddActor vtkRendererAddActorFunction
//input a0 a1 
//output none
ArrayVector vtkRendererAddActorFunction(int nargout, const ArrayVector& arg) {
  /* Signature void AddActor (vtkProp *p); */
  if (arg.size() < 2) 
    throw Exception("Function AddActor for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkProp  *temp0;
  temp0 = GetVTKPointer<vtkProp>(arg[1]);
  vtk_pointer->AddActor(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:AddVolume vtkRendererAddVolumeFunction
//input a0 a1 
//output none
ArrayVector vtkRendererAddVolumeFunction(int nargout, const ArrayVector& arg) {
  /* Signature void AddVolume (vtkProp *p); */
  if (arg.size() < 2) 
    throw Exception("Function AddVolume for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkProp  *temp0;
  temp0 = GetVTKPointer<vtkProp>(arg[1]);
  vtk_pointer->AddVolume(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:RemoveActor vtkRendererRemoveActorFunction
//input a0 a1 
//output none
ArrayVector vtkRendererRemoveActorFunction(int nargout, const ArrayVector& arg) {
  /* Signature void RemoveActor (vtkProp *p); */
  if (arg.size() < 2) 
    throw Exception("Function RemoveActor for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkProp  *temp0;
  temp0 = GetVTKPointer<vtkProp>(arg[1]);
  vtk_pointer->RemoveActor(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:RemoveVolume vtkRendererRemoveVolumeFunction
//input a0 a1 
//output none
ArrayVector vtkRendererRemoveVolumeFunction(int nargout, const ArrayVector& arg) {
  /* Signature void RemoveVolume (vtkProp *p); */
  if (arg.size() < 2) 
    throw Exception("Function RemoveVolume for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkProp  *temp0;
  temp0 = GetVTKPointer<vtkProp>(arg[1]);
  vtk_pointer->RemoveVolume(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:AddLight vtkRendererAddLightFunction
//input a0 a1 
//output none
ArrayVector vtkRendererAddLightFunction(int nargout, const ArrayVector& arg) {
  /* Signature void AddLight (vtkLight *); */
  if (arg.size() < 2) 
    throw Exception("Function AddLight for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkLight  *temp0;
  temp0 = GetVTKPointer<vtkLight>(arg[1]);
  vtk_pointer->AddLight(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:RemoveLight vtkRendererRemoveLightFunction
//input a0 a1 
//output none
ArrayVector vtkRendererRemoveLightFunction(int nargout, const ArrayVector& arg) {
  /* Signature void RemoveLight (vtkLight *); */
  if (arg.size() < 2) 
    throw Exception("Function RemoveLight for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkLight  *temp0;
  temp0 = GetVTKPointer<vtkLight>(arg[1]);
  vtk_pointer->RemoveLight(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:RemoveAllLights vtkRendererRemoveAllLightsFunction
//input a0 
//output none
ArrayVector vtkRendererRemoveAllLightsFunction(int nargout, const ArrayVector& arg) {
  /* Signature void RemoveAllLights (); */
  if (arg.size() < 1) 
    throw Exception("Function RemoveAllLights for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->RemoveAllLights();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetLights vtkRendererGetLightsFunction
//input a0 
//output y
ArrayVector vtkRendererGetLightsFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkLightCollection *GetLights (); */
  if (arg.size() < 1) 
    throw Exception("Function GetLights for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkLightCollection  *temp20;
  temp20 = (vtk_pointer)->GetLights();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:CreateLight vtkRendererCreateLightFunction
//input a0 
//output none
ArrayVector vtkRendererCreateLightFunction(int nargout, const ArrayVector& arg) {
  /* Signature void CreateLight (void ); */
  if (arg.size() < 1) 
    throw Exception("Function CreateLight for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->CreateLight();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:MakeLight vtkRendererMakeLightFunction
//input a0 
//output y
ArrayVector vtkRendererMakeLightFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual vtkLight *MakeLight (); */
  if (arg.size() < 1) 
    throw Exception("Function MakeLight for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkLight  *temp20;
  temp20 = (vtk_pointer)->MakeLight();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetTwoSidedLighting vtkRendererGetTwoSidedLightingFunction
//input a0 
//output y
ArrayVector vtkRendererGetTwoSidedLightingFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetTwoSidedLighting (); */
  if (arg.size() < 1) 
    throw Exception("Function GetTwoSidedLighting for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetTwoSidedLighting();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetTwoSidedLighting vtkRendererSetTwoSidedLightingFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetTwoSidedLightingFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetTwoSidedLighting (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetTwoSidedLighting for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetTwoSidedLighting(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:TwoSidedLightingOn vtkRendererTwoSidedLightingOnFunction
//input a0 
//output none
ArrayVector vtkRendererTwoSidedLightingOnFunction(int nargout, const ArrayVector& arg) {
  /* Signature void TwoSidedLightingOn (); */
  if (arg.size() < 1) 
    throw Exception("Function TwoSidedLightingOn for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->TwoSidedLightingOn();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:TwoSidedLightingOff vtkRendererTwoSidedLightingOffFunction
//input a0 
//output none
ArrayVector vtkRendererTwoSidedLightingOffFunction(int nargout, const ArrayVector& arg) {
  /* Signature void TwoSidedLightingOff (); */
  if (arg.size() < 1) 
    throw Exception("Function TwoSidedLightingOff for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->TwoSidedLightingOff();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetLightFollowCamera vtkRendererSetLightFollowCameraFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetLightFollowCameraFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetLightFollowCamera (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetLightFollowCamera for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetLightFollowCamera(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetLightFollowCamera vtkRendererGetLightFollowCameraFunction
//input a0 
//output y
ArrayVector vtkRendererGetLightFollowCameraFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetLightFollowCamera (); */
  if (arg.size() < 1) 
    throw Exception("Function GetLightFollowCamera for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetLightFollowCamera();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:LightFollowCameraOn vtkRendererLightFollowCameraOnFunction
//input a0 
//output none
ArrayVector vtkRendererLightFollowCameraOnFunction(int nargout, const ArrayVector& arg) {
  /* Signature void LightFollowCameraOn (); */
  if (arg.size() < 1) 
    throw Exception("Function LightFollowCameraOn for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->LightFollowCameraOn();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:LightFollowCameraOff vtkRendererLightFollowCameraOffFunction
//input a0 
//output none
ArrayVector vtkRendererLightFollowCameraOffFunction(int nargout, const ArrayVector& arg) {
  /* Signature void LightFollowCameraOff (); */
  if (arg.size() < 1) 
    throw Exception("Function LightFollowCameraOff for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->LightFollowCameraOff();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetAutomaticLightCreation vtkRendererGetAutomaticLightCreationFunction
//input a0 
//output y
ArrayVector vtkRendererGetAutomaticLightCreationFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetAutomaticLightCreation (); */
  if (arg.size() < 1) 
    throw Exception("Function GetAutomaticLightCreation for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetAutomaticLightCreation();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetAutomaticLightCreation vtkRendererSetAutomaticLightCreationFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetAutomaticLightCreationFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetAutomaticLightCreation (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetAutomaticLightCreation for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetAutomaticLightCreation(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:AutomaticLightCreationOn vtkRendererAutomaticLightCreationOnFunction
//input a0 
//output none
ArrayVector vtkRendererAutomaticLightCreationOnFunction(int nargout, const ArrayVector& arg) {
  /* Signature void AutomaticLightCreationOn (); */
  if (arg.size() < 1) 
    throw Exception("Function AutomaticLightCreationOn for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->AutomaticLightCreationOn();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:AutomaticLightCreationOff vtkRendererAutomaticLightCreationOffFunction
//input a0 
//output none
ArrayVector vtkRendererAutomaticLightCreationOffFunction(int nargout, const ArrayVector& arg) {
  /* Signature void AutomaticLightCreationOff (); */
  if (arg.size() < 1) 
    throw Exception("Function AutomaticLightCreationOff for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->AutomaticLightCreationOff();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:UpdateLightsGeometryToFollowCamera vtkRendererUpdateLightsGeometryToFollowCameraFunction
//input a0 
//output y
ArrayVector vtkRendererUpdateLightsGeometryToFollowCameraFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual int UpdateLightsGeometryToFollowCamera (void ); */
  if (arg.size() < 1) 
    throw Exception("Function UpdateLightsGeometryToFollowCamera for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->UpdateLightsGeometryToFollowCamera();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetVolumes vtkRendererGetVolumesFunction
//input a0 
//output y
ArrayVector vtkRendererGetVolumesFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkVolumeCollection *GetVolumes (); */
  if (arg.size() < 1) 
    throw Exception("Function GetVolumes for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkVolumeCollection  *temp20;
  temp20 = (vtk_pointer)->GetVolumes();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetActors vtkRendererGetActorsFunction
//input a0 
//output y
ArrayVector vtkRendererGetActorsFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkActorCollection *GetActors (); */
  if (arg.size() < 1) 
    throw Exception("Function GetActors for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkActorCollection  *temp20;
  temp20 = (vtk_pointer)->GetActors();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetActiveCamera vtkRendererSetActiveCameraFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetActiveCameraFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetActiveCamera (vtkCamera *); */
  if (arg.size() < 2) 
    throw Exception("Function SetActiveCamera for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkCamera  *temp0;
  temp0 = GetVTKPointer<vtkCamera>(arg[1]);
  vtk_pointer->SetActiveCamera(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetActiveCamera vtkRendererGetActiveCameraFunction
//input a0 
//output y
ArrayVector vtkRendererGetActiveCameraFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkCamera *GetActiveCamera (); */
  if (arg.size() < 1) 
    throw Exception("Function GetActiveCamera for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkCamera  *temp20;
  temp20 = (vtk_pointer)->GetActiveCamera();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:MakeCamera vtkRendererMakeCameraFunction
//input a0 
//output y
ArrayVector vtkRendererMakeCameraFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual vtkCamera *MakeCamera (); */
  if (arg.size() < 1) 
    throw Exception("Function MakeCamera for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkCamera  *temp20;
  temp20 = (vtk_pointer)->MakeCamera();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetErase vtkRendererSetEraseFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetEraseFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetErase (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetErase for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetErase(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetErase vtkRendererGetEraseFunction
//input a0 
//output y
ArrayVector vtkRendererGetEraseFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetErase (); */
  if (arg.size() < 1) 
    throw Exception("Function GetErase for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetErase();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:EraseOn vtkRendererEraseOnFunction
//input a0 
//output none
ArrayVector vtkRendererEraseOnFunction(int nargout, const ArrayVector& arg) {
  /* Signature void EraseOn (); */
  if (arg.size() < 1) 
    throw Exception("Function EraseOn for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->EraseOn();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:EraseOff vtkRendererEraseOffFunction
//input a0 
//output none
ArrayVector vtkRendererEraseOffFunction(int nargout, const ArrayVector& arg) {
  /* Signature void EraseOff (); */
  if (arg.size() < 1) 
    throw Exception("Function EraseOff for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->EraseOff();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetDraw vtkRendererSetDrawFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetDrawFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetDraw (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetDraw for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetDraw(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetDraw vtkRendererGetDrawFunction
//input a0 
//output y
ArrayVector vtkRendererGetDrawFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetDraw (); */
  if (arg.size() < 1) 
    throw Exception("Function GetDraw for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetDraw();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:DrawOn vtkRendererDrawOnFunction
//input a0 
//output none
ArrayVector vtkRendererDrawOnFunction(int nargout, const ArrayVector& arg) {
  /* Signature void DrawOn (); */
  if (arg.size() < 1) 
    throw Exception("Function DrawOn for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->DrawOn();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:DrawOff vtkRendererDrawOffFunction
//input a0 
//output none
ArrayVector vtkRendererDrawOffFunction(int nargout, const ArrayVector& arg) {
  /* Signature void DrawOff (); */
  if (arg.size() < 1) 
    throw Exception("Function DrawOff for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->DrawOff();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:AddCuller vtkRendererAddCullerFunction
//input a0 a1 
//output none
ArrayVector vtkRendererAddCullerFunction(int nargout, const ArrayVector& arg) {
  /* Signature void AddCuller (vtkCuller *); */
  if (arg.size() < 2) 
    throw Exception("Function AddCuller for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkCuller  *temp0;
  temp0 = GetVTKPointer<vtkCuller>(arg[1]);
  vtk_pointer->AddCuller(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:RemoveCuller vtkRendererRemoveCullerFunction
//input a0 a1 
//output none
ArrayVector vtkRendererRemoveCullerFunction(int nargout, const ArrayVector& arg) {
  /* Signature void RemoveCuller (vtkCuller *); */
  if (arg.size() < 2) 
    throw Exception("Function RemoveCuller for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkCuller  *temp0;
  temp0 = GetVTKPointer<vtkCuller>(arg[1]);
  vtk_pointer->RemoveCuller(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetCullers vtkRendererGetCullersFunction
//input a0 
//output y
ArrayVector vtkRendererGetCullersFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkCullerCollection *GetCullers (); */
  if (arg.size() < 1) 
    throw Exception("Function GetCullers for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkCullerCollection  *temp20;
  temp20 = (vtk_pointer)->GetCullers();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetAmbient1 vtkRendererSetAmbient1Function
//input a0 a1 a2 a3 
//output none
ArrayVector vtkRendererSetAmbient1Function(int nargout, const ArrayVector& arg) {
  /* Signature void SetAmbient (double , double , double ); */
  if (arg.size() < 4) 
    throw Exception("Function SetAmbient1 for class vtkRenderer requires 4 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp0;
  double   temp1;
  double   temp2;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  vtk_pointer->SetAmbient(temp0,temp1,temp2);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetAmbient2 vtkRendererSetAmbient2Function
//input a0 a1 
//output none
ArrayVector vtkRendererSetAmbient2Function(int nargout, const ArrayVector& arg) {
  /* Signature void SetAmbient (double  a[3]); */
  if (arg.size() < 2) 
    throw Exception("Function SetAmbient2 for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double temp0[3];
  if ( !(arg[1].isVector() && arg[1].length() == 3) )
    throw Exception("Mismatch in vector lengths");
  Array vect = arg[1].asDenseArray().toClass(Double);
  BasicArray<double> data = vect.real<double>();
  int length = data.length();
  for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
  vtk_pointer->SetAmbient(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetAmbient vtkRendererGetAmbientFunction
//input a0 
//output y
ArrayVector vtkRendererGetAmbientFunction(int nargout, const ArrayVector& arg) {
  /* Signature double  *GetAmbient (); */
  if (arg.size() < 1) 
    throw Exception("Function GetAmbient for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double  *temp20;
  temp20 = (vtk_pointer)->GetAmbient();
  BasicArray<double> tempResult(NTuple(3,1));
  tempResult[1] = (double)temp20[0];
  tempResult[2] = (double)temp20[1];
  tempResult[3] = (double)temp20[2];
  retval = Array(tempResult);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetAllocatedRenderTime vtkRendererSetAllocatedRenderTimeFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetAllocatedRenderTimeFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetAllocatedRenderTime (double ); */
  if (arg.size() < 2) 
    throw Exception("Function SetAllocatedRenderTime for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  vtk_pointer->SetAllocatedRenderTime(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetAllocatedRenderTime vtkRendererGetAllocatedRenderTimeFunction
//input a0 
//output y
ArrayVector vtkRendererGetAllocatedRenderTimeFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual double GetAllocatedRenderTime (); */
  if (arg.size() < 1) 
    throw Exception("Function GetAllocatedRenderTime for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetAllocatedRenderTime();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetTimeFactor vtkRendererGetTimeFactorFunction
//input a0 
//output y
ArrayVector vtkRendererGetTimeFactorFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual double GetTimeFactor (); */
  if (arg.size() < 1) 
    throw Exception("Function GetTimeFactor for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetTimeFactor();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:Render vtkRendererRenderFunction
//input a0 
//output none
ArrayVector vtkRendererRenderFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void Render (); */
  if (arg.size() < 1) 
    throw Exception("Function Render for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->Render();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:DeviceRender vtkRendererDeviceRenderFunction
//input a0 
//output none
ArrayVector vtkRendererDeviceRenderFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void DeviceRender () = 0; */
  if (arg.size() < 1) 
    throw Exception("Function DeviceRender for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->DeviceRender();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:DeviceRenderTranslucentPolygonalGeometry vtkRendererDeviceRenderTranslucentPolygonalGeometryFunction
//input a0 
//output none
ArrayVector vtkRendererDeviceRenderTranslucentPolygonalGeometryFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void DeviceRenderTranslucentPolygonalGeometry (); */
  if (arg.size() < 1) 
    throw Exception("Function DeviceRenderTranslucentPolygonalGeometry for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->DeviceRenderTranslucentPolygonalGeometry();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:Clear vtkRendererClearFunction
//input a0 
//output none
ArrayVector vtkRendererClearFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void Clear (); */
  if (arg.size() < 1) 
    throw Exception("Function Clear for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->Clear();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:VisibleActorCount vtkRendererVisibleActorCountFunction
//input a0 
//output y
ArrayVector vtkRendererVisibleActorCountFunction(int nargout, const ArrayVector& arg) {
  /* Signature int VisibleActorCount (); */
  if (arg.size() < 1) 
    throw Exception("Function VisibleActorCount for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->VisibleActorCount();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:VisibleVolumeCount vtkRendererVisibleVolumeCountFunction
//input a0 
//output y
ArrayVector vtkRendererVisibleVolumeCountFunction(int nargout, const ArrayVector& arg) {
  /* Signature int VisibleVolumeCount (); */
  if (arg.size() < 1) 
    throw Exception("Function VisibleVolumeCount for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->VisibleVolumeCount();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:ComputeVisiblePropBounds1 vtkRendererComputeVisiblePropBounds1Function
//input a0 a1 
//output none
ArrayVector vtkRendererComputeVisiblePropBounds1Function(int nargout, const ArrayVector& arg) {
  /* Signature void ComputeVisiblePropBounds (double bounds[6]); */
  if (arg.size() < 2) 
    throw Exception("Function ComputeVisiblePropBounds1 for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double temp0[6];
  if ( !(arg[1].isVector() && arg[1].length() == 6) )
    throw Exception("Mismatch in vector lengths");
  Array vect = arg[1].asDenseArray().toClass(Double);
  BasicArray<double> data = vect.real<double>();
  int length = data.length();
  for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
  vtk_pointer->ComputeVisiblePropBounds(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:ComputeVisiblePropBounds2 vtkRendererComputeVisiblePropBounds2Function
//input a0 
//output y
ArrayVector vtkRendererComputeVisiblePropBounds2Function(int nargout, const ArrayVector& arg) {
  /* Signature double *ComputeVisiblePropBounds (); */
  if (arg.size() < 1) 
    throw Exception("Function ComputeVisiblePropBounds2 for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double  *temp20;
  temp20 = (vtk_pointer)->ComputeVisiblePropBounds();
  BasicArray<double> tempResult(NTuple(6,1));
  tempResult[1] = (double)temp20[0];
  tempResult[2] = (double)temp20[1];
  tempResult[3] = (double)temp20[2];
  tempResult[4] = (double)temp20[3];
  tempResult[5] = (double)temp20[4];
  tempResult[6] = (double)temp20[5];
  retval = Array(tempResult);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:ResetCameraClippingRange1 vtkRendererResetCameraClippingRange1Function
//input a0 
//output none
ArrayVector vtkRendererResetCameraClippingRange1Function(int nargout, const ArrayVector& arg) {
  /* Signature void ResetCameraClippingRange (); */
  if (arg.size() < 1) 
    throw Exception("Function ResetCameraClippingRange1 for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->ResetCameraClippingRange();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:ResetCameraClippingRange2 vtkRendererResetCameraClippingRange2Function
//input a0 a1 
//output none
ArrayVector vtkRendererResetCameraClippingRange2Function(int nargout, const ArrayVector& arg) {
  /* Signature void ResetCameraClippingRange (double bounds[6]); */
  if (arg.size() < 2) 
    throw Exception("Function ResetCameraClippingRange2 for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double temp0[6];
  if ( !(arg[1].isVector() && arg[1].length() == 6) )
    throw Exception("Mismatch in vector lengths");
  Array vect = arg[1].asDenseArray().toClass(Double);
  BasicArray<double> data = vect.real<double>();
  int length = data.length();
  for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
  vtk_pointer->ResetCameraClippingRange(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:ResetCameraClippingRange3 vtkRendererResetCameraClippingRange3Function
//input a0 a1 a2 a3 a4 a5 a6 
//output none
ArrayVector vtkRendererResetCameraClippingRange3Function(int nargout, const ArrayVector& arg) {
  /* Signature void ResetCameraClippingRange (double xmin, double xmax, double ymin, double ymax, double zmin, double zmax); */
  if (arg.size() < 7) 
    throw Exception("Function ResetCameraClippingRange3 for class vtkRenderer requires 7 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  double   temp4;
  double   temp5;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  if ( !arg[5].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp4 = (double) arg[5].asDouble();
  if ( !arg[6].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp5 = (double) arg[6].asDouble();
  vtk_pointer->ResetCameraClippingRange(temp0,temp1,temp2,temp3,temp4,temp5);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetNearClippingPlaneTolerance vtkRendererSetNearClippingPlaneToleranceFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetNearClippingPlaneToleranceFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetNearClippingPlaneTolerance (double ); */
  if (arg.size() < 2) 
    throw Exception("Function SetNearClippingPlaneTolerance for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  vtk_pointer->SetNearClippingPlaneTolerance(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetNearClippingPlaneToleranceMinValue vtkRendererGetNearClippingPlaneToleranceMinValueFunction
//input a0 
//output y
ArrayVector vtkRendererGetNearClippingPlaneToleranceMinValueFunction(int nargout, const ArrayVector& arg) {
  /* Signature double GetNearClippingPlaneToleranceMinValue (); */
  if (arg.size() < 1) 
    throw Exception("Function GetNearClippingPlaneToleranceMinValue for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetNearClippingPlaneToleranceMinValue();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetNearClippingPlaneToleranceMaxValue vtkRendererGetNearClippingPlaneToleranceMaxValueFunction
//input a0 
//output y
ArrayVector vtkRendererGetNearClippingPlaneToleranceMaxValueFunction(int nargout, const ArrayVector& arg) {
  /* Signature double GetNearClippingPlaneToleranceMaxValue (); */
  if (arg.size() < 1) 
    throw Exception("Function GetNearClippingPlaneToleranceMaxValue for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetNearClippingPlaneToleranceMaxValue();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetNearClippingPlaneTolerance vtkRendererGetNearClippingPlaneToleranceFunction
//input a0 
//output y
ArrayVector vtkRendererGetNearClippingPlaneToleranceFunction(int nargout, const ArrayVector& arg) {
  /* Signature double GetNearClippingPlaneTolerance (); */
  if (arg.size() < 1) 
    throw Exception("Function GetNearClippingPlaneTolerance for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetNearClippingPlaneTolerance();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:ResetCamera1 vtkRendererResetCamera1Function
//input a0 
//output none
ArrayVector vtkRendererResetCamera1Function(int nargout, const ArrayVector& arg) {
  /* Signature void ResetCamera (); */
  if (arg.size() < 1) 
    throw Exception("Function ResetCamera1 for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->ResetCamera();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:ResetCamera2 vtkRendererResetCamera2Function
//input a0 a1 
//output none
ArrayVector vtkRendererResetCamera2Function(int nargout, const ArrayVector& arg) {
  /* Signature void ResetCamera (double bounds[6]); */
  if (arg.size() < 2) 
    throw Exception("Function ResetCamera2 for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double temp0[6];
  if ( !(arg[1].isVector() && arg[1].length() == 6) )
    throw Exception("Mismatch in vector lengths");
  Array vect = arg[1].asDenseArray().toClass(Double);
  BasicArray<double> data = vect.real<double>();
  int length = data.length();
  for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
  vtk_pointer->ResetCamera(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:ResetCamera3 vtkRendererResetCamera3Function
//input a0 a1 a2 a3 a4 a5 a6 
//output none
ArrayVector vtkRendererResetCamera3Function(int nargout, const ArrayVector& arg) {
  /* Signature void ResetCamera (double xmin, double xmax, double ymin, double ymax, double zmin, double zmax); */
  if (arg.size() < 7) 
    throw Exception("Function ResetCamera3 for class vtkRenderer requires 7 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  double   temp4;
  double   temp5;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  if ( !arg[5].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp4 = (double) arg[5].asDouble();
  if ( !arg[6].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp5 = (double) arg[6].asDouble();
  vtk_pointer->ResetCamera(temp0,temp1,temp2,temp3,temp4,temp5);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetRenderWindow vtkRendererSetRenderWindowFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetRenderWindowFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetRenderWindow (vtkRenderWindow *); */
  if (arg.size() < 2) 
    throw Exception("Function SetRenderWindow for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkRenderWindow  *temp0;
  temp0 = GetVTKPointer<vtkRenderWindow>(arg[1]);
  vtk_pointer->SetRenderWindow(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetRenderWindow vtkRendererGetRenderWindowFunction
//input a0 
//output y
ArrayVector vtkRendererGetRenderWindowFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkRenderWindow *GetRenderWindow ();return this RenderWindow  */
  if (arg.size() < 1) 
    throw Exception("Function GetRenderWindow for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkRenderWindow  *temp20;
  temp20 = (vtk_pointer)->GetRenderWindow();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetVTKWindow vtkRendererGetVTKWindowFunction
//input a0 
//output y
ArrayVector vtkRendererGetVTKWindowFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual vtkWindow *GetVTKWindow (); */
  if (arg.size() < 1) 
    throw Exception("Function GetVTKWindow for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkWindow  *temp20;
  temp20 = (vtk_pointer)->GetVTKWindow();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetBackingStore vtkRendererSetBackingStoreFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetBackingStoreFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetBackingStore (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetBackingStore for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetBackingStore(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetBackingStore vtkRendererGetBackingStoreFunction
//input a0 
//output y
ArrayVector vtkRendererGetBackingStoreFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetBackingStore (); */
  if (arg.size() < 1) 
    throw Exception("Function GetBackingStore for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetBackingStore();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:BackingStoreOn vtkRendererBackingStoreOnFunction
//input a0 
//output none
ArrayVector vtkRendererBackingStoreOnFunction(int nargout, const ArrayVector& arg) {
  /* Signature void BackingStoreOn (); */
  if (arg.size() < 1) 
    throw Exception("Function BackingStoreOn for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->BackingStoreOn();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:BackingStoreOff vtkRendererBackingStoreOffFunction
//input a0 
//output none
ArrayVector vtkRendererBackingStoreOffFunction(int nargout, const ArrayVector& arg) {
  /* Signature void BackingStoreOff (); */
  if (arg.size() < 1) 
    throw Exception("Function BackingStoreOff for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->BackingStoreOff();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetInteractive vtkRendererSetInteractiveFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetInteractiveFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetInteractive (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetInteractive for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetInteractive(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetInteractive vtkRendererGetInteractiveFunction
//input a0 
//output y
ArrayVector vtkRendererGetInteractiveFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetInteractive (); */
  if (arg.size() < 1) 
    throw Exception("Function GetInteractive for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetInteractive();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:InteractiveOn vtkRendererInteractiveOnFunction
//input a0 
//output none
ArrayVector vtkRendererInteractiveOnFunction(int nargout, const ArrayVector& arg) {
  /* Signature void InteractiveOn (); */
  if (arg.size() < 1) 
    throw Exception("Function InteractiveOn for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->InteractiveOn();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:InteractiveOff vtkRendererInteractiveOffFunction
//input a0 
//output none
ArrayVector vtkRendererInteractiveOffFunction(int nargout, const ArrayVector& arg) {
  /* Signature void InteractiveOff (); */
  if (arg.size() < 1) 
    throw Exception("Function InteractiveOff for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->InteractiveOff();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetLayer vtkRendererSetLayerFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetLayerFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetLayer (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetLayer for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetLayer(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetLayer vtkRendererGetLayerFunction
//input a0 
//output y
ArrayVector vtkRendererGetLayerFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetLayer (); */
  if (arg.size() < 1) 
    throw Exception("Function GetLayer for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetLayer();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetPreserveDepthBuffer vtkRendererSetPreserveDepthBufferFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetPreserveDepthBufferFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetPreserveDepthBuffer (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetPreserveDepthBuffer for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetPreserveDepthBuffer(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetPreserveDepthBuffer vtkRendererGetPreserveDepthBufferFunction
//input a0 
//output y
ArrayVector vtkRendererGetPreserveDepthBufferFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetPreserveDepthBuffer (); */
  if (arg.size() < 1) 
    throw Exception("Function GetPreserveDepthBuffer for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetPreserveDepthBuffer();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:PreserveDepthBufferOn vtkRendererPreserveDepthBufferOnFunction
//input a0 
//output none
ArrayVector vtkRendererPreserveDepthBufferOnFunction(int nargout, const ArrayVector& arg) {
  /* Signature void PreserveDepthBufferOn (); */
  if (arg.size() < 1) 
    throw Exception("Function PreserveDepthBufferOn for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->PreserveDepthBufferOn();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:PreserveDepthBufferOff vtkRendererPreserveDepthBufferOffFunction
//input a0 
//output none
ArrayVector vtkRendererPreserveDepthBufferOffFunction(int nargout, const ArrayVector& arg) {
  /* Signature void PreserveDepthBufferOff (); */
  if (arg.size() < 1) 
    throw Exception("Function PreserveDepthBufferOff for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->PreserveDepthBufferOff();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:Transparent vtkRendererTransparentFunction
//input a0 
//output y
ArrayVector vtkRendererTransparentFunction(int nargout, const ArrayVector& arg) {
  /* Signature int Transparent (); */
  if (arg.size() < 1) 
    throw Exception("Function Transparent for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->Transparent();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:WorldToView1 vtkRendererWorldToView1Function
//input a0 
//output none
ArrayVector vtkRendererWorldToView1Function(int nargout, const ArrayVector& arg) {
  /* Signature void WorldToView (); */
  if (arg.size() < 1) 
    throw Exception("Function WorldToView1 for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->WorldToView();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:ViewToWorld1 vtkRendererViewToWorld1Function
//input a0 
//output none
ArrayVector vtkRendererViewToWorld1Function(int nargout, const ArrayVector& arg) {
  /* Signature void ViewToWorld (); */
  if (arg.size() < 1) 
    throw Exception("Function ViewToWorld1 for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->ViewToWorld();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetZ vtkRendererGetZFunction
//input a0 a1 a2 
//output y
ArrayVector vtkRendererGetZFunction(int nargout, const ArrayVector& arg) {
  /* Signature double GetZ (int x, int y); */
  if (arg.size() < 3) 
    throw Exception("Function GetZ for class vtkRenderer requires 3 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  int      temp1;
  double   temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (int) arg[2].asDouble();
  temp20 = (vtk_pointer)->GetZ(temp0,temp1);
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetMTime vtkRendererGetMTimeFunction
//input a0 
//output y
ArrayVector vtkRendererGetMTimeFunction(int nargout, const ArrayVector& arg) {
  /* Signature unsigned long GetMTime (); */
  if (arg.size() < 1) 
    throw Exception("Function GetMTime for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  unsigned long     temp20;
  temp20 = (vtk_pointer)->GetMTime();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetLastRenderTimeInSeconds vtkRendererGetLastRenderTimeInSecondsFunction
//input a0 
//output y
ArrayVector vtkRendererGetLastRenderTimeInSecondsFunction(int nargout, const ArrayVector& arg) {
  /* Signature double GetLastRenderTimeInSeconds (); */
  if (arg.size() < 1) 
    throw Exception("Function GetLastRenderTimeInSeconds for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetLastRenderTimeInSeconds();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetNumberOfPropsRendered vtkRendererGetNumberOfPropsRenderedFunction
//input a0 
//output y
ArrayVector vtkRendererGetNumberOfPropsRenderedFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetNumberOfPropsRendered (); */
  if (arg.size() < 1) 
    throw Exception("Function GetNumberOfPropsRendered for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetNumberOfPropsRendered();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:PickProp1 vtkRendererPickProp1Function
//input a0 a1 a2 
//output y
ArrayVector vtkRendererPickProp1Function(int nargout, const ArrayVector& arg) {
  /* Signature vtkAssemblyPath *PickProp (double selectionX, double selectionY);return this PickProp selectionX selectionY selectionX selectionY  */
  if (arg.size() < 3) 
    throw Exception("Function PickProp1 for class vtkRenderer requires 3 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp0;
  double   temp1;
  vtkAssemblyPath  *temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  temp20 = (vtk_pointer)->PickProp(temp0,temp1);
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:PickProp2 vtkRendererPickProp2Function
//input a0 a1 a2 a3 a4 
//output y
ArrayVector vtkRendererPickProp2Function(int nargout, const ArrayVector& arg) {
  /* Signature vtkAssemblyPath *PickProp (double selectionX1, double selectionY1, double selectionX2, double selectionY2); */
  if (arg.size() < 5) 
    throw Exception("Function PickProp2 for class vtkRenderer requires 5 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp0;
  double   temp1;
  double   temp2;
  double   temp3;
  vtkAssemblyPath  *temp20;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  if ( !arg[2].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp1 = (double) arg[2].asDouble();
  if ( !arg[3].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp2 = (double) arg[3].asDouble();
  if ( !arg[4].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp3 = (double) arg[4].asDouble();
  temp20 = (vtk_pointer)->PickProp(temp0,temp1,temp2,temp3);
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:StereoMidpoint vtkRendererStereoMidpointFunction
//input a0 
//output none
ArrayVector vtkRendererStereoMidpointFunction(int nargout, const ArrayVector& arg) {
  /* Signature virtual void StereoMidpoint ();return  */
  if (arg.size() < 1) 
    throw Exception("Function StereoMidpoint for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->StereoMidpoint();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetTiledAspectRatio vtkRendererGetTiledAspectRatioFunction
//input a0 
//output y
ArrayVector vtkRendererGetTiledAspectRatioFunction(int nargout, const ArrayVector& arg) {
  /* Signature double GetTiledAspectRatio (); */
  if (arg.size() < 1) 
    throw Exception("Function GetTiledAspectRatio for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetTiledAspectRatio();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:IsActiveCameraCreated vtkRendererIsActiveCameraCreatedFunction
//input a0 
//output y
ArrayVector vtkRendererIsActiveCameraCreatedFunction(int nargout, const ArrayVector& arg) {
  /* Signature int IsActiveCameraCreated ();return this ActiveCamera NULL  */
  if (arg.size() < 1) 
    throw Exception("Function IsActiveCameraCreated for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->IsActiveCameraCreated();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetUseDepthPeeling vtkRendererSetUseDepthPeelingFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetUseDepthPeelingFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetUseDepthPeeling (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetUseDepthPeeling for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetUseDepthPeeling(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetUseDepthPeeling vtkRendererGetUseDepthPeelingFunction
//input a0 
//output y
ArrayVector vtkRendererGetUseDepthPeelingFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetUseDepthPeeling (); */
  if (arg.size() < 1) 
    throw Exception("Function GetUseDepthPeeling for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetUseDepthPeeling();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:UseDepthPeelingOn vtkRendererUseDepthPeelingOnFunction
//input a0 
//output none
ArrayVector vtkRendererUseDepthPeelingOnFunction(int nargout, const ArrayVector& arg) {
  /* Signature void UseDepthPeelingOn (); */
  if (arg.size() < 1) 
    throw Exception("Function UseDepthPeelingOn for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->UseDepthPeelingOn();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:UseDepthPeelingOff vtkRendererUseDepthPeelingOffFunction
//input a0 
//output none
ArrayVector vtkRendererUseDepthPeelingOffFunction(int nargout, const ArrayVector& arg) {
  /* Signature void UseDepthPeelingOff (); */
  if (arg.size() < 1) 
    throw Exception("Function UseDepthPeelingOff for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtk_pointer->UseDepthPeelingOff();
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetOcclusionRatio vtkRendererSetOcclusionRatioFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetOcclusionRatioFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetOcclusionRatio (double ); */
  if (arg.size() < 2) 
    throw Exception("Function SetOcclusionRatio for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (double) arg[1].asDouble();
  vtk_pointer->SetOcclusionRatio(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetOcclusionRatioMinValue vtkRendererGetOcclusionRatioMinValueFunction
//input a0 
//output y
ArrayVector vtkRendererGetOcclusionRatioMinValueFunction(int nargout, const ArrayVector& arg) {
  /* Signature double GetOcclusionRatioMinValue (); */
  if (arg.size() < 1) 
    throw Exception("Function GetOcclusionRatioMinValue for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetOcclusionRatioMinValue();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetOcclusionRatioMaxValue vtkRendererGetOcclusionRatioMaxValueFunction
//input a0 
//output y
ArrayVector vtkRendererGetOcclusionRatioMaxValueFunction(int nargout, const ArrayVector& arg) {
  /* Signature double GetOcclusionRatioMaxValue (); */
  if (arg.size() < 1) 
    throw Exception("Function GetOcclusionRatioMaxValue for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetOcclusionRatioMaxValue();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetOcclusionRatio vtkRendererGetOcclusionRatioFunction
//input a0 
//output y
ArrayVector vtkRendererGetOcclusionRatioFunction(int nargout, const ArrayVector& arg) {
  /* Signature double GetOcclusionRatio (); */
  if (arg.size() < 1) 
    throw Exception("Function GetOcclusionRatio for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  double   temp20;
  temp20 = (vtk_pointer)->GetOcclusionRatio();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetMaximumNumberOfPeels vtkRendererSetMaximumNumberOfPeelsFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetMaximumNumberOfPeelsFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetMaximumNumberOfPeels (int ); */
  if (arg.size() < 2) 
    throw Exception("Function SetMaximumNumberOfPeels for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp0;
  if ( !arg[1].isScalar() )
    throw Exception("Expecting a scalar argument");
  temp0 = (int) arg[1].asDouble();
  vtk_pointer->SetMaximumNumberOfPeels(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetMaximumNumberOfPeels vtkRendererGetMaximumNumberOfPeelsFunction
//input a0 
//output y
ArrayVector vtkRendererGetMaximumNumberOfPeelsFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetMaximumNumberOfPeels (); */
  if (arg.size() < 1) 
    throw Exception("Function GetMaximumNumberOfPeels for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetMaximumNumberOfPeels();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetLastRenderingUsedDepthPeeling vtkRendererGetLastRenderingUsedDepthPeelingFunction
//input a0 
//output y
ArrayVector vtkRendererGetLastRenderingUsedDepthPeelingFunction(int nargout, const ArrayVector& arg) {
  /* Signature int GetLastRenderingUsedDepthPeeling (); */
  if (arg.size() < 1) 
    throw Exception("Function GetLastRenderingUsedDepthPeeling for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  int      temp20;
  temp20 = (vtk_pointer)->GetLastRenderingUsedDepthPeeling();
  retval = Array(double(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetDelegate vtkRendererSetDelegateFunction
//input a0 a1 
//output none
ArrayVector vtkRendererSetDelegateFunction(int nargout, const ArrayVector& arg) {
  /* Signature void SetDelegate (vtkRendererDelegate *d); */
  if (arg.size() < 2) 
    throw Exception("Function SetDelegate for class vtkRenderer requires 2 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkRendererDelegate  *temp0;
  temp0 = GetVTKPointer<vtkRendererDelegate>(arg[1]);
  vtk_pointer->SetDelegate(temp0);
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetDelegate vtkRendererGetDelegateFunction
//input a0 
//output y
ArrayVector vtkRendererGetDelegateFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkRendererDelegate *GetDelegate (); */
  if (arg.size() < 1) 
    throw Exception("Function GetDelegate for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkRendererDelegate  *temp20;
  temp20 = (vtk_pointer)->GetDelegate();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:GetSelector vtkRendererGetSelectorFunction
//input a0 
//output y
ArrayVector vtkRendererGetSelectorFunction(int nargout, const ArrayVector& arg) {
  /* Signature vtkHardwareSelector *GetSelector (); */
  if (arg.size() < 1) 
    throw Exception("Function GetSelector for class vtkRenderer requires 1 argument(s)");
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  Array retval;
  vtkHardwareSelector  *temp20;
  temp20 = (vtk_pointer)->GetSelector();
  retval = MakeVTKPointer((vtkObjectBase*)(temp20));
  return retval;
}

//@@Signature
//gfunction @vtkRenderer:SetAmbient vtkRendererSetAmbientFunction
//input varargin
//output varargout
ArrayVector vtkRendererSetAmbientFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 4) return vtkRendererSetAmbient1Function(nargout,arg);
  if (arg.size() == 2) return vtkRendererSetAmbient2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkRendererSetAmbient");
}

//@@Signature
//gfunction @vtkRenderer:ComputeVisiblePropBounds vtkRendererComputeVisiblePropBoundsFunction
//input varargin
//output varargout
ArrayVector vtkRendererComputeVisiblePropBoundsFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 2) return vtkRendererComputeVisiblePropBounds1Function(nargout,arg);
  if (arg.size() == 1) return vtkRendererComputeVisiblePropBounds2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkRendererComputeVisiblePropBounds");
}

//@@Signature
//gfunction @vtkRenderer:ResetCameraClippingRange vtkRendererResetCameraClippingRangeFunction
//input varargin
//output varargout
ArrayVector vtkRendererResetCameraClippingRangeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 1) return vtkRendererResetCameraClippingRange1Function(nargout,arg);
  if (arg.size() == 2) return vtkRendererResetCameraClippingRange2Function(nargout,arg);
  if (arg.size() == 7) return vtkRendererResetCameraClippingRange3Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkRendererResetCameraClippingRange");
}

//@@Signature
//gfunction @vtkRenderer:ResetCamera vtkRendererResetCameraFunction
//input varargin
//output varargout
ArrayVector vtkRendererResetCameraFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 1) return vtkRendererResetCamera1Function(nargout,arg);
  if (arg.size() == 2) return vtkRendererResetCamera2Function(nargout,arg);
  if (arg.size() == 7) return vtkRendererResetCamera3Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkRendererResetCamera");
}

//@@Signature
//gfunction @vtkRenderer:WorldToView vtkRendererWorldToViewFunction
//input varargin
//output varargout
ArrayVector vtkRendererWorldToViewFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 1) return vtkRendererWorldToView1Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkRendererWorldToView");
}

//@@Signature
//gfunction @vtkRenderer:ViewToWorld vtkRendererViewToWorldFunction
//input varargin
//output varargout
ArrayVector vtkRendererViewToWorldFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 1) return vtkRendererViewToWorld1Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkRendererViewToWorld");
}

//@@Signature
//gfunction @vtkRenderer:PickProp vtkRendererPickPropFunction
//input varargin
//output varargout
ArrayVector vtkRendererPickPropFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 3) return vtkRendererPickProp1Function(nargout,arg);
  if (arg.size() == 5) return vtkRendererPickProp2Function(nargout,arg);
  throw Exception("unable to resolve to an overloaded instance of vtkRendererPickProp");
}

//@@Signature
//sgfunction @vtkRenderer:display vtkRendererDisplayFunction
//input varargin
//output varargout
ArrayVector vtkRendererDisplayFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() == 0) return ArrayVector();
  if (arg[0].length() > 1) {
     PrintArrayClassic(arg[0],100,eval);
     return ArrayVector();
  }
  eval->outputMessage("  " + arg[0].className() + "\n");
  eval->outputMessage("  ClassName : ");
  {
    ArrayVector tmp = vtkRendererGetClassNameFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Lights : ");
  {
    ArrayVector tmp = vtkRendererGetLightsFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  TwoSidedLighting : ");
  {
    ArrayVector tmp = vtkRendererGetTwoSidedLightingFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  LightFollowCamera : ");
  {
    ArrayVector tmp = vtkRendererGetLightFollowCameraFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  AutomaticLightCreation : ");
  {
    ArrayVector tmp = vtkRendererGetAutomaticLightCreationFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Volumes : ");
  {
    ArrayVector tmp = vtkRendererGetVolumesFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Actors : ");
  {
    ArrayVector tmp = vtkRendererGetActorsFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  ActiveCamera : ");
  {
    ArrayVector tmp = vtkRendererGetActiveCameraFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Erase : ");
  {
    ArrayVector tmp = vtkRendererGetEraseFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Draw : ");
  {
    ArrayVector tmp = vtkRendererGetDrawFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Cullers : ");
  {
    ArrayVector tmp = vtkRendererGetCullersFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Ambient : ");
  {
    ArrayVector tmp = vtkRendererGetAmbientFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  AllocatedRenderTime : ");
  {
    ArrayVector tmp = vtkRendererGetAllocatedRenderTimeFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  TimeFactor : ");
  {
    ArrayVector tmp = vtkRendererGetTimeFactorFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  NearClippingPlaneToleranceMinValue : ");
  {
    ArrayVector tmp = vtkRendererGetNearClippingPlaneToleranceMinValueFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  NearClippingPlaneToleranceMaxValue : ");
  {
    ArrayVector tmp = vtkRendererGetNearClippingPlaneToleranceMaxValueFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  NearClippingPlaneTolerance : ");
  {
    ArrayVector tmp = vtkRendererGetNearClippingPlaneToleranceFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  RenderWindow : ");
  {
    ArrayVector tmp = vtkRendererGetRenderWindowFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  VTKWindow : ");
  {
    ArrayVector tmp = vtkRendererGetVTKWindowFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  BackingStore : ");
  {
    ArrayVector tmp = vtkRendererGetBackingStoreFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Interactive : ");
  {
    ArrayVector tmp = vtkRendererGetInteractiveFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Layer : ");
  {
    ArrayVector tmp = vtkRendererGetLayerFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  PreserveDepthBuffer : ");
  {
    ArrayVector tmp = vtkRendererGetPreserveDepthBufferFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  MTime : ");
  {
    ArrayVector tmp = vtkRendererGetMTimeFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  LastRenderTimeInSeconds : ");
  {
    ArrayVector tmp = vtkRendererGetLastRenderTimeInSecondsFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  NumberOfPropsRendered : ");
  {
    ArrayVector tmp = vtkRendererGetNumberOfPropsRenderedFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  TiledAspectRatio : ");
  {
    ArrayVector tmp = vtkRendererGetTiledAspectRatioFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  UseDepthPeeling : ");
  {
    ArrayVector tmp = vtkRendererGetUseDepthPeelingFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  OcclusionRatioMinValue : ");
  {
    ArrayVector tmp = vtkRendererGetOcclusionRatioMinValueFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  OcclusionRatioMaxValue : ");
  {
    ArrayVector tmp = vtkRendererGetOcclusionRatioMaxValueFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  OcclusionRatio : ");
  {
    ArrayVector tmp = vtkRendererGetOcclusionRatioFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  MaximumNumberOfPeels : ");
  {
    ArrayVector tmp = vtkRendererGetMaximumNumberOfPeelsFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  LastRenderingUsedDepthPeeling : ");
  {
    ArrayVector tmp = vtkRendererGetLastRenderingUsedDepthPeelingFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Delegate : ");
  {
    ArrayVector tmp = vtkRendererGetDelegateFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  eval->outputMessage("  Selector : ");
  {
    ArrayVector tmp = vtkRendererGetSelectorFunction(1,arg[0]);
    if (tmp.size() > 0)
      eval->outputMessage(SummarizeArrayCellEntry(tmp[0]));
    else
      eval->outputMessage("[]");
  }
  eval->outputMessage("\n");
  return ArrayVector();
}
//@@Signature
//gfunction @vtkRenderer:subsref vtkRendererSubsrefFunction
//input varargin
//output varargout
ArrayVector vtkRendererSubsrefFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2) return ArrayVector();
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  const StructArray& sa = arg[1].constStructPtr();
  const BasicArray<Array>& typea = sa["type"];
  const BasicArray<Array>& subsa = sa["subs"];
  if ((typea.length() == 1) && (typea[1].asString() == "."))
  {
    if (subsa[1].asString() == "ClassName")
      return vtkRendererGetClassNameFunction(nargout,arg);
    if (subsa[1].asString() == "Lights")
      return vtkRendererGetLightsFunction(nargout,arg);
    if (subsa[1].asString() == "TwoSidedLighting")
      return vtkRendererGetTwoSidedLightingFunction(nargout,arg);
    if (subsa[1].asString() == "LightFollowCamera")
      return vtkRendererGetLightFollowCameraFunction(nargout,arg);
    if (subsa[1].asString() == "AutomaticLightCreation")
      return vtkRendererGetAutomaticLightCreationFunction(nargout,arg);
    if (subsa[1].asString() == "Volumes")
      return vtkRendererGetVolumesFunction(nargout,arg);
    if (subsa[1].asString() == "Actors")
      return vtkRendererGetActorsFunction(nargout,arg);
    if (subsa[1].asString() == "ActiveCamera")
      return vtkRendererGetActiveCameraFunction(nargout,arg);
    if (subsa[1].asString() == "Erase")
      return vtkRendererGetEraseFunction(nargout,arg);
    if (subsa[1].asString() == "Draw")
      return vtkRendererGetDrawFunction(nargout,arg);
    if (subsa[1].asString() == "Cullers")
      return vtkRendererGetCullersFunction(nargout,arg);
    if (subsa[1].asString() == "Ambient")
      return vtkRendererGetAmbientFunction(nargout,arg);
    if (subsa[1].asString() == "AllocatedRenderTime")
      return vtkRendererGetAllocatedRenderTimeFunction(nargout,arg);
    if (subsa[1].asString() == "TimeFactor")
      return vtkRendererGetTimeFactorFunction(nargout,arg);
    if (subsa[1].asString() == "NearClippingPlaneToleranceMinValue")
      return vtkRendererGetNearClippingPlaneToleranceMinValueFunction(nargout,arg);
    if (subsa[1].asString() == "NearClippingPlaneToleranceMaxValue")
      return vtkRendererGetNearClippingPlaneToleranceMaxValueFunction(nargout,arg);
    if (subsa[1].asString() == "NearClippingPlaneTolerance")
      return vtkRendererGetNearClippingPlaneToleranceFunction(nargout,arg);
    if (subsa[1].asString() == "RenderWindow")
      return vtkRendererGetRenderWindowFunction(nargout,arg);
    if (subsa[1].asString() == "VTKWindow")
      return vtkRendererGetVTKWindowFunction(nargout,arg);
    if (subsa[1].asString() == "BackingStore")
      return vtkRendererGetBackingStoreFunction(nargout,arg);
    if (subsa[1].asString() == "Interactive")
      return vtkRendererGetInteractiveFunction(nargout,arg);
    if (subsa[1].asString() == "Layer")
      return vtkRendererGetLayerFunction(nargout,arg);
    if (subsa[1].asString() == "PreserveDepthBuffer")
      return vtkRendererGetPreserveDepthBufferFunction(nargout,arg);
    if (subsa[1].asString() == "MTime")
      return vtkRendererGetMTimeFunction(nargout,arg);
    if (subsa[1].asString() == "LastRenderTimeInSeconds")
      return vtkRendererGetLastRenderTimeInSecondsFunction(nargout,arg);
    if (subsa[1].asString() == "NumberOfPropsRendered")
      return vtkRendererGetNumberOfPropsRenderedFunction(nargout,arg);
    if (subsa[1].asString() == "TiledAspectRatio")
      return vtkRendererGetTiledAspectRatioFunction(nargout,arg);
    if (subsa[1].asString() == "UseDepthPeeling")
      return vtkRendererGetUseDepthPeelingFunction(nargout,arg);
    if (subsa[1].asString() == "OcclusionRatioMinValue")
      return vtkRendererGetOcclusionRatioMinValueFunction(nargout,arg);
    if (subsa[1].asString() == "OcclusionRatioMaxValue")
      return vtkRendererGetOcclusionRatioMaxValueFunction(nargout,arg);
    if (subsa[1].asString() == "OcclusionRatio")
      return vtkRendererGetOcclusionRatioFunction(nargout,arg);
    if (subsa[1].asString() == "MaximumNumberOfPeels")
      return vtkRendererGetMaximumNumberOfPeelsFunction(nargout,arg);
    if (subsa[1].asString() == "LastRenderingUsedDepthPeeling")
      return vtkRendererGetLastRenderingUsedDepthPeelingFunction(nargout,arg);
    if (subsa[1].asString() == "Delegate")
      return vtkRendererGetDelegateFunction(nargout,arg);
    if (subsa[1].asString() == "Selector")
      return vtkRendererGetSelectorFunction(nargout,arg);
  }
  if ((typea.length() == 2) && (typea[1].asString() == ".")
      && (typea[2].asString() == "()"))
  {
    if (subsa[1].asString() == "Z")
    {
      ArrayVector tmp(arg[0]);
      tmp += ArrayVectorFromCellArray(subsa[2]);
      return vtkRendererGetZFunction(nargout,tmp);
    }
  }
}

//@@Signature
//sgfunction @vtkRenderer:subsasgn //input varargin
//output varargin
ArrayVector vtkRendererSubsasgnFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3) return ArrayVector();
  vtkRenderer* vtk_pointer = GetVTKPointer<vtkRenderer>(arg[0]);
  const StructArray& sa = arg[1].constStructPtr();
  const BasicArray<Array>& typea = sa["type"];
  const BasicArray<Array>& subsa = sa["subs"];
  if ((typea.length() == 1) && (typea[1].asString() == "."))
  {
    if (subsa[1].asString() == "TwoSidedLighting")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetTwoSidedLightingFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "LightFollowCamera")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetLightFollowCameraFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "AutomaticLightCreation")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetAutomaticLightCreationFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "ActiveCamera")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetActiveCameraFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Erase")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetEraseFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Draw")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetDrawFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Ambient")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetAmbientFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Ambient")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetAmbientFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "AllocatedRenderTime")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetAllocatedRenderTimeFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "NearClippingPlaneTolerance")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetNearClippingPlaneToleranceFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "RenderWindow")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetRenderWindowFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "BackingStore")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetBackingStoreFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Interactive")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetInteractiveFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Layer")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetLayerFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "PreserveDepthBuffer")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetPreserveDepthBufferFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "UseDepthPeeling")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetUseDepthPeelingFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "OcclusionRatio")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetOcclusionRatioFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "MaximumNumberOfPeels")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetMaximumNumberOfPeelsFunction(nargout,tmp);
    }
    if (subsa[1].asString() == "Delegate")
    {
      ArrayVector tmp(arg[0]);
      tmp.push_back(arg[2]);
      return vtkRendererSetDelegateFunction(nargout,tmp);
    }
  }
  return ArrayVector();
}
