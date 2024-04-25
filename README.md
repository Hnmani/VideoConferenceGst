# Video Conference App
This is a video conference app for linux, to run the program you need to install the library gstreamer,
Here is the code you can use to install the library in linux,
```
apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-tools gstreamer1.0-x gstreamer1.0-alsa gstreamer1.0-gl gstreamer1.0-gtk3 gstreamer1.0-qt5 gstreamer1.0-pulseaudio
```

To Compile the code run:

```
sudo -o videoconference videoconference.c `pkg-config --cflags --libs gstreamer1.0`
```

You need to run the code on both the systems for initiating the video conference and you also need to pass IP Address of the other system with which you want to communicate as a command line parameter
To run the code:

```
./videoconference RECEIVER_IP_ADDRESS
```

Enjoy,
HNM
