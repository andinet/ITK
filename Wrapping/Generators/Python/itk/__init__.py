# ==========================================================================
#
#   Copyright NumFOCUS
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#          http://www.apache.org/licenses/LICENSE-2.0.txt
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# ==========================================================================*/

"""itk: Top-level container package for ITK wrappers."""

from itkConfig import ITK_GLOBAL_VERSION_STRING as _LOCAL_ITK_GLOBAL_VERSION_STRING

__version__ = _LOCAL_ITK_GLOBAL_VERSION_STRING
del _LOCAL_ITK_GLOBAL_VERSION_STRING


def _initialize_module():
    """
    A function to explicitly avoid polluting the global namespace
    """

    def _get_lazy_attributes(local_lazy_attributes, l_module, l_data):
        template_feature_tuples = [
            (t[0], t[3]) if len(t) > 3 else (t[0], False) for t in l_data["templates"]
        ]
        for (template_name, is_in_library) in template_feature_tuples:
            if is_in_library:
                # insert in front front if in library
                local_lazy_attributes.setdefault(template_name, []).insert(0, l_module)
            else:
                # append to end
                local_lazy_attributes.setdefault(template_name, []).append(l_module)
        if "snake_case_functions" in l_data:
            for function in l_data["snake_case_functions"]:
                # snake case always appended to end
                local_lazy_attributes.setdefault(function, []).append(l_module)

    import itkBase
    import itkConfig
    import itkLazy
    import itkInitHelpers
    import os
    import sys

    this_module = sys.modules[__name__]
    if itkConfig.LazyLoading:
        # If we are loading lazily (on-demand), make a dict mapping the available
        # classes/functions/etc. (read from the configuration modules) to the
        # modules they are declared in. Then pass that dict to a LazyITKModule
        # instance and (later) do some surgery on sys.modules so that the 'itk'
        # module becomes that new instance instead of what is executed from this
        # file.
        lazy_attributes = {}
        for module, data in itkBase.itk_base_global_module_data.items():
            _get_lazy_attributes(lazy_attributes, module, data)

        if isinstance(this_module, itkLazy.LazyITKModule):
            # Handle reload case where we've already done this once.
            # If we made a new module every time, multiple reload()s would fail
            # because the identity of sys.modules['itk'] would always be changing.
            this_module.__init__(__name__, lazy_attributes)
            del lazy_attributes
        else:
            this_module = itkLazy.LazyITKModule(__name__, lazy_attributes)
    else:
        # We're not lazy-loading. Just load the modules in the order specified in
        # the known_modules list for consistency.
        for module in itkBase.itk_base_global_module_data.keys():
            itkBase.itk_load_swig_module(module, this_module.__dict__)

    # Regardless of how it was loaded, fill up the itk module with the ITK types
    # and extras.
    import itkTypes

    for k, v in itkTypes.__dict__.items():
        if k != "itkCType" and not k.startswith("_"):
            setattr(this_module, k, v)
    del itkTypes
    for k, v in itkInitHelpers.__dict__.items():
        if not k.startswith("_"):
            setattr(this_module, k, v)

    import itkExtras

    for k, v in itkExtras.__dict__.items():
        if not k.startswith("_"):
            setattr(this_module, k, v)
    del itkExtras

    # --
    # Needed to propagate symbol to itk.image from itkTemplate.image
    import itkTemplate

    setattr(this_module, "image", itkTemplate.image)
    setattr(this_module, "output", itkTemplate.output)
    del itkTemplate
    # --

    # Populate itk.ITKModuleName
    for module, data in itkBase.itk_base_global_module_data.items():
        attributes = {}
        _get_lazy_attributes(attributes, module, data)
        itk_module = itkLazy.LazyITKModule(module, attributes)
        setattr(this_module, module, itk_module)

    # Set the __path__ attribute, which is required for this module to be used as a
    # package
    setattr(this_module, "__path__", __path__)
    setattr(this_module, "__spec__", __spec__)  # pytype: disable=name-error

    if itkConfig.LazyLoading:
        # this has to be the last step, else python gets confused about itkTypes
        # and itkExtras above. I'm not sure why...
        sys.modules[__name__] = this_module
    else:
        # do some cleanup
        del module, this_module, itk_module
        del itkBase, itkConfig, itkLazy, itkInitHelpers, os, sys


# Now do the initialization
_initialize_module()
