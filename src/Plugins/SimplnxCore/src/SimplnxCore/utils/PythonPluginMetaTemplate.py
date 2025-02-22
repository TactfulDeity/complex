{% set name = "simplnx.#PLUGIN_NAME#" %}
{% set version = "1.0.0" %}

package:
  name: {{ name|lower }}
  version: {{ version }}

source:
  - path: ../
    folder: #PLUGIN_NAME#

build:
  noarch: python
  script: {{ PYTHON }} -m pip install ./#PLUGIN_NAME# -vv --no-deps --no-build-isolation
  number: 0

requirements:
  build:
    - python >=3.10
    - pytest
    - pip
    - hatchling
  run:
    - python >=3.10

test:
  imports:
    - #PLUGIN_NAME#
  commands:
    - pip check
  requires:
    - pip

about:
  home: https://www.dream3d.io/
  license: BSD
  summary: DREAM3D-NX consists of data analysis tools (Filters) that allow for the construction of customized workflows (Pipelines) to analyze data.

