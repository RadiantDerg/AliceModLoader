# To build

Note: I haven't figured out how to compile Detours on your local machine, so I took the built binaries from their GitHub. But the instructions should be as follows:

```sh
1. cd ./dependencies/Detours
2. set DETOURS_TARGET_PROCESSOR=x86
3. set DETOURS_CONFIG=Debug #or Release
3. nmake
4. cd ../..
5. msbuild /p:Platform=x86
```
