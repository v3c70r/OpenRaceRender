# OpenRaceRender

Open Race Render is a tool to render [TrackAddict](http://racerender.com/TrackAddict/Features.html) data overlay on video. 
It is similar to [RaceRender](http://racerender.com/RR3/Features.html) but with limited functionalities.

*Video output is NOT supported for now*

## Run

```
OpenRaceRender <LogFile> <VideoFile>
```

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

* libmpv
* ImGui
* std::filesystem from C++17

