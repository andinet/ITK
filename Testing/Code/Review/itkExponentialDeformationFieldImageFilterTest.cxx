/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkExponentialDeformationFieldImageFilterTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkOrientedImage.h"
#include "itkVector.h"
#include "itkExponentialDeformationFieldImageFilter.h"


int itkExponentialDeformationFieldImageFilterTest(int, char* [] ) 
{
  // Define the dimension of the images
  const unsigned int ImageDimension = 3;

  typedef itk::Vector< double, ImageDimension >   PixelType;

  // Declare the types of the images
  typedef itk::Image<PixelType, ImageDimension>  ImageType;
  
  // Declare Iterator types apropriated for each image
  typedef itk::ImageRegionIteratorWithIndex< ImageType>  IteratorType;


  // Declare the type of the index to access images
  typedef itk::Index<ImageDimension>         IndexType;

  // Declare the type of the size
  typedef itk::Size<ImageDimension>          SizeType;

  // Declare the type of the Region
  typedef itk::ImageRegion<ImageDimension>   RegionType;

  // Create two images
  ImageType::Pointer inputImage  = ImageType::New();
  
  // Define their size, and start index
  SizeType size;
  size[0] = 2;
  size[1] = 2;
  size[2] = 2;

  IndexType start;
  start[0] = 0;
  start[1] = 0;
  start[2] = 0;

  RegionType region;
  region.SetIndex( start );
  region.SetSize( size );

  // Initialize Image A
  inputImage->SetLargestPossibleRegion( region );
  inputImage->SetBufferedRegion( region );
  inputImage->SetRequestedRegion( region );
  inputImage->Allocate();

  // Create one iterator for the Input Image (this is a light object)
  IteratorType it( inputImage, inputImage->GetBufferedRegion() );

  // Initialize the content of Image A
  PixelType vectorValue;
  vectorValue.Fill( 5.0 ); // FIXME: replace with something more interesting...
  
  it.GoToBegin();
  while( !it.IsAtEnd() )
    {
    it.Set( vectorValue );
    std::cout << it.Get() << std::endl;
    ++it;
    }

  // Declare the type for the filter
  typedef itk::ExponentialDeformationFieldImageFilter<
                                  ImageType, ImageType  >   FilterType;


  // Create one filter
  FilterType::Pointer filter = FilterType::New();


  // Connect the input images
  filter->SetInput( inputImage );

  filter->SetMaximumNumberOfIterations( 20 );
  
  // Execute the filter
  filter->Update();

  // Get the Smart Pointer to the Filter Output
  ImageType::Pointer outputImage = filter->GetOutput();

  // Create an iterator for going through the image output
  IteratorType ot(outputImage, outputImage->GetRequestedRegion());
  
  //  Check the content of the result image
  std::cout << "Verification of the output " << std::endl;
  const PixelType::ValueType epsilon = 1e-6;

  bool testpassed = true;

  ot.GoToBegin();
  it.GoToBegin();
  while( !ot.IsAtEnd() )
    {
    PixelType input  = it.Get();
    PixelType output = ot.Get();
    // The input is a constant field, its exponential
    // should be exactly equal
    testpassed &= ( (input-output).GetNorm() < epsilon );
    std::cout << input << " => ";
    std::cout << output  << std::endl;
    ++ot;
    ++it;
    }


  // Ask for the inverse deformation
  filter->ComputeInverseOn();
  
  // Execute the filter
  filter->Update();

  // Get the Smart Pointer to the Filter Output
  ImageType::Pointer outputImage2 = filter->GetOutput();

  // Create an iterator for going through the image output
  IteratorType ot2(outputImage, outputImage->GetRequestedRegion());
  
  //  Check the content of the result image
  std::cout << "Verification of the inverse output " << std::endl;

  ot2.GoToBegin();
  it.GoToBegin();
  while( !ot2.IsAtEnd() )
    {
    PixelType input  = it.Get();
    PixelType output = ot2.Get();
    // The input is a constant field, its inverse exponential
    // should be exactly equal to its opposite
    testpassed &= ( (input+output).GetNorm() < epsilon );
    std::cout << input << " => ";
    std::cout << output  << std::endl;
    ++ot2;
    ++it;
    }

  
  // Try with 0 iterations
  filter->ComputeInverseOff();
  filter->SetMaximumNumberOfIterations( 0 );
  
  // Execute the filter
  filter->Update();

  // Get the Smart Pointer to the Filter Output
  ImageType::Pointer outputImage3 = filter->GetOutput();

  // Create an iterator for going through the image output
  IteratorType ot3(outputImage, outputImage->GetRequestedRegion());
  
  //  Check the content of the result image
  std::cout << "Verification of the output with 0 iterations " << std::endl;

  ot3.GoToBegin();
  it.GoToBegin();
  while( !ot3.IsAtEnd() )
    {
    PixelType input  = it.Get();
    PixelType output = ot3.Get();
    // The input is a constant field, its inverse exponential
    // should be exactly equal to its opposite
    testpassed &= ( (input-output).GetNorm() < epsilon );
    std::cout << input << " => ";
    std::cout << output  << std::endl;
    ++ot3;
    ++it;
    }

  
  // Try inverse with 0 iterations
  filter->ComputeInverseOn();
  filter->SetMaximumNumberOfIterations( 0 );
  
  // Execute the filter
  filter->Update();

  // Get the Smart Pointer to the Filter Output
  ImageType::Pointer outputImage4 = filter->GetOutput();

  // Create an iterator for going through the image output
  IteratorType ot4(outputImage, outputImage->GetRequestedRegion());
  
  //  Check the content of the result image
  std::cout << "Verification of the inverse output with 0 iterations " << std::endl;

  ot4.GoToBegin();
  it.GoToBegin();
  while( !ot4.IsAtEnd() )
    {
    PixelType input  = it.Get();
    PixelType output = ot4.Get();
    // The input is a constant field, its inverse exponential
    // should be exactly equal to its opposite
    testpassed &= ( (input+output).GetNorm() < epsilon );
    std::cout << input << " => ";
    std::cout << output  << std::endl;
    ++ot4;
    ++it;
    }
  

  if (!testpassed) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
