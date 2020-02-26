echo %dst%
xcopy /Y /I "%~dp0*.h" "%cd%\..\..\qt_blackboard_shared\include"
xcopy /Y /I "%cd%\..\*.exe" "%cd%\..\..\qt_blackboard_shared\bin\"
xcopy /Y /I "%cd%\..\*.dll" "%cd%\..\..\qt_blackboard_shared\bin\"
xcopy /Y /I "%cd%\..\*.lib" "%cd%\..\..\qt_blackboard_shared\lib\"
xcopy /Y /I "%cd%\..\*.pdb" "%cd%\..\..\qt_blackboard_shared\dev\"
xcopy /Y /I "%cd%\..\*.map" "%cd%\..\..\qt_blackboard_shared\dev\"
