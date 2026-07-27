::del *.opt /s  ::不允许删除JLINK的设置

del *.scvd /s
del JLinkLog.txt /s

rmdir Objects /s /q
rmdir Listings /s /q

exit
