#pragma once

#include "SimplnxCore/SimplnxCore_export.hpp"

#include "simplnx/Filter/FilterTraits.hpp"
#include "simplnx/Filter/IFilter.hpp"

namespace nx::core
{
/**
 * @class PointSampleTriangleGeometryFilter
 * @brief This filter will ....
 */
class SIMPLNXCORE_EXPORT PointSampleTriangleGeometryFilter : public IFilter
{
public:
  PointSampleTriangleGeometryFilter() = default;
  ~PointSampleTriangleGeometryFilter() noexcept override = default;

  PointSampleTriangleGeometryFilter(const PointSampleTriangleGeometryFilter&) = delete;
  PointSampleTriangleGeometryFilter(PointSampleTriangleGeometryFilter&&) noexcept = delete;

  PointSampleTriangleGeometryFilter& operator=(const PointSampleTriangleGeometryFilter&) = delete;
  PointSampleTriangleGeometryFilter& operator=(PointSampleTriangleGeometryFilter&&) noexcept = delete;

  // Parameter Keys
  // static inline constexpr StringLiteral k_VertexParentGroup_Key = "vertex_geometry_parent_group";
  // static inline constexpr StringLiteral k_SamplesNumberType_Key = "samples_number_type";
  static inline constexpr StringLiteral k_NumberOfSamples_Key = "number_of_samples";
  static inline constexpr StringLiteral k_UseMask_Key = "use_mask";
  static inline constexpr StringLiteral k_TriangleGeometry_Key = "input_triangle_geometry_path";
  // static inline constexpr StringLiteral k_ParentGeometry_Key = "parent_geometry";
  static inline constexpr StringLiteral k_TriangleAreasArrayPath_Key = "triangle_areas_array_path";
  static inline constexpr StringLiteral k_MaskArrayPath_Key = "mask_array_path";
  static inline constexpr StringLiteral k_SelectedDataArrayPaths_Key = "input_data_array_paths";
  static inline constexpr StringLiteral k_VertexGeometryPath_Key = "output_vertex_geometry_path";
  static inline constexpr StringLiteral k_VertexDataGroupName_Key = "vertex_data_group_name";
  static inline constexpr StringLiteral k_UseSeed_Key = "use_seed";
  static inline constexpr StringLiteral k_SeedValue_Key = "seed_value";
  static inline constexpr StringLiteral k_SeedArrayName_Key = "seed_array_name";

  /**
   * @brief Reads SIMPL json and converts it simplnx Arguments.
   * @param json
   * @return Result<Arguments>
   */
  static Result<Arguments> FromSIMPLJson(const nlohmann::json& json);

  /**
   * @brief Returns the name of the filter.
   * @return
   */
  std::string name() const override;

  /**
   * @brief Returns the C++ classname of this filter.
   * @return
   */
  std::string className() const override;

  /**
   * @brief Returns the uuid of the filter.
   * @return
   */
  Uuid uuid() const override;

  /**
   * @brief Returns the human readable name of the filter.
   * @return
   */
  std::string humanName() const override;

  /**
   * @brief Returns the default tags for this filter.
   * @return
   */
  std::vector<std::string> defaultTags() const override;

  /**
   * @brief Returns the parameters of the filter (i.e. its inputs)
   * @return
   */
  Parameters parameters() const override;

  /**
   * @brief Returns parameters version integer.
   * Initial version should always be 1.
   * Should be incremented everytime the parameters change.
   * @return VersionType
   */
  VersionType parametersVersion() const override;

  /**
   * @brief Returns a copy of the filter.
   * @return
   */
  UniquePointer clone() const override;

protected:
  /**
   * @brief Takes in a DataStructure and checks that the filter can be run on it with the given arguments.
   * Returns any warnings/errors. Also returns the changes that would be applied to the DataStructure.
   * Some parts of the actions may not be completely filled out if all the required information is not available at preflight time.
   * @param dataStructure The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  PreflightResult preflightImpl(const DataStructure& dataStructure, const Arguments& filterArgs, const MessageHandler& messageHandler, const std::atomic_bool& shouldCancel) const override;

  /**
   * @brief Applies the filter's algorithm to the DataStructure with the given arguments. Returns any warnings/errors.
   * On failure, there is no guarantee that the DataStructure is in a correct state.
   * @param dataStructure The input DataStructure instance
   * @param filterArgs These are the input values for each parameter that is required for the filter
   * @param messageHandler The MessageHandler object
   * @return Returns a Result object with error or warning values if any of those occurred during execution of this function
   */
  Result<> executeImpl(DataStructure& dataStructure, const Arguments& filterArgs, const PipelineFilter* pipelineNode, const MessageHandler& messageHandler,
                       const std::atomic_bool& shouldCancel) const override;
};
} // namespace nx::core

SIMPLNX_DEF_FILTER_TRAITS(nx::core, PointSampleTriangleGeometryFilter, "ee34ef95-aa04-4ad3-8232-5783a880d279");
