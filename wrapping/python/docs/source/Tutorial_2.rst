.. _Tutorial_2:

==================================
Tutorial 2: Manipulating Pipelines
==================================

This tutorial is an introduction on how to interact with simplnx pipelines. It will cover the following topics:

- Inserting a filter into a pipeline
- Modifying the arguments of a filter in an existing pipeline
- Printing filter information from a pipeline

###################################
2.1 Introduction
###################################

Setup your environment in the same way as from :ref:`Tutorial 1<Tutorial_1_Setup>`. In this tutorial we will be manipulating a basic pipeline.

###################################
2.2 Necessary Import Statements
###################################

Use the same import statements as from :ref:`Tutorial 1<Tutorial_1_Imports>`:

.. code:: python

    import simplnx as nx
    import numpy as np

If you will be using filters from DREAM3D-NX's other plugins, then you may additionally need the following:

.. code:: python

    import itkimageprocessing as nxitk
    import orientationanalysis as nxor

Also use the following imports:

.. code:: python

    from pathlib import Path

###################################
2.3 Set Output Directory
###################################

Set the output directory where the output from this tutorial will be stored, and create the directory.  We are going to set the output directory in the same location as the current script.

.. code:: python

    output_dir = Path(__file__).parent / 'Output' / 'Tutorial_2_Output'
    output_dir.mkdir(exist_ok=True, parents=True)

#####################################
2.4 Creating the DataStructure Object
#####################################

.. code:: python

    data_structure = nx.DataStructure()

This line creates a DataStructure object, which will serve as the overall container for our `simplnx` data.

#############################
2.5 Reading the Pipeline File
#############################

SIMPLNX has an object called the "Pipeline" object that holds a linear list of filters. This object
has an API that allows the developer to query the pipeline for filters and also to insert filters
into the pipeline. We are going to add a line of code to read a pipeline directly from a ".d3dpipeline" file.

.. code:: python

    pipeline_file_path = Path(__file__).parent / 'Pipelines' / 'lesson_2.d3dpipeline'
    pipeline = nx.Pipeline().from_file(str(pipeline_file_path))

########################################
2.6 Printing Pipeline Filter Information
########################################

One basic example of using the pipeline object is to loop over each filter in the pipeline and print its human name. This example can  
be done as follows:

.. code:: python

    for index, filter in enumerate(pipeline):
        print(f"[{index}]: {filter.get_filter().human_name()}")

This loop iterates over each filter in the pipeline and prints out its index and human name.

The output should look like this:

.. code:: text

    [0]: Create Geometry
    [1]: Create Data Array
    [2]: Write DREAM3D NX File

######################################
2.7 Inserting a Filter into a Pipeline
######################################

To extend or customize a data processing workflow, you might need to insert new filters into an existing pipeline. The following steps demonstrate how to do this.

***********************************
2.7.1 Defining the Filter Arguments
***********************************

Here, we define the arguments for the new filter. These arguments specify the configuration for the CreateDataGroup filter that we will add to the pipeline.

.. code:: python

    create_data_group_args = {
        "data_object_path": nx.DataPath("Small IN100/EBSD Data")
    }

**************************
2.7.2 Inserting the Filter
**************************

We can insert the new filter into the pipeline at the specified position (index 2). The CreateDataGroupFilter is used to create the filter, and the arguments are passed to configure it.

.. code:: python

    pipeline.insert(2, nx.CreateDataGroupFilter(), create_data_group_args)

.. _2.7.3:

*************************************
2.7.3 Executing the Modified Pipeline
*************************************

Each time a pipeline is executed, it will return an :ref:`nx.IFilter.ExecuteResult <result>` object. This 
object contains the errors and warnings that occurred while the filter was executing.

A typical function that can be written to properly error check the 'result' value is the following:

.. code:: python

    def check_pipeline_result(result: nx.Result) -> None:
        """
        This function will check the `result` for any errors. If errors do exist then a 
        `RuntimeError` will be thrown. Your own code to modify this to return something
        else that doesn't just stop your script in its tracks.
        """
        if len(result.warnings) != 0:
            for w in result.warnings:
                print(f'Warning: ({w.code}) {w.message}')
        
        has_errors = len(result.errors) != 0 
        if has_errors:
            for err in result.errors:
                print(f'Error: ({err.code}) {err.message}')
            raise RuntimeError(result)
        
        print(f"Pipeline :: No errors running the pipeline")

If you were to integrate this into your own code, then we would execute the pipeline and check the result like this:

.. code:: python

    result = pipeline.execute(data_structure)
    check_pipeline_result(result=result)

This code executes the modified pipeline with the DataStructure object. The `check_pipeline_result function` is used to verify the execution result, printing out any errors or warnings from the pipeline execution.

.. _2.7.4:

**********************************
2.7.4 Saving the Modified Pipeline
**********************************

We can save the modified pipeline configuration to a new pipeline file for future use.
Let's create the output pipeline file path using pathlib, and then call `to_file` to save the pipeline to a pipeline file.

.. code:: python

    output_pipeline_file_path = output_dir / 'lesson_2a_modified_pipeline.d3dpipeline'
    pipeline.to_file("Modified Pipeline", str(output_pipeline_file_path))

