#include "ComputeNeighborhoodsFilter.hpp"

#include "Algorithms/ComputeNeighborhoods.hpp"

#include "simplnx/DataStructure/AttributeMatrix.hpp"
#include "simplnx/DataStructure/DataPath.hpp"
#include "simplnx/Filter/Actions/CreateArrayAction.hpp"
#include "simplnx/Filter/Actions/CreateNeighborListAction.hpp"
#include "simplnx/Parameters/ArraySelectionParameter.hpp"
#include "simplnx/Parameters/DataObjectNameParameter.hpp"
#include "simplnx/Parameters/GeometrySelectionParameter.hpp"

#include "simplnx/Utilities/SIMPLConversion.hpp"

#include "simplnx/Parameters/NumberParameter.hpp"

using namespace nx::core;

namespace nx::core
{
//------------------------------------------------------------------------------
std::string ComputeNeighborhoodsFilter::name() const
{
  return FilterTraits<ComputeNeighborhoodsFilter>::name.str();
}

//------------------------------------------------------------------------------
std::string ComputeNeighborhoodsFilter::className() const
{
  return FilterTraits<ComputeNeighborhoodsFilter>::className;
}

//------------------------------------------------------------------------------
Uuid ComputeNeighborhoodsFilter::uuid() const
{
  return FilterTraits<ComputeNeighborhoodsFilter>::uuid;
}

//------------------------------------------------------------------------------
std::string ComputeNeighborhoodsFilter::humanName() const
{
  return "Compute Feature Neighborhoods";
}

//------------------------------------------------------------------------------
std::vector<std::string> ComputeNeighborhoodsFilter::defaultTags() const
{
  return {className(), "Statistics", "Morphological", "Find", "Generate", "Calculate", "Determine"};
}

//------------------------------------------------------------------------------
Parameters ComputeNeighborhoodsFilter::parameters() const
{
  Parameters params;

  // Create the parameter descriptors that are needed for this filter
  params.insertSeparator(Parameters::Separator{"Input Parameter(s)"});

  params.insert(std::make_unique<Float32Parameter>(k_MultiplesOfAverage_Key, "Multiples of Average Diameter", "Defines the search radius to use when looking for 'neighboring' Features", 1.0F));

  params.insert(std::make_unique<GeometrySelectionParameter>(k_SelectedImageGeometryPath_Key, "Selected Image Geometry", "The target geometry", DataPath({"Data Container"}),
                                                             GeometrySelectionParameter::AllowedTypes{IGeometry::Type::Image}));

  params.insertSeparator(Parameters::Separator{"Input Feature Data"});
  params.insert(std::make_unique<ArraySelectionParameter>(
      k_EquivalentDiametersArrayPath_Key, "Equivalent Diameters", "Path to the array specifying the diameter of a sphere with the same volume as the Feature",
      DataPath({"Cell Feature Data", "EquivalentDiameters"}), ArraySelectionParameter::AllowedTypes{DataType::float32}, ArraySelectionParameter::AllowedComponentShapes{{1}}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_FeaturePhasesArrayPath_Key, "Phases", "Path to the array specifying to which Ensemble each Feature belongs",
                                                          DataPath({"Cell Feature Data", "Phases"}), ArraySelectionParameter::AllowedTypes{DataType::int32},
                                                          ArraySelectionParameter::AllowedComponentShapes{{1}}));
  params.insert(std::make_unique<ArraySelectionParameter>(k_CentroidsArrayPath_Key, "Centroids", "Path to the array specifying the X, Y, Z coordinates of Feature center of mass",
                                                          DataPath({"Cell Feature Data", "Centroids"}), ArraySelectionParameter::AllowedTypes{DataType::float32},
                                                          ArraySelectionParameter::AllowedComponentShapes{{3}}));
  params.insertSeparator(Parameters::Separator{"Output Feature Data"});
  params.insert(std::make_unique<DataObjectNameParameter>(k_NeighborhoodsArrayName_Key, "Neighborhoods",
                                                          "Number of Features that have their centroid within the user specified multiple of equivalent sphere diameters from each Feature",
                                                          "Neighborhoods"));
  params.insert(std::make_unique<DataObjectNameParameter>(k_NeighborhoodListArrayName_Key, "Neighborhood List",
                                                          "List of the Features whose centroids are within the user specified multiple of equivalent sphere diameter from each Feature",
                                                          "NeighborhoodList"));

  return params;
}

//------------------------------------------------------------------------------
IFilter::VersionType ComputeNeighborhoodsFilter::parametersVersion() const
{
  return 1;
}

//------------------------------------------------------------------------------
IFilter::UniquePointer ComputeNeighborhoodsFilter::clone() const
{
  return std::make_unique<ComputeNeighborhoodsFilter>();
}

//------------------------------------------------------------------------------
IFilter::PreflightResult ComputeNeighborhoodsFilter::preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler,
                                                                   const std::atomic_bool& shouldCancel) const
{
  auto pEquivalentDiametersArrayPathValue = filterArgs.value<DataPath>(k_EquivalentDiametersArrayPath_Key);
  auto pFeaturePhasesArrayPathValue = filterArgs.value<DataPath>(k_FeaturePhasesArrayPath_Key);
  auto pCentroidsArrayPathValue = filterArgs.value<DataPath>(k_CentroidsArrayPath_Key);
  auto pNeighborhoodsArrayNameValue = filterArgs.value<std::string>(k_NeighborhoodsArrayName_Key);
  auto pNeighborhoodListArrayNameValue = filterArgs.value<std::string>(k_NeighborhoodListArrayName_Key);

  PreflightResult preflightResult;

  nx::core::Result<OutputActions> resultOutputActions;

  // Get the Feature Phases Array and get its TupleShape
  auto tupleValidityCheck = dataStructure.validateNumberOfTuples({pEquivalentDiametersArrayPathValue, pFeaturePhasesArrayPathValue, pCentroidsArrayPathValue});
  if(!tupleValidityCheck)
  {
    return {MakeErrorResult<OutputActions>(-5730, fmt::format("The following DataArrays all must have equal number of tuples but this was not satisfied.\n{}", tupleValidityCheck.error()))};
  }
  const auto* cellFeatureData = dataStructure.getDataAs<AttributeMatrix>(pFeaturePhasesArrayPathValue.getParent());
  if(cellFeatureData == nullptr)
  {
    return MakePreflightErrorResult(
        -5731,
        fmt::format(
            "The selected input feature data arrays (equivalent diameters, phases, and centroids) are not located in an attribute matrix. Make sure you have selected the input arrays located in the "
            "cell feature attribute matrix of the selected geometry"));
  }

  // Create the Neighborhoods Array in the Feature Attribute Matrix
  {
    auto action = std::make_unique<CreateArrayAction>(DataType::int32, cellFeatureData->getShape(), std::vector<usize>{1ULL}, pFeaturePhasesArrayPathValue.replaceName(pNeighborhoodsArrayNameValue));
    resultOutputActions.value().appendAction(std::move(action));
  }
  // Create the NeighborList Output NeighborList in the Feature Attribute Matrix
  {
    auto action = std::make_unique<CreateNeighborListAction>(DataType::int32, cellFeatureData->getNumTuples(), pFeaturePhasesArrayPathValue.replaceName(pNeighborhoodListArrayNameValue));
    resultOutputActions.value().appendAction(std::move(action));
  }

  std::vector<PreflightValue> preflightUpdatedValues;

  // Return both the resultOutputActions and the preflightUpdatedValues via std::move()
  return {std::move(resultOutputActions), std::move(preflightUpdatedValues)};
}

