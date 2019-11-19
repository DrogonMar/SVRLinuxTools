# SVRLinuxTools
A SteamVR overlay for controlling linux stuff like MPD or PulseAudio.

## CLion
There's already configurations for running with and without VR just look
for:
 * SVRLinuxTools-VR
 * SVRLinuxTools-NonVR

# Goals
- [ ] Have a volume control that talks to PulseAudio
- [ ] Add a mixer to the volume control
- [ ] Add simple play/pause for MPD
- [ ] Allow users to select and add songs to MPD kinda like Cantata or
other MPD clients.
- [ ] Add support for [GPMDP](https://www.googleplaymusicdesktopplayer.com/).
- [ ] Make it work on Windows.

# Building from source
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