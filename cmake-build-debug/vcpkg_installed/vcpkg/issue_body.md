Package: libpq[core,lz4,openssl,zlib]:x64-linux@16.4

**Host Environment**

- Host: x64-linux
- Compiler: GNU 13.2.0
-    vcpkg-tool version: 2024-08-01-fd884a0d390d12783076341bd43d77c3a6a15658
    vcpkg-scripts version: 110c50d47 2024-09-05 (5 days ago)

**To Reproduce**

`vcpkg install `

**Failure logs**

```
-- Downloading https://ftp.postgresql.org/pub/source/v16.4/postgresql-16.4.tar.bz2;https://www.mirrorservice.org/sites/ftp.postgresql.org/source/v16.4/postgresql-16.4.tar.bz2 -> postgresql-16.4.tar.bz2...
-- Extracting source /home/mustafa/.vcpkg-clion/vcpkg/downloads/postgresql-16.4.tar.bz2
-- Applying patch unix/installdirs.patch
-- Applying patch unix/fix-configure.patch
-- Applying patch unix/single-linkage.patch
-- Applying patch unix/no-server-tools.patch
-- Applying patch unix/mingw-install.patch
-- Applying patch unix/python.patch
-- Applying patch windows/macro-def.patch
-- Applying patch windows/win_bison_flex.patch
-- Applying patch windows/msbuild.patch
-- Applying patch windows/spin_delay.patch
-- Applying patch android/unversioned_so.patch
-- Using source at /home/mustafa/.vcpkg-clion/vcpkg/buildtrees/libpq/src/tgresql-16-0b2040c781.clean
-- Found external ninja('1.12.0').
-- Getting CMake variables for x64-linux
CMake Error at scripts/cmake/vcpkg_find_acquire_program.cmake:166 (message):
  Could not find flex.  Please install it via your package manager:

      sudo apt-get install flex
Call Stack (most recent call first):
  buildtrees/versioning_/versions/libpq/dcaa1ec8552b8ef987d6d80f708b8828bcd795eb/portfile.cmake:43 (vcpkg_find_acquire_program)
  scripts/ports.cmake:192 (include)



```

**Additional context**

<details><summary>vcpkg.json</summary>

```
{
  "name": "phonedirectorybackcpp",
  "version-string": "1.0.0",
  "builtin-baseline": "110c50d4700794d87d95138cd8c1b3bbfee9bab5",
  "dependencies": [
    {
      "name": "crow",
      "version>=": "1.2.0"
    },
    {
      "name": "vcpkg-cmake-config",
      "version>=": "2024-05-23"
    },
    {
      "name": "vcpkg-cmake",
      "version>=": "2024-04-23"
    },
    {
      "name": "libpqxx",
      "version>=": "7.9.2"
    }
  ]
}

```
</details>
