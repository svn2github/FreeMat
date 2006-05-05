SUBDIRS = libs src images tools

DESTDIR = src

MAC_APP = $(PACKAGE).app

EXTRA_DIST = MFiles

dist-hook:
	rm -rf `find $(distdir)/MFiles -name .svn`
	rm -rf `find $(distdir)/MFiles -name '*~'`

$(MAC_APP): $(MAC_APP)/Contents/PkgInfo $(MAC_APP)/Contents/Info.plist
	@cp $(DESTDIR)/$(PACKAGE) $(MAC_APP)/Contents/MacOS/$(PACKAGE)
	@test -d $(MAC_APP)/Contents/Resources/ || mkdir -p $(MAC_APP)/Contents/Resources/
	@cp @top_srcdir@/src/appIcon.icns $(MAC_APP)/Contents/Resources/appIcon.icns

$(MAC_APP)/Contents/PkgInfo:
	@test -d $(MAC_APP)/Contents/MacOS/ || mkdir -p $(MAC_APP)/Contents/MacOS/
	@rm -f $(MAC_APP)/Contents/Px1kgInfo
	@echo "APPL????" > $(MAC_APP)/Contents/PkgInfo

$(MAC_APP)/Contents/Info.plist:
	@test -d $(MAC_APP)/Contents/MacOS/ || mkdir -p $(MAC_APP)/Contents/MacOS/
	@rm -f $(MAC_APP)/Contents/Info.plist
	@echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" >> $(MAC_APP)/Contents/Info.plist
	@echo "<!DOCTYPE plist SYSTEM \"file://localhost/System/Library/DTDs/PropertyList.dtd\">" >> $(MAC_APP)/Contents/Info.plist
	@echo "<plist version=\"0.9\">" >> $(MAC_APP)/Contents/Info.plist
	@echo "<dict>"  >> $(MAC_APP)/Contents/Info.plist
	@echo "<key>CFBundleIconFile</key>"  >> $(MAC_APP)/Contents/Info.plist
	@echo "<string>appIcon.icns</string>"  >> $(MAC_APP)/Contents/Info.plist
	@echo "<key>CFBundlePackageType</key>" >> $(MAC_APP)/Contents/Info.plist
	@echo "<string>APPL</string>" >> $(MAC_APP)/Contents/Info.plist
	@echo "<key>CFBundleExecutable</key>"  >> $(MAC_APP)/Contents/Info.plist
	@echo "<string>$(PACKAGE)</string>"  >> $(MAC_APP)/Contents/Info.plist
	@echo "</dict>" >> $(MAC_APP)/Contents/Info.plist
	@echo "</plist>" >> $(MAC_APP)/Contents/Info.plist

help-mingw: FORCE
	declare -x PATH="$(PATH):@QT4DIR@/bin" && @abs_top_builddir@/tools/helpgen/helpgen @abs_top_srcdir@ @abs_top_builddir@

help-unix: FORCE
	@abs_top_builddir@/tools/helpgen/helpgen @abs_top_srcdir@ @abs_top_builddir@

help: @help_target@

FORCE:
