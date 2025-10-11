mkdir /p C:\WWW\links\legacy-images
rem "The backslash after the parameter mustn't be present."
rem copy %1Output\LegacyCGIWindows\Win32\Debug\links-legacy.exe C:\WWW\links
rem copy %1Sources\Content\legacy-images C:\WWW\links\legacy-images
rem copy %1Sources\Content\Styles.css C:\WWW\links
rem copy %1Sources\Content\Common.js C:\WWW\links
echo %~1Output\LegacyCGIWindows\Win32\Debug\links-legacy.exe
copy "%~dp0..\..\Output\LegacyCGIWindows\Win32\Debug\links-legacy.exe" C:\WWW\links
