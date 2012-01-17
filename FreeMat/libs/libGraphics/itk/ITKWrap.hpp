#ifndef __ITKWrap_hpp__
#define __ITKWrap_hpp__

#include <itkImportImageFilter.h>
#include "Array.hpp"

template <int dims, int dataclass, typename pixelclass>
typename itk::Image<pixelclass, dims>::Pointer CreateITKFromArray(const Array& t)
{
  typedef itk::Image<pixelclass, dims> ITKType;
  typedef itk::ImportImageFilter<pixelclass, dims> ImageImportType;
  ImageImportType::Pointer importer = ImageImportType::New();
  ImageImportType::RegionType region;
  for (int i=0;i<dims;i++)
    region.SetSize(i, static_cast<ImageImportType::RegionType::SizeValueType>(t.dimensions()[i]));
  importer->SetRegion(region);
  ImageType::SpacingType vol_spacing;
  for (int i=0;i<dims;i++)
    vol_spacing[i] = 1;
  importer->SetSpacing(vol_spacing);
  ImageType::PointType vol_origin;
  for (int i=0;i<dims;i++)
    vol_origin[i] = 0;
  importer->SetOrigin(vol_origin);
  importer->SetImportPointer(reinterpret_cast<pixelclass*>(t.getConstVoidPointer()), t.length(), false);
  importer->Update();
  return importer->GetOutput();
}

#endif
