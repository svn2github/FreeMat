mne: main.cpp
	g++ -o mne -g main.cpp -framework Carbon
	/Developer/Tools/Rez -t APPL -o mne mac.r

bundle:
	mkdir -p mne.app
	mkdir -p mne.app/Contents
	cp Info.plist mne.app/Contents/.
	mkdir -p mne.app/Contents/MacOS
	mkdir -p mne.app/Contents/Resources
	rm -f mne.app/Contents/MacOS/mne
	rm -f mne.app/mne
	cp mne mne.app/Contents/MacOS/.
	ln -s mne.app/Contents/MacOS/mne mne.app/mne

clean:
	rm mne