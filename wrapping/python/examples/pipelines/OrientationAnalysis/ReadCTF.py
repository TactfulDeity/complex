import simplnx as nx

import itkimageprocessing as cxitk
import orientationanalysis as cxor
import simplnx_test_dirs as nxtest

import numpy as np

#Create a Data Structure
data_structure = nx.DataStructure()

# Filter 1
# Instantiate Filter
nx_filter = cxor.ReadCtfDataFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    cell_attribute_matrix_name=("Cell Data"),
    cell_ensemble_attribute_matrix_name=("Cell Ensemble Data"),
    output_image_geometry_path =nx.DataPath("DataContainer"),
    degrees_to_radians=True,
    edax_hexagonal_alignment=False,
    input_file=nxtest.get_data_directory() / "T12-MAI-2010/fw-ar-IF1-aptr12-corr.ctf"
)
nxtest.check_filter_result(nx_filter, result)

# Filter 2
# Instantiate Filter
nx_filter = nx.RotateSampleRefFrameFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    #output_image_geometry_path=nx.DataPath("DataContainer/"),
    remove_original_geometry=True,
    rotate_slice_by_slice=False,
    rotation_axis_angle=[0.0, 1.0, 0.0, 180.0],
    #rotation_matrix: List[List[float]] = ...,
    rotation_representation_index=0,
    input_image_geometry_path=nx.DataPath("DataContainer")
)
nxtest.check_filter_result(nx_filter, result)

# Filter 3
# Define ArrayThreshold object
threshold_1 = nx.ArrayThreshold()
threshold_1.array_path = nx.DataPath("DataContainer/Cell Data/Error")
threshold_1.comparison = nx.ArrayThreshold.ComparisonType.Equal
threshold_1.value = 0.0

# Define ArrayThresholdSet object
threshold_set = nx.ArrayThresholdSet()
threshold_set.thresholds = [threshold_1]

# Instantiate Filter
nx_filter = nx.MultiThresholdObjectsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(data_structure=data_structure,
                        array_thresholds_object=threshold_set,
                        output_data_array_name="Mask",
                        created_mask_type=nx.DataType.boolean)
nxtest.check_filter_result(nx_filter, result)

# Filter 4
# Instantiate Filter
nx_filter = cxor.ComputeIPFColorsFilter()
# Execute Filter with Parameters
result = nx_filter.execute(
    data_structure=data_structure,
    cell_euler_angles_array_path=nx.DataPath("DataContainer/Cell Data/EulerAngles"),
    cell_ipf_colors_array_name=("IPFColors"),
    cell_phases_array_path=nx.DataPath("DataContainer/Cell Data/Phases"),
    crystal_structures_array_path=nx.DataPath("DataContainer/Cell Ensemble Data/CrystalStructures"),
    mask_array_path=nx.DataPath("DataContainer/Cell Data/Mask"),
    reference_dir=[0.0, 0.0, 1.0],
    use_mask=True
)
nxtest.check_filter_result(nx_filter, result)

print("===> Pipeline Complete")
