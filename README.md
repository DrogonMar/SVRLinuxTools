# SVRLinuxTools
A SteamVR overlay for controlling linux stuff like MPD or PulseAudio.

## Issues
Currently on the latest version GPMDP the song time slider doesnt work, this is because GPMDP returns 0 for this info.
No idea if this is just me or something.

## CLion
There's already configurations for running with and without VR just look
for:
 * SVRLinuxTools-VR
 * SVRLinuxTools-NonVR

## Goals
- [ ] Make ImageCache a bit smarter, *Currently it will have two version of each image because of how googles url's are*.
- [ ] Have a volume control that talks to PulseAudio.
- [ ] Add a mixer to the volume control.
- [ ] Add simple play/pause for MPD.
- [ ] Allow users to select and add songs to MPD kinda like Cantata or
other MPD clients.
- [ ] Add full support for [GPMDP](https://www.googleplaymusicdesktopplayer.com/).
- [ ] Have the UI not look bad.
- [ ] Make the UI in QML (optional).
- [ ] Make it work on Windows.

## Building from source
```shell script
git submodule update --init --recursive
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# now run it :)
./SVRLinuxTools
```

## Arguments
* --novr: Run without the need for SteamVR useful for debugging.
