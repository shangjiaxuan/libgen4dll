# libgen4dll

A simple tool for generating `.lib` file from `.dll`

## Notes

You need to either include the VC binaries in your path 
or use the VS command prompt to use.

Usage:

`libgen4dll <dll-files>`

If you have VC binaries in your path, you can simply drag and drop.

## Reasoning

I have failed many times to compile the tensorflow C++ bindings from 
source using bazel. The inconsistency is overwhelming, and bazel builds
require specific version of python and bazel, and configuring an
environment just for that seemed overwhelming.

I found out that the C binding binaries [here](https://www.tensorflow.org/install/lang_c) was in fact a C++ binding with VC name mangling conventions. But the download did not include a .lib file, and thus requires the caller to call `LoadLibrary()` and `GetProcAddress()` for every called function. The C++ functions requires using mangled names and calling member functions with `__thiscall` convention seemed really difficult.

Thus creating a lib file for the dll seemed the right approach. As suggested
[here](https://stackoverflow.com/questions/9360280/how-to-make-a-lib-file-when-have-a-dll-file-and-a-header-file),
I made a simple tool that will automate this process, since accoring to the tensorflow
page, the C and C++ API is subject to change in future versions, thus requires a
automated approach to do this.

## License
This tool is free to use and modify as long as the derived forms are still open-source.(GPL-v3)


