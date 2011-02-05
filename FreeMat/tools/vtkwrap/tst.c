// Octave wrapper for vtkViewport object
//
#include "OctavizCommon.h"
#define VTK_STREAMS_FWD_ONLY
#include "vtkViewport.h"

void populate_help_mapvtkViewport( std::map<std::string,std::string> &help_map );


DEFUN_DLD ( vtkViewport, args, nargout, 
  "-*- texinfo -*-\n\
@deftypefn {Built-in Function} {} vtkViewport\n\
\n\
 vtkViewport provides an abstract specification for Viewports. A Viewport\n\
is an object that controls the rendering process for objects. Rendering\n\
is the process of converting geometry, a specification for lights, and\n\
a camera view into an image. vtkViewport also performs coordinate\n\
transformation between world coordinates, view coordinates (the computer\n\
graphics rendering coordinate system), and display coordinates (the\n\
actual screen coordinates on the display device). Certain advanced\n\
rendering features such as two-sided lighting can also be controlled.\n\
@end deftypefn")
{
  octave_value retval;
  static std::map<std::string,std::string> help_map;
  static bool help_populated = false;
  if ( !help_populated ) 
    {
    populate_help_mapvtkViewport( help_map );
    help_populated = true;
    }

  int nargin  = args.length ();

  if ( nargin < 1 )
    {
    vtkObjectBase *new_vtk_object = vtkViewport::New();
    retval = octave_value( new vtk_object( new_vtk_object, true ) );
    return retval;
    }

  if ( nargin == 1 )
    {
    // This can only be the "New" command
    if ( !args(0).is_string() )
      {
      error("If a single parameter is passed, then it has to be \"New\" or \"List\".");
      return retval;
      }
    if ( args(0).string_value() == "New" )
      {
      vtkObjectBase *new_vtk_object = vtkViewport::New();
      retval = octave_value( new vtk_object( new_vtk_object, true ) );
      return retval;
      }
    if ( args(0).string_value() == "List" )
      {
      octave_stdout << help_map["List"] << std::endl;
      return retval;
      }
    error("If a single parameter is passed, then it has to be \"New\" or \"List\".");
    return retval;
    }
  // The second parameter has to be a string command
  if ( !args(1).is_string() )
    {
    error("Second parameter has to be a string command.");
    return retval;
	  }


  // If it is not the command New, the first parameter has to be a vtk_object
  // or a method name followed by a string Help
  if ( ( args(0).type_id() != vtk_object::static_type_id() ) &&
       ( args(1).string_value() != "Help" ) )
    {
    error("First parameter has to be a vtk_object or the command \"New\". If the first parameter is a method name, then the second has to be Help");
    return retval;
    }

    if ( args(1).string_value() == "Help" )
    {
    octave_stdout << help_map[args(0).string_value()] << std::endl;
    return retval;
    }

  vtkViewport *vtk_pointer = reinterpret_cast<vtkViewport*>( args(0).uint64_scalar_value().value() );
  std::string method_name = args(1).string_value();
  int    error_flag;
  error_flag = 0; error_flag = error_flag;

  if ( ( method_name == "GetClassName" ) && ( nargin == 2 ) )
    {

/* ah const char *GetClassName (); */
    const char    *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetClassName();
      retval = Array(QString(temp20));
      return retval;
      }
    }
  if ( ( method_name == "IsA" ) && ( nargin == 3 ) )
    {

/* ah int IsA (const char *name); */
    char    *temp0;
    int      temp20;
    error_flag = 0;

    char tmp_string0[1024];
    strcpy(tmp_string0,qPrintable(args[%1].asString())); 
    temp0 = tmp_string0;
    if (!error_flag)
      {
      temp20 = (vtk_pointer)->IsA(temp0);
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "NewInstance" ) && ( nargin == 2 ) )
    {

/* ah vtkViewport *NewInstance (); */
    vtkViewport  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->NewInstance();
      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp20);
      retval = octave_value( new vtk_object(tmp_ptr) );
      return retval;
      }
    }
  if ( ( method_name == "SafeDownCast" ) && ( nargin == 3 ) )
    {

/* ah vtkViewport *SafeDownCast (vtkObject* o); */
    vtkObject  *temp0;
    vtkViewport  *temp20;
    error_flag = 0;

    if ( args(2).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp0 = reinterpret_cast<vtkObject*>( args(2).uint64_scalar_value().value() );
    if (!error_flag)
      {
      temp20 = (vtk_pointer)->SafeDownCast(temp0);
      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp20);
      retval = octave_value( new vtk_object(tmp_ptr) );
      return retval;
      }
    }
  if ( ( method_name == "AddViewProp" ) && ( nargin == 3 ) )
    {

/* ah void AddViewProp (vtkProp *); */
    vtkProp  *temp0;
    error_flag = 0;

    if ( args(2).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp0 = reinterpret_cast<vtkProp*>( args(2).uint64_scalar_value().value() );
    if (!error_flag)
      {
      vtk_pointer->AddViewProp(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetViewProps" ) && ( nargin == 2 ) )
    {

/* ah vtkPropCollection *GetViewProps ();return this Props  */
    vtkPropCollection  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetViewProps();
      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp20);
      retval = octave_value( new vtk_object(tmp_ptr) );
      return retval;
      }
    }
  if ( ( method_name == "HasViewProp" ) && ( nargin == 3 ) )
    {

/* ah int HasViewProp (vtkProp *); */
    vtkProp  *temp0;
    int      temp20;
    error_flag = 0;

    if ( args(2).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp0 = reinterpret_cast<vtkProp*>( args(2).uint64_scalar_value().value() );
    if (!error_flag)
      {
      temp20 = (vtk_pointer)->HasViewProp(temp0);
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "RemoveViewProp" ) && ( nargin == 3 ) )
    {

/* ah void RemoveViewProp (vtkProp *); */
    vtkProp  *temp0;
    error_flag = 0;

    if ( args(2).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp0 = reinterpret_cast<vtkProp*>( args(2).uint64_scalar_value().value() );
    if (!error_flag)
      {
      vtk_pointer->RemoveViewProp(temp0);
      return retval;
      }
    }
  if ( ( method_name == "RemoveAllViewProps" ) && ( nargin == 2 ) )
    {

/* ah void RemoveAllViewProps (void ); */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->RemoveAllViewProps();
      return retval;
      }
    }
  if ( ( method_name == "AddActor2D" ) && ( nargin == 3 ) )
    {

/* ah void AddActor2D (vtkProp *p); */
    vtkProp  *temp0;
    error_flag = 0;

    if ( args(2).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp0 = reinterpret_cast<vtkProp*>( args(2).uint64_scalar_value().value() );
    if (!error_flag)
      {
      vtk_pointer->AddActor2D(temp0);
      return retval;
      }
    }
  if ( ( method_name == "RemoveActor2D" ) && ( nargin == 3 ) )
    {

/* ah void RemoveActor2D (vtkProp *p); */
    vtkProp  *temp0;
    error_flag = 0;

    if ( args(2).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp0 = reinterpret_cast<vtkProp*>( args(2).uint64_scalar_value().value() );
    if (!error_flag)
      {
      vtk_pointer->RemoveActor2D(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetActors2D" ) && ( nargin == 2 ) )
    {

/* ah vtkActor2DCollection *GetActors2D (); */
    vtkActor2DCollection  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetActors2D();
      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp20);
      retval = octave_value( new vtk_object(tmp_ptr) );
      return retval;
      }
    }
  if ( ( method_name == "SetBackground" ) && ( nargin == 5 ) )
    {

/* ah void SetBackground (double , double , double ); */
    double   temp0;
    double   temp1;
    double   temp2;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if ( !args[4].isScalar() ) error_flag = 1;
    else temp2 = (double) args[4].asDouble();
    if (!error_flag)
      {
      vtk_pointer->SetBackground(temp0,temp1,temp2);
      return retval;
      }
    }
  if ( ( method_name == "SetBackground" ) && ( nargin == 3 ) )
    {

/* ah void SetBackground (double  a[3]); */
    double temp0[3];
    error_flag = 0;

    if ( !(args[2].isVector() && args[2].length() == 3) ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->SetBackground(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetBackground" ) && ( nargin == 2 ) )
    {

/* ah double  *GetBackground (); */
    double  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetBackground();
      BasicArray<double> tempResult(NTuple(3,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      tempResult[3] = (double)temp20[2];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "SetBackground2" ) && ( nargin == 5 ) )
    {

/* ah void SetBackground2 (double , double , double ); */
    double   temp0;
    double   temp1;
    double   temp2;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if ( !args[4].isScalar() ) error_flag = 1;
    else temp2 = (double) args[4].asDouble();
    if (!error_flag)
      {
      vtk_pointer->SetBackground2(temp0,temp1,temp2);
      return retval;
      }
    }
  if ( ( method_name == "SetBackground2" ) && ( nargin == 3 ) )
    {

/* ah void SetBackground2 (double  a[3]); */
    double temp0[3];
    error_flag = 0;

    if ( !(args[2].isVector() && args[2].length() == 3) ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->SetBackground2(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetBackground2" ) && ( nargin == 2 ) )
    {

/* ah double  *GetBackground2 (); */
    double  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetBackground2();
      BasicArray<double> tempResult(NTuple(3,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      tempResult[3] = (double)temp20[2];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "GradientBackgroundOn" ) && ( nargin == 2 ) )
    {

/* ah void GradientBackgroundOn (); */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->GradientBackgroundOn();
      return retval;
      }
    }
  if ( ( method_name == "GradientBackgroundOff" ) && ( nargin == 2 ) )
    {

/* ah void GradientBackgroundOff (); */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->GradientBackgroundOff();
      return retval;
      }
    }
  if ( ( method_name == "SetAspect" ) && ( nargin == 4 ) )
    {

/* ah void SetAspect (double , double ); */
    double   temp0;
    double   temp1;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if (!error_flag)
      {
      vtk_pointer->SetAspect(temp0,temp1);
      return retval;
      }
    }
  if ( ( method_name == "SetAspect" ) && ( nargin == 3 ) )
    {

/* ah void SetAspect (double  a[2]); */
    double temp0[2];
    error_flag = 0;

    if ( !(args[2].isVector() && args[2].length() == 2) ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->SetAspect(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetAspect" ) && ( nargin == 2 ) )
    {

/* ah double  *GetAspect (); */
    double  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetAspect();
      BasicArray<double> tempResult(NTuple(2,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "ComputeAspect" ) && ( nargin == 2 ) )
    {

/* ah virtual void ComputeAspect (); */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->ComputeAspect();
      return retval;
      }
    }
  if ( ( method_name == "SetPixelAspect" ) && ( nargin == 4 ) )
    {

/* ah void SetPixelAspect (double , double ); */
    double   temp0;
    double   temp1;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if (!error_flag)
      {
      vtk_pointer->SetPixelAspect(temp0,temp1);
      return retval;
      }
    }
  if ( ( method_name == "SetPixelAspect" ) && ( nargin == 3 ) )
    {

/* ah void SetPixelAspect (double  a[2]); */
    double temp0[2];
    error_flag = 0;

    if ( !(args[2].isVector() && args[2].length() == 2) ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->SetPixelAspect(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetPixelAspect" ) && ( nargin == 2 ) )
    {

/* ah double  *GetPixelAspect (); */
    double  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPixelAspect();
      BasicArray<double> tempResult(NTuple(2,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "SetViewport" ) && ( nargin == 6 ) )
    {

/* ah void SetViewport (double , double , double , double ); */
    double   temp0;
    double   temp1;
    double   temp2;
    double   temp3;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if ( !args[4].isScalar() ) error_flag = 1;
    else temp2 = (double) args[4].asDouble();
    if ( !args[5].isScalar() ) error_flag = 1;
    else temp3 = (double) args[5].asDouble();
    if (!error_flag)
      {
      vtk_pointer->SetViewport(temp0,temp1,temp2,temp3);
      return retval;
      }
    }
  if ( ( method_name == "SetViewport" ) && ( nargin == 3 ) )
    {

/* ah void SetViewport (double  a[4]); */
    double temp0[4];
    error_flag = 0;

    if ( !(args[2].isVector() && args[2].length() == 4) ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->SetViewport(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetViewport" ) && ( nargin == 2 ) )
    {

/* ah double  *GetViewport (); */
    double  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetViewport();
      BasicArray<double> tempResult(NTuple(4,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      tempResult[3] = (double)temp20[2];
      tempResult[4] = (double)temp20[3];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "SetDisplayPoint" ) && ( nargin == 5 ) )
    {

/* ah void SetDisplayPoint (double , double , double ); */
    double   temp0;
    double   temp1;
    double   temp2;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if ( !args[4].isScalar() ) error_flag = 1;
    else temp2 = (double) args[4].asDouble();
    if (!error_flag)
      {
      vtk_pointer->SetDisplayPoint(temp0,temp1,temp2);
      return retval;
      }
    }
  if ( ( method_name == "SetDisplayPoint" ) && ( nargin == 3 ) )
    {

/* ah void SetDisplayPoint (double  a[3]); */
    double temp0[3];
    error_flag = 0;

    if ( !(args[2].isVector() && args[2].length() == 3) ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->SetDisplayPoint(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetDisplayPoint" ) && ( nargin == 2 ) )
    {

/* ah double  *GetDisplayPoint (); */
    double  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetDisplayPoint();
      BasicArray<double> tempResult(NTuple(3,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      tempResult[3] = (double)temp20[2];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "SetViewPoint" ) && ( nargin == 5 ) )
    {

/* ah void SetViewPoint (double , double , double ); */
    double   temp0;
    double   temp1;
    double   temp2;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if ( !args[4].isScalar() ) error_flag = 1;
    else temp2 = (double) args[4].asDouble();
    if (!error_flag)
      {
      vtk_pointer->SetViewPoint(temp0,temp1,temp2);
      return retval;
      }
    }
  if ( ( method_name == "SetViewPoint" ) && ( nargin == 3 ) )
    {

/* ah void SetViewPoint (double  a[3]); */
    double temp0[3];
    error_flag = 0;

    if ( !(args[2].isVector() && args[2].length() == 3) ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->SetViewPoint(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetViewPoint" ) && ( nargin == 2 ) )
    {

/* ah double  *GetViewPoint (); */
    double  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetViewPoint();
      BasicArray<double> tempResult(NTuple(3,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      tempResult[3] = (double)temp20[2];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "SetWorldPoint" ) && ( nargin == 6 ) )
    {

/* ah void SetWorldPoint (double , double , double , double ); */
    double   temp0;
    double   temp1;
    double   temp2;
    double   temp3;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if ( !args[4].isScalar() ) error_flag = 1;
    else temp2 = (double) args[4].asDouble();
    if ( !args[5].isScalar() ) error_flag = 1;
    else temp3 = (double) args[5].asDouble();
    if (!error_flag)
      {
      vtk_pointer->SetWorldPoint(temp0,temp1,temp2,temp3);
      return retval;
      }
    }
  if ( ( method_name == "SetWorldPoint" ) && ( nargin == 3 ) )
    {

/* ah void SetWorldPoint (double  a[4]); */
    double temp0[4];
    error_flag = 0;

    if ( !(args[2].isVector() && args[2].length() == 4) ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      for ( int k = 0; k < length; k++ ) temp0[k] = (double)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->SetWorldPoint(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetWorldPoint" ) && ( nargin == 2 ) )
    {

/* ah double  *GetWorldPoint (); */
    double  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetWorldPoint();
      BasicArray<double> tempResult(NTuple(4,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      tempResult[3] = (double)temp20[2];
      tempResult[4] = (double)temp20[3];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "GetCenter" ) && ( nargin == 2 ) )
    {

/* ah virtual double *GetCenter (); */
    double  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetCenter();
      BasicArray<double> tempResult(NTuple(2,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "IsInViewport" ) && ( nargin == 4 ) )
    {

/* ah virtual int IsInViewport (int x, int y); */
    int      temp0;
    int      temp1;
    int      temp20;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (int) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (int) args[3].asDouble();
    if (!error_flag)
      {
      temp20 = (vtk_pointer)->IsInViewport(temp0,temp1);
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetVTKWindow" ) && ( nargin == 2 ) )
    {

/* ah virtual vtkWindow *GetVTKWindow () = 0; */
    vtkWindow  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetVTKWindow();
      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp20);
      retval = octave_value( new vtk_object(tmp_ptr) );
      return retval;
      }
    }
  if ( ( method_name == "DisplayToView" ) && ( nargin == 2 ) )
    {

/* ah virtual void DisplayToView (); */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->DisplayToView();
      return retval;
      }
    }
  if ( ( method_name == "ViewToDisplay" ) && ( nargin == 2 ) )
    {

/* ah virtual void ViewToDisplay (); */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->ViewToDisplay();
      return retval;
      }
    }
  if ( ( method_name == "WorldToView" ) && ( nargin == 2 ) )
    {

/* ah virtual void WorldToView (); */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->WorldToView();
      return retval;
      }
    }
  if ( ( method_name == "ViewToWorld" ) && ( nargin == 2 ) )
    {

/* ah virtual void ViewToWorld (); */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->ViewToWorld();
      return retval;
      }
    }
  if ( ( method_name == "DisplayToWorld" ) && ( nargin == 2 ) )
    {

/* ah void DisplayToWorld ();this DisplayToView this ViewToWorld  */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->DisplayToWorld();
      return retval;
      }
    }
  if ( ( method_name == "WorldToDisplay" ) && ( nargin == 2 ) )
    {

/* ah void WorldToDisplay ();this WorldToView this ViewToDisplay  */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->WorldToDisplay();
      return retval;
      }
    }
  if ( ( method_name == "GetSize" ) && ( nargin == 2 ) )
    {

/* ah virtual int *GetSize (); */
    int     *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetSize();
      BasicArray<double> tempResult(NTuple(2,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "GetOrigin" ) && ( nargin == 2 ) )
    {

/* ah virtual int *GetOrigin (); */
    int     *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetOrigin();
      BasicArray<double> tempResult(NTuple(2,1));
      tempResult[1] = (double)temp20[0];
      tempResult[2] = (double)temp20[1];
      retval = Array(tempResult);
      return retval;
      }
    }
  if ( ( method_name == "GetTiledSize" ) && ( nargin == 4 ) )
    {

/* ah void GetTiledSize (int *width, int *height); */
    int     *temp0;
    int     *temp1;
    error_flag = 0;

    if ( !args[2].isVector() ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp0 = new int[length];
      for ( int k = 0; k < length; k++ ) temp0[k] = (int)data[k+1];
      }
    if ( !args[3].isVector() ) error_flag = 1;
    else 
      {
      Array vect = args[3].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp1 = new int[length];
      for ( int k = 0; k < length; k++ ) temp1[k] = (int)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->GetTiledSize(temp0,temp1);
      return retval;
      }
    }
  if ( ( method_name == "GetTiledSizeAndOrigin" ) && ( nargin == 6 ) )
    {

/* ah virtual void GetTiledSizeAndOrigin (int *width, int *height, int *lowerLeftX, int *lowerLeftY); */
    int     *temp0;
    int     *temp1;
    int     *temp2;
    int     *temp3;
    error_flag = 0;

    if ( !args[2].isVector() ) error_flag = 1;
    else 
      {
      Array vect = args[2].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp0 = new int[length];
      for ( int k = 0; k < length; k++ ) temp0[k] = (int)data[k+1];
      }
    if ( !args[3].isVector() ) error_flag = 1;
    else 
      {
      Array vect = args[3].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp1 = new int[length];
      for ( int k = 0; k < length; k++ ) temp1[k] = (int)data[k+1];
      }
    if ( !args[4].isVector() ) error_flag = 1;
    else 
      {
      Array vect = args[4].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp2 = new int[length];
      for ( int k = 0; k < length; k++ ) temp2[k] = (int)data[k+1];
      }
    if ( !args[5].isVector() ) error_flag = 1;
    else 
      {
      Array vect = args[5].asDenseArray().toClass(Double);
      BasicArray<double> data = vect.real<double>();
      int length = data.length();
      temp3 = new int[length];
      for ( int k = 0; k < length; k++ ) temp3[k] = (int)data[k+1];
      }
    if (!error_flag)
      {
      vtk_pointer->GetTiledSizeAndOrigin(temp0,temp1,temp2,temp3);
      return retval;
      }
    }
  if ( ( method_name == "PickProp" ) && ( nargin == 4 ) )
    {

/* ah virtual vtkAssemblyPath *PickProp (double selectionX, double selectionY) = 0; */
    double   temp0;
    double   temp1;
    vtkAssemblyPath  *temp20;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if (!error_flag)
      {
      temp20 = (vtk_pointer)->PickProp(temp0,temp1);
      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp20);
      retval = octave_value( new vtk_object(tmp_ptr) );
      return retval;
      }
    }
  if ( ( method_name == "PickPropFrom" ) && ( nargin == 5 ) )
    {

/* ah vtkAssemblyPath *PickPropFrom (double selectionX, double selectionY, vtkPropCollection *); */
    double   temp0;
    double   temp1;
    vtkPropCollection  *temp2;
    vtkAssemblyPath  *temp20;
    error_flag = 0;

    if ( !args[2].isScalar() ) error_flag = 1;
    else temp0 = (double) args[2].asDouble();
    if ( !args[3].isScalar() ) error_flag = 1;
    else temp1 = (double) args[3].asDouble();
    if ( args(4).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp2 = reinterpret_cast<vtkPropCollection*>( args(4).uint64_scalar_value().value() );
    if (!error_flag)
      {
      temp20 = (vtk_pointer)->PickPropFrom(temp0,temp1,temp2);
      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp20);
      retval = octave_value( new vtk_object(tmp_ptr) );
      return retval;
      }
    }
  if ( ( method_name == "GetPickX" ) && ( nargin == 2 ) )
    {

/* ah double GetPickX () const;return this PickX1 this PickX2  */
    double   temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickX();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetPickY" ) && ( nargin == 2 ) )
    {

/* ah double GetPickY () const;return this PickY1 this PickY2  */
    double   temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickY();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetPickWidth" ) && ( nargin == 2 ) )
    {

/* ah double GetPickWidth () const;return this PickX2 this PickX1  */
    double   temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickWidth();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetPickHeight" ) && ( nargin == 2 ) )
    {

/* ah double GetPickHeight () const;return this PickY2 this PickY1  */
    double   temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickHeight();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetPickX1" ) && ( nargin == 2 ) )
    {

/* ah double GetPickX1 () const;return this PickX1  */
    double   temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickX1();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetPickY1" ) && ( nargin == 2 ) )
    {

/* ah double GetPickY1 () const;return this PickY1  */
    double   temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickY1();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetPickX2" ) && ( nargin == 2 ) )
    {

/* ah double GetPickX2 () const;return this PickX2  */
    double   temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickX2();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetPickY2" ) && ( nargin == 2 ) )
    {

/* ah double GetPickY2 () const;return this PickY2  */
    double   temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickY2();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetIsPicking" ) && ( nargin == 2 ) )
    {

/* ah int GetIsPicking (); */
    int      temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetIsPicking();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "GetPickResultProps" ) && ( nargin == 2 ) )
    {

/* ah vtkPropCollection *GetPickResultProps (); */
    vtkPropCollection  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickResultProps();
      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp20);
      retval = octave_value( new vtk_object(tmp_ptr) );
      return retval;
      }
    }
  if ( ( method_name == "GetPickedZ" ) && ( nargin == 2 ) )
    {

/* ah virtual double GetPickedZ () = 0; */
    double   temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetPickedZ();
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "RemoveProp" ) && ( nargin == 3 ) )
    {

/* ah void RemoveProp (vtkProp *); */
    vtkProp  *temp0;
    error_flag = 0;

    if ( args(2).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp0 = reinterpret_cast<vtkProp*>( args(2).uint64_scalar_value().value() );
    if (!error_flag)
      {
      vtk_pointer->RemoveProp(temp0);
      return retval;
      }
    }
  if ( ( method_name == "AddProp" ) && ( nargin == 3 ) )
    {

/* ah void AddProp (vtkProp *); */
    vtkProp  *temp0;
    error_flag = 0;

    if ( args(2).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp0 = reinterpret_cast<vtkProp*>( args(2).uint64_scalar_value().value() );
    if (!error_flag)
      {
      vtk_pointer->AddProp(temp0);
      return retval;
      }
    }
  if ( ( method_name == "GetProps" ) && ( nargin == 2 ) )
    {

/* ah vtkPropCollection *GetProps (); */
    vtkPropCollection  *temp20;
    error_flag = 0;

    if (!error_flag)
      {
      temp20 = (vtk_pointer)->GetProps();
      vtkObjectBase *tmp_ptr = (vtkObjectBase*)(temp20);
      retval = octave_value( new vtk_object(tmp_ptr) );
      return retval;
      }
    }
  if ( ( method_name == "HasProp" ) && ( nargin == 3 ) )
    {

/* ah int HasProp (vtkProp *); */
    vtkProp  *temp0;
    int      temp20;
    error_flag = 0;

    if ( args(2).type_id() != vtk_object::static_type_id() ) error_flag = 1;
    else temp0 = reinterpret_cast<vtkProp*>( args(2).uint64_scalar_value().value() );
    if (!error_flag)
      {
      temp20 = (vtk_pointer)->HasProp(temp0);
      retval = Array(double(temp20));
      return retval;
      }
    }
  if ( ( method_name == "RemoveAllProps" ) && ( nargin == 2 ) )
    {

/* ah void RemoveAllProps (); */
    error_flag = 0;

    if (!error_flag)
      {
      vtk_pointer->RemoveAllProps();
      return retval;
      }
    }

  if ( error_flag )
    {
    error("Method was found but arguments were wrong.");
    return retval;
    }

  // Check superclass methods.
  retval = feval("vtkObject",args, nargout); 
  // Reduce the result list to a scalar (if it is a single result)
  while ( retval.is_list() && retval.length() == 1 )
    {
    octave_value_list list = retval.list_value();
    retval = list(0);
    }




  return retval;
}

void populate_help_mapvtkViewport( std::map<std::string,std::string> &help_map ) 
{
  help_map["List"] = "GetClassName\nIsA\nNewInstance\nSafeDownCast\nAddViewProp\nGetViewProps\nHasViewProp\nRemoveViewProp\nRemoveAllViewProps\nAddActor2D\nRemoveActor2D\nGetActors2D\nSetBackground\nSetBackground\nGetBackground\nSetBackground2\nSetBackground2\nGetBackground2\nGradientBackgroundOn\nGradientBackgroundOff\nSetAspect\nSetAspect\nGetAspect\nComputeAspect\nSetPixelAspect\nSetPixelAspect\nGetPixelAspect\nSetViewport\nSetViewport\nGetViewport\nSetDisplayPoint\nSetDisplayPoint\nGetDisplayPoint\nSetViewPoint\nSetViewPoint\nGetViewPoint\nSetWorldPoint\nSetWorldPoint\nGetWorldPoint\nGetCenter\nIsInViewport\nGetVTKWindow\nDisplayToView\nViewToDisplay\nWorldToView\nViewToWorld\nDisplayToWorld\nWorldToDisplay\nGetSize\nGetOrigin\nGetTiledSize\nGetTiledSizeAndOrigin\nPickProp\nPickPropFrom\nGetPickX\nGetPickY\nGetPickWidth\nGetPickHeight\nGetPickX1\nGetPickY1\nGetPickX2\nGetPickY2\nGetIsPicking\nGetPickResultProps\nGetPickedZ\nRemoveProp\nAddProp\nGetProps\nHasProp\nRemoveAllProps\n";
  help_map["AddViewProp"] = " Add a prop to the list of props. Prop is the superclass of all\n\
   actors, volumes, 2D actors, composite props etc.";
  help_map["GetViewProps"] = " Return any props in this viewport.";
  help_map["HasViewProp"] = " Query if a prop is in the list of props.";
  help_map["RemoveViewProp"] = " Remove an actor from the list of actors.";
  help_map["RemoveAllViewProps"] = " Remove all actors from the list of actors.";
  help_map["AddActor2D"] = " Add/Remove different types of props to the renderer.\n\
   These methods are all synonyms to AddViewProp and RemoveViewProp.\n\
   They are here for convenience and backwards compatibility.";
  help_map["RemoveActor2D"] = " Add/Remove different types of props to the renderer.\n\
   These methods are all synonyms to AddViewProp and RemoveViewProp.\n\
   They are here for convenience and backwards compatibility.";
  help_map["GetActors2D"] = " Add/Remove different types of props to the renderer.\n\
   These methods are all synonyms to AddViewProp and RemoveViewProp.\n\
   They are here for convenience and backwards compatibility.";
  help_map["SetBackground"] = " Set/Get the background color of the rendering screen using an rgb color\n\
   specification.";
  help_map["SetBackground"] = " Set/Get the background color of the rendering screen using an rgb color\n\
   specification.";
  help_map["GetBackground"] = " Set/Get the background color of the rendering screen using an rgb color\n\
   specification.";
  help_map["SetBackground2"] = " Set/Get the second background color of the rendering screen\n\
   for gradient backgrounds using an rgb color specification.";
  help_map["SetBackground2"] = " Set/Get the second background color of the rendering screen\n\
   for gradient backgrounds using an rgb color specification.";
  help_map["GetBackground2"] = " Set/Get the second background color of the rendering screen\n\
   for gradient backgrounds using an rgb color specification.";
  help_map["GradientBackgroundOn"] = " Set/Get whether this viewport should have a gradient background\n\
   using the Background (top) and Background2 (bottom) colors.\n\
   Default is off.";
  help_map["GradientBackgroundOff"] = " Set/Get whether this viewport should have a gradient background\n\
   using the Background (top) and Background2 (bottom) colors.\n\
   Default is off.";
  help_map["SetAspect"] = " Set the aspect ratio of the rendered image. This is computed\n\
   automatically and should not be set by the user.";
  help_map["SetAspect"] = " Set the aspect ratio of the rendered image. This is computed\n\
   automatically and should not be set by the user.";
  help_map["GetAspect"] = " Set the aspect ratio of the rendered image. This is computed\n\
   automatically and should not be set by the user.";
  help_map["ComputeAspect"] = " Set the aspect ratio of the rendered image. This is computed\n\
   automatically and should not be set by the user.";
  help_map["SetPixelAspect"] = " Set the aspect ratio of a pixel in the rendered image.\n\
   This factor permits the image to rendered anisotropically\n\
   (i.e., stretched in one direction or the other).";
  help_map["SetPixelAspect"] = " Set the aspect ratio of a pixel in the rendered image.\n\
   This factor permits the image to rendered anisotropically\n\
   (i.e., stretched in one direction or the other).";
  help_map["GetPixelAspect"] = " Set the aspect ratio of a pixel in the rendered image.\n\
   This factor permits the image to rendered anisotropically\n\
   (i.e., stretched in one direction or the other).";
  help_map["SetViewport"] = " Specify the viewport for the Viewport to draw in the rendering window.\n\
   Coordinates are expressed as (xmin,ymin,xmax,ymax), where each\n\
   coordinate is 0 <= coordinate <= 1.0.";
  help_map["SetViewport"] = " Specify the viewport for the Viewport to draw in the rendering window.\n\
   Coordinates are expressed as (xmin,ymin,xmax,ymax), where each\n\
   coordinate is 0 <= coordinate <= 1.0.";
  help_map["GetViewport"] = " Specify the viewport for the Viewport to draw in the rendering window.\n\
   Coordinates are expressed as (xmin,ymin,xmax,ymax), where each\n\
   coordinate is 0 <= coordinate <= 1.0.";
  help_map["SetDisplayPoint"] = " Set/get a point location in display (or screen) coordinates.\n\
   The lower left corner of the window is the origin and y increases\n\
   as you go up the screen.";
  help_map["SetDisplayPoint"] = " Set/get a point location in display (or screen) coordinates.\n\
   The lower left corner of the window is the origin and y increases\n\
   as you go up the screen.";
  help_map["GetDisplayPoint"] = " Set/get a point location in display (or screen) coordinates.\n\
   The lower left corner of the window is the origin and y increases\n\
   as you go up the screen.";
  help_map["SetViewPoint"] = " Specify a point location in view coordinates. The origin is in the\n\
   middle of the viewport and it extends from -1 to 1 in all three\n\
   dimensions.";
  help_map["SetViewPoint"] = " Specify a point location in view coordinates. The origin is in the\n\
   middle of the viewport and it extends from -1 to 1 in all three\n\
   dimensions.";
  help_map["GetViewPoint"] = " Specify a point location in view coordinates. The origin is in the\n\
   middle of the viewport and it extends from -1 to 1 in all three\n\
   dimensions.";
  help_map["SetWorldPoint"] = " Specify a point location in world coordinates. This method takes\n\
   homogeneous coordinates.";
  help_map["SetWorldPoint"] = " Specify a point location in world coordinates. This method takes\n\
   homogeneous coordinates.";
  help_map["GetWorldPoint"] = " Specify a point location in world coordinates. This method takes\n\
   homogeneous coordinates.";
  help_map["GetCenter"] = " Return the center of this viewport in display coordinates.";
  help_map["IsInViewport"] = " Is a given display point in this Viewport's viewport.";
  help_map["GetVTKWindow"] = " Return the vtkWindow that owns this vtkViewport.";
  help_map["DisplayToView"] = " Convert display coordinates to view coordinates.";
  help_map["ViewToDisplay"] = " Convert view coordinates to display coordinates.";
  help_map["WorldToView"] = " Convert world point coordinates to view coordinates.";
  help_map["ViewToWorld"] = " Convert view point coordinates to world coordinates.";
  help_map["DisplayToWorld"] = " Convert display (or screen) coordinates to world coordinates.";
  help_map["WorldToDisplay"] = " Convert world point coordinates to display (or screen) coordinates.";
  help_map["GetSize"] = " Get the size and origin of the viewport in display coordinates. Note:\n\
   if the window has not yet been realized, GetSize() and GetOrigin()\n\
   return (0,0).";
  help_map["GetOrigin"] = " Get the size and origin of the viewport in display coordinates. Note:\n\
   if the window has not yet been realized, GetSize() and GetOrigin()\n\
   return (0,0).";
  help_map["GetTiledSize"] = " Get the size and origin of the viewport in display coordinates. Note:\n\
   if the window has not yet been realized, GetSize() and GetOrigin()\n\
   return (0,0).";
  help_map["GetTiledSizeAndOrigin"] = " Get the size and origin of the viewport in display coordinates. Note:\n\
   if the window has not yet been realized, GetSize() and GetOrigin()\n\
   return (0,0).";
  help_map["PickProp"] = " Return the Prop that has the highest z value at the given x, y position\n\
   in the viewport.  Basically, the top most prop that renders the pixel at\n\
   selectionX, selectionY will be returned.  If no Props are there NULL is\n\
   returned.  This method selects from the Viewports Prop list.";
  help_map["PickPropFrom"] = " Same as PickProp with two arguments, but selects from the given\n\
   collection of Props instead of the Renderers props.  Make sure\n\
   the Props in the collection are in this renderer.";
  help_map["GetPickX"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetPickY"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetPickWidth"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetPickHeight"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetPickX1"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetPickY1"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetPickX2"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetPickY2"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetIsPicking"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetPickResultProps"] = " Methods used to return the pick (x,y) in local display coordinates (i.e.,\n\
   it's that same as selectionX and selectionY).";
  help_map["GetPickedZ"] = " Return the Z value for the last picked Prop.";
  help_map["RemoveProp"] = " @deprecated Replaced by vtkViewport::RemoveViewProp() as of VTK 5.0.";
  help_map["AddProp"] = " @deprecated Replaced by vtkViewport::AddViewProp() as of VTK 5.0.";
  help_map["GetProps"] = " @deprecated Replaced by vtkViewport::GetViewProps() as of VTK 5.0.";
  help_map["HasProp"] = " @deprecated Replaced by vtkViewport::HasViewProp() as of VTK 5.0.";
  help_map["RemoveAllProps"] = " @deprecated Replaced by vtkViewport::RemoveAllViewProps() as of VTK 5.0.";
}


