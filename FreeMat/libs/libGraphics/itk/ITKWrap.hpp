#ifndef __ITKWrap_hpp__
#define __ITKWrap_hpp__

#include <itkImportImageFilter.h>
#include "Array.hpp"

template <int dims, int dataclass, typename pixelclass>
typename itk::Image<pixelclass, dims>::Pointer CreateITKFromArray(const Array& t)
{
  Array p = t;
  p.ensureNotScalarEncoded();
  typedef itk::Image<pixelclass, dims> ITKType;
  typedef itk::ImportImageFilter<pixelclass, dims> ImageImportType;
  typename ImageImportType::Pointer importer = ImageImportType::New();
  typename ImageImportType::RegionType region;
  for (int i=0;i<dims;i++)
    region.SetSize(i, p.dimensions()[i]);
  importer->SetRegion(region);
  typename ITKType::SpacingType vol_spacing;
  for (int i=0;i<dims;i++)
    vol_spacing[i] = 1;
  importer->SetSpacing(vol_spacing);
  typename ITKType::PointType vol_origin;
  for (int i=0;i<dims;i++)
    vol_origin[i] = 0;
  importer->SetOrigin(vol_origin);
  importer->SetImportPointer(reinterpret_cast<pixelclass*>(p.constReal<pixelclass>()), p.length(), false);
  importer->Update();
  return importer->GetOutput();
}

template <int dims, int dataclass, typename pixelclass>
Array CreateArrayFromITK(typename itk::Image<pixelclass,dims>::Pointer p)
{
  
}

#endif
