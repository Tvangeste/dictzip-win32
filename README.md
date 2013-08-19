## Dictzip for Windows

This project provides `Dictzip` utility for Windows, built with Visual Studio 2012.
It requires no additional dependencies when executed, just a simple exe file.

The sources are based on the official [dictd](http://sourceforge.net/projects/dict/) v1.12.1,
with minor tweaks to be able to build with MSVC++ on Windows.

### How to build

You'd need Visual Studio 2012 (Express Edition should be fine too). Open the solution in the
Visual Studio and just press "Build".

Alternatively, you could run the build from the command line:

    MSBuild /v:m /nologo /p:Configuration=Release

### License

The original license is: GPL v1 or later.