##############################
2.8 Modifying Pipeline Filters
##############################

Sometimes you need to adjust the parameters of existing filters in your pipeline. Here’s how you can modify a filter's parameters.

.. _2.8.1:

************************************
2.8.1 Modifying the Filter Arguments
************************************

We can modify the parameters of a given filter by writing and using a short method:

.. code:: python

    def modify_pipeline_filter(pipeline: nx.Pipeline, index: int, key: str, value):
        # The get_args method retrieves the current arguments, and set_args applies the modifications.
        param_dict = pipeline[index].get_args()
        param_dict[key] = value
        pipeline[index].set_args(param_dict)
    
    modify_pipeline_filter(pipeline, 1, "numeric_type", nx.NumericType.int8)

Here, we use the modify_pipeline_filter method to change the 2nd filter's NumericType parameter value to int8.

*************************************
2.8.2 Executing the Modified Pipeline
*************************************

Just like in :ref:`Section 2.7.3 <2.7.3>`, we can execute the modified pipeline and check the result using the check_pipeline_result method:

.. code:: python

    result = pipeline.execute(data_structure)
    check_pipeline_result(result=result)

**********************************
2.8.3 Saving the Modified Pipeline
**********************************

Just like in :ref:`Section 2.7.4 <2.7.4>`, we can save the modified pipeline to a new pipeline file for future use:

.. code:: python

    output_pipeline_file_path = output_dir / 'lesson_2b_modified_pipeline.d3dpipeline'
    pipeline.to_file("Modified Pipeline", str(output_pipeline_file_path))

#########################
2.9 Looping On a Pipeline
#########################

In certain cases, it might be necessary to modify pipeline filters in a loop.  One example where this is handy is when the same pipeline needs to be run on multiple image slices.

Let's modify a pipeline in a loop to generate IPF maps using DREAM3D-NX.

The Pipeline that we will modify is as follows:
    1. Read EDAX EBSD Data (.ang)
    2. Rotate Euler Reference Frame
    3. Rotate Sample Reference Frame
    4. Multi-Threshold Objects
    5. Generate IPF Colors
    6. Write Image (ITK)
    7. Write DREAM3D NX File

Filter 1 is the ReadAngDataFilter which we will need to adjust the input file (https://www.dream3d.io/python_docs/OrientationAnalysis.html#OrientationAnalysis.ReadAngDataFilter).

Filter 6 is the image writing filter where we need to adjust the output file (https://www.dream3d.io/python_docs/ITKImageProcessing.html#write-image-itk).

Filter 7 is the write dream3d file filter where we need to adjust the output file (https://www.dream3d.io/python_docs/simplnx.html#write-dream3d-nx-file).

*************************
2.9.1 Setting Up the Loop
*************************

The `check_pipeline_result` method from :ref:`Section 2.7.3 <2.7.3>` and the `modify_pipeline_filter` method from :ref:`Section 2.8.1 <2.8.1>` can be used inside a loop to update file paths for the 1st, 6th, and 7th filters.  The pipeline can be executed and saved (and the execution result checked) at the end of each iteration of the loop.

.. code:: python

    # Loop over the EBSD pipeline
    edax_ipf_colors_output_dir = output_dir / 'Edax_IPF_Colors'
    edax_ipf_colors_output_dir.mkdir(exist_ok=True, parents=True)
    for i in range(1, 6):
        # Create the data structure
        data_structure = nx.DataStructure()

        # Read the pipeline file
        pipeline_file_path = Path(__file__).parent / 'Pipelines' / 'lesson_2_ebsd.d3dpipeline'
        pipeline = nx.Pipeline().from_file(str(pipeline_file_path))

        # Modify file paths for the 1st, 6th, and 7th filters
        modify_pipeline_filter(pipeline, 0, "input_file", str(Path(__file__).parent / 'Data' / 'Small_IN100' / f'Slice_{i}.ang'))
        modify_pipeline_filter(pipeline, 5, "file_name", str(edax_ipf_colors_output_dir / f'Small_IN100_Slice_{i}.png'))
        modify_pipeline_filter(pipeline, 6, "export_file_path", str(edax_ipf_colors_output_dir.parent / f'Small_IN100_Slice_{i}.dream3d'))

        # Execute the modified pipeline
        result = pipeline.execute(data_structure)
        check_pipeline_result(result=result)

        # Output the modified pipeline
        output_pipeline_file_path = edax_ipf_colors_output_dir / f'Small_IN100_Slice_{i}.d3dpipeline'
        pipeline.to_file(f"Small_IN100_Slice_{i}", str(output_pipeline_file_path))

The code above will generate IPF maps for SmallIN100 slices 1-6.

##################
2.10 Full Examples
##################

Full examples of the concepts in this tutorial are located at:

https://github.com/BlueQuartzSoftware/NXWorkshop/blob/develop/PythonTutorial/tutorial_2a.py
https://github.com/BlueQuartzSoftware/NXWorkshop/blob/develop/PythonTutorial/tutorial_2b.py
https://github.com/BlueQuartzSoftware/NXWorkshop/blob/develop/PythonTutorial/tutorial_2c.py