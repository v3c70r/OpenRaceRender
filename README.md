# OpenRaceRender

![MacOS](https://github.com/v3c70r/OpenRaceRender/workflows/MacOS/badge.svg?branch=master)|  ![Windows](https://github.com/v3c70r/OpenRaceRender/workflows/Windows/badge.svg)|![Linux](https://github.com/v3c70r/OpenRaceRender/workflows/Linux/badge.svg)
Open Race Render is a tool to render [TrackAddict](http://racerender.com/TrackAddict/Features.html) data overlay. It can export the overlay as PNG file sequence. 
It is similar to [RaceRender](http://racerender.com/RR3/Features.html) but with limited functionalities.

## Run

```
OpenRaceRender <LogFile>
```
A file dialog will pop up at launch if LogFile is not provided.

## Supported Widget

See [screenshot](https://github.com/v3c70r/OpenRaceRender/raw/master/screen.png) for supported formats.

* Engine speed box with throttle percentage
* Map
* X-Y pannel accelorations
* Timeslider
* Plot on any selected columns

## TODOs

* Smoother interpolations of sensor data
* Denoising
* Per lap data comparison
* Render to video
* Picture-in-picture
* Simple video editing

## Dependencies

* ImGui
* std::filesystem from C++17
