@echo off
echo 1  ICON  favicon.ico > ico.rc
windres -i ico.rc -o ico.res -O coff

