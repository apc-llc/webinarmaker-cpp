# Make webinars out of still slides and demo videos

This is an engine to create a video presentation that interleaves still slides or images with video fragments, such as demos and walkthroughs.

Based on [moviemaker-cpp](https://github.com/apc-llc/moviemaker-cpp).


## Building

```
mkdir build
cd build
cmake ..
make
```

## Usage

All working material for the webinar is provided in separate files and is encoded into a single continuous video as described by a config file, for example:

```json
{
    "output" : "webinar.mp4",
    "fps" : 30,
    "width" : 1600,
	"height" : 900,
    "sequence" : [
        { "type" : "slide", "path" : "../../../build/slides.pdf", "page" : 1, "duration" : 10.0 },
        { "type" : "slide", "path" : "../../../build/slides.pdf", "page" : 2, "duration" : 10.0 },
        { "type" : "slide", "path" : "../../../build/slides.pdf", "page" : 3, "duration" : 10.0 },
        { "type" : "slide", "path" : "../../../build/slides.pdf", "page" : 4, "duration" : 10.0 },
        { "type" : "slide", "path" : "../../../build/slides.pdf", "page" : 5, "duration" : 10.0 },
        { "type" : "video", "path" : "videos/video1.mp4" },
        { "type" : "slide", "path" : "../../../build/slides.pdf", "page" : 6, "duration" : 10.0 },
        { "type" : "video", "path" : "videos/video2.mp4" },
        { "type" : "slide", "path" : "../../../build/slides.pdf", "page" : 7, "duration" : 10.0 },
        { "type" : "video", "path" : "videos/video3.mp4" },
        { "type" : "slide", "path" : "../../../build/slides.pdf", "page" : 8, "duration" : 10.0 },
        { "type" : "video", "path" : "videos/video4.mp4" },
        { "type" : "video", "path" : "videos/video5.mp4" },
        { "type" : "slide", "path" : "../../../build/slides.pdf", "page" : 9, "duration" : 10.0 }
    ]
}
```

The webinar maker shall be executed with the config file as an argument:

```
./mkwebinar ../example.json
```

