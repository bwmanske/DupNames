# DupNames
fuzzy matching of directory names

You need a DupNames.ini file or specify in INI file on the command line.

sample command line:<p>
`DupNames -ic:\test\sample.ini'

sample DupNames.ini:
```
[InitState]
TokenMatchCount=0

[PathList]
ProtectedPath1=v:\Movies\Animation
NormalPath1=w:\video\Finished Encoding
```

### sample INI notes:
- InitState section is unused
- ProtectedPath and NormalPath Followed by a number has no upper bound but the numbers nust be consecutive.
- When the option to delete is added, ProtectedPathxx(s) will not allow deletion
- If a file is added under a protected path then under normal it will remain protected
- if a file is added under a normal path then under protected it will become protected
- Paths include all subdirectories
- Only AVI, MKV, MP4 & M4V files are saved, the rest are ignored
