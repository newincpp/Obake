if not exist ".\Plugins\" mkdir .\Plugins\
for /R .\ %%f in (*.dll) do copy %%f .\Plugins\
start Editor.exe