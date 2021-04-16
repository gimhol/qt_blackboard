echo %dst%
set target_bin_path=%~dp0..\qt_blackboard_shared\bin
set target_lib_path=%~dp0..\qt_blackboard_shared\lib
set target_inc_path=%~dp0..\qt_blackboard_shared\include
set target_dev_path=%~dp0..\qt_blackboard_shared\dev

xcopy /Y /I "%~dp0..\blackboard-core\*.h" "%target_inc_path%"
xcopy /Y /I "%~dp0..\blackboard-utils\*.h" "%target_inc_path%"
cd ../blackboard-core/debug
xcopy /Y /I "*.exe" "%target_bin_path%"
xcopy /Y /I "*.dll" "%target_bin_path%"
xcopy /Y /I "*.lib" "%target_inc_path%"
xcopy /Y /I "*.pdb" "%target_dev_path%"
xcopy /Y /I "*.map" "%target_dev_path%"
cd ../release
xcopy /Y /I "*.exe" "%target_bin_path%"
xcopy /Y /I "*.dll" "%target_bin_path%"
xcopy /Y /I "*.lib" "%target_inc_path%"
xcopy /Y /I "*.pdb" "%target_dev_path%"
xcopy /Y /I "*.map" "%target_dev_path%"
cd ../../blackboard-utils/debug
xcopy /Y /I "*.exe" "%target_bin_path%"
xcopy /Y /I "*.dll" "%target_bin_path%"
xcopy /Y /I "*.lib" "%target_inc_path%"
xcopy /Y /I "*.pdb" "%target_dev_path%"
xcopy /Y /I "*.map" "%target_dev_path%"
cd ../release
xcopy /Y /I "*.exe" "%target_bin_path%"
xcopy /Y /I "*.dll" "%target_bin_path%"
xcopy /Y /I "*.lib" "%target_inc_path%"
xcopy /Y /I "*.pdb" "%target_dev_path%"
xcopy /Y /I "*.map" "%target_dev_path%"
cd ../../blackboard-demo/debug
xcopy /Y /I "*.exe" "%target_bin_path%"
xcopy /Y /I "*.dll" "%target_bin_path%"
xcopy /Y /I "*.lib" "%target_inc_path%"
xcopy /Y /I "*.pdb" "%target_dev_path%"
xcopy /Y /I "*.map" "%target_dev_path%"
cd ../release
xcopy /Y /I "*.exe" "%target_bin_path%"
xcopy /Y /I "*.dll" "%target_bin_path%"
xcopy /Y /I "*.lib" "%target_inc_path%"
xcopy /Y /I "*.pdb" "%target_dev_path%"
xcopy /Y /I "*.map" "%target_dev_path%"