//------------------------------------------------------------------------------
Result<> ComputeNeighborhoodsFilter::executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                                                 const std::atomic_bool& shouldCancel) const
{
  ComputeNeighborhoodsInputValues inputValues;

  inputValues.MultiplesOfAverage = filterArgs.value<float32>(k_MultiplesOfAverage_Key);
  inputValues.EquivalentDiametersArrayPath = filterArgs.value<DataPath>(k_EquivalentDiametersArrayPath_Key);
  inputValues.FeaturePhasesArrayPath = filterArgs.value<DataPath>(k_FeaturePhasesArrayPath_Key);
  inputValues.CentroidsArrayPath = filterArgs.value<DataPath>(k_CentroidsArrayPath_Key);
  inputValues.NeighborhoodsArrayName = inputValues.FeaturePhasesArrayPath.replaceName(filterArgs.value<std::string>(k_NeighborhoodsArrayName_Key));
  inputValues.NeighborhoodListArrayName = inputValues.FeaturePhasesArrayPath.replaceName(filterArgs.value<std::string>(k_NeighborhoodListArrayName_Key));
  inputValues.InputImageGeometry = filterArgs.value<DataPath>(k_SelectedImageGeometryPath_Key);

  return ComputeNeighborhoods(dataStructure, messageHandler, shouldCancel, &inputValues)();
}

namespace
{
namespace SIMPL
{
constexpr StringLiteral k_MultiplesOfAverageKey = "MultiplesOfAverage";
constexpr StringLiteral k_EquivalentDiametersArrayPathKey = "EquivalentDiametersArrayPath";
constexpr StringLiteral k_FeaturePhasesArrayPathKey = "FeaturePhasesArrayPath";
constexpr StringLiteral k_CentroidsArrayPathKey = "CentroidsArrayPath";
constexpr StringLiteral k_NeighborhoodsArrayNameKey = "NeighborhoodsArrayName";
constexpr StringLiteral k_NeighborhoodListArrayNameKey = "NeighborhoodListArrayName";
} // namespace SIMPL
} // namespace

Result<Arguments> ComputeNeighborhoodsFilter::FromSIMPLJson(const nlohmann::json& json)
{
  Arguments args = ComputeNeighborhoodsFilter().getDefaultArguments();

  std::vector<Result<>> results;

  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionToGeometrySelectionFilterParameterConverter>(args, json, SIMPL::k_EquivalentDiametersArrayPathKey,
                                                                                                                                      k_SelectedImageGeometryPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::FloatFilterParameterConverter<float32>>(args, json, SIMPL::k_MultiplesOfAverageKey, k_MultiplesOfAverage_Key));
  results.push_back(
      SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionFilterParameterConverter>(args, json, SIMPL::k_EquivalentDiametersArrayPathKey, k_EquivalentDiametersArrayPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionFilterParameterConverter>(args, json, SIMPL::k_FeaturePhasesArrayPathKey, k_FeaturePhasesArrayPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::DataArraySelectionFilterParameterConverter>(args, json, SIMPL::k_CentroidsArrayPathKey, k_CentroidsArrayPath_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::LinkedPathCreationFilterParameterConverter>(args, json, SIMPL::k_NeighborhoodsArrayNameKey, k_NeighborhoodsArrayName_Key));
  results.push_back(SIMPLConversion::ConvertParameter<SIMPLConversion::LinkedPathCreationFilterParameterConverter>(args, json, SIMPL::k_NeighborhoodListArrayNameKey, k_NeighborhoodListArrayName_Key));

  Result<> conversionResult = MergeResults(std::move(results));

  return ConvertResultTo<Arguments>(std::move(conversionResult), std::move(args));
}
} // namespace nx::core
