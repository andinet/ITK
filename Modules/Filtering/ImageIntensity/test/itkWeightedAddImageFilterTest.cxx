/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkWeightedAddImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkTestingMacros.h"


int
itkWeightedAddImageFilterTest(int argc, char * argv[])
{
  if (argc != 2)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << itkNameOfTestExecutableMacro(argv);
    std::cerr << " alpha" << std::endl;
    return EXIT_FAILURE;
  }


  // Define the dimension of the images
  constexpr unsigned int myDimension = 3;

  // Declare the types of the images
  using myImageType1 = itk::Image<float, myDimension>;
  using myImageType2 = itk::Image<float, myDimension>;
  using myImageType3 = itk::Image<float, myDimension>;

  // Declare the type of the index to access images
  using myIndexType = itk::Index<myDimension>;

  // Declare the type of the size
  using mySizeType = itk::Size<myDimension>;

  // Declare the type of the Region
  using myRegionType = itk::ImageRegion<myDimension>;

  // Declare the type for the ADD filter
  using myFilterType = itk::WeightedAddImageFilter<myImageType1, myImageType2, myImageType3>;

  // Declare the pointers to images
  using myImageType1Pointer = myImageType1::Pointer;
  using myImageType2Pointer = myImageType2::Pointer;
  using myImageType3Pointer = myImageType3::Pointer;
  using myFilterTypePointer = myFilterType::Pointer;

  // Create two images
  const myImageType1Pointer inputImageA = myImageType1::New();
  const myImageType2Pointer inputImageB = myImageType2::New();

  // Define their size, and start index
  mySizeType size;
  size[0] = 2;
  size[1] = 2;
  size[2] = 2;

  myIndexType start;
  start[0] = 0;
  start[1] = 0;
  start[2] = 0;

  const myRegionType region{ start, size };

  // Initialize Image A
  inputImageA->SetRegions(region);
  inputImageA->Allocate();

  // Initialize Image B
  inputImageB->SetRegions(region);
  inputImageB->Allocate();


  // Declare Iterator types apropriated for each image
  using myIteratorType1 = itk::ImageRegionIteratorWithIndex<myImageType1>;
  using myIteratorType2 = itk::ImageRegionIteratorWithIndex<myImageType2>;
  using myIteratorType3 = itk::ImageRegionIteratorWithIndex<myImageType3>;

  // Create one iterator for Image A (this is a light object)
  myIteratorType1 it1(inputImageA, inputImageA->GetBufferedRegion());

  // Initialize the content of Image A
  std::cout << "First operand " << std::endl;
  while (!it1.IsAtEnd())
  {
    it1.Set(2.0);
    std::cout << it1.Get() << std::endl;
    ++it1;
  }

  // Create one iterator for Image B (this is a light object)
  myIteratorType2 it2(inputImageB, inputImageB->GetBufferedRegion());

  // Initialize the content of Image B
  std::cout << "Second operand " << std::endl;
  while (!it2.IsAtEnd())
  {
    it2.Set(3.0);
    std::cout << it2.Get() << std::endl;
    ++it2;
  }


  // Create an ADD Filter
  const myFilterTypePointer filter = myFilterType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(filter, WeightedAddImageFilter, BinaryGeneratorImageFilter);


  // Connect the input images
  filter->SetInput1(inputImageA);
  filter->SetInput2(inputImageB);

  auto alpha = static_cast<typename myFilterType::RealType>(std::stod(argv[1]));
  filter->SetAlpha(alpha);
  ITK_TEST_SET_GET_VALUE(alpha, filter->GetAlpha());

  // Get the Smart Pointer to the Filter Output
  const myImageType3Pointer outputImage = filter->GetOutput();


  // Execute the filter
  filter->Update();

  // Create an iterator for going through the image output
  myIteratorType3 it3(outputImage, outputImage->GetBufferedRegion());

  // Print the content of the result image
  std::cout << " Result " << std::endl;
  while (!it3.IsAtEnd())
  {
    std::cout << it3.Get() << std::endl;
    ++it3;
  }


  // All objects should be automatically destroyed at this point
  return EXIT_SUCCESS;
}
