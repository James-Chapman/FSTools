# FSTools
File System Tools

To clone this repo use make sure you use the `recursive` switch as it relies on a third party hash library:

`git clone --recursive https://github.com/James-Chapman/FSTools.git`

## HashGen
Generate MD5, SHA1, SHA256, SHA512 hashes.

```
USAGE:
  HashGen.exe <hashType> <path>

  hashType - Options are: md5, sha1, sha256 & sha512.
  path     - Path to a file or dir
```