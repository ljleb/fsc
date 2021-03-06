# Community .fsc File Utility

`fsc` is a [*Fruity Loop* score file](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/fformats_other_fsc.htm) editor. It allows to apply many operations that cannot be automated or done easily directly in FL Studio.

## Download

You can hop to the [Relases page](https://github.com/lebel-louisjacob/fsc/releases) and select your platform to get the latest version of the tool.

## Usage

At the time of writing this, `fsc` is a Command Line Interface (CLI). You can use it by opening a terminal and typing:

```sh
fsc [options] <notes-fsc> <output-fsc>
```

In the above snippet, `<notes-fsc>`, `<output-fsc>` and `[options]` are placeholder values. `<notes-fsc>` is the path of the *input* `.fsc` note data file, `<output-fsc>` is the path to the *resulting* `.fsc` note data file, and `[options]` is any combination of the supported options described below.

By default, `fsc` simply copies the contents of `<notes-fsc>` to `<output-fsc>`, without applying any modification.

*Be careful when selecting an output path. Currently, if the file already exists, `fsc` silently overwrites the content!*

Supported options:

- `--pan-from <events-fsc>` *(shorthand `-p`)* : map the `<events-fsc>` file *event data* to the **pan** of the notes
- `--velocity-from <events-fsc>` *(shorthand `-V`)* : map the `<events-fsc>` file *event data* to the **Velocity** of the notes
- `--release-from <events-fsc>` *(shorthand `-R`)* : map the `<events-fsc>` file *event data* to the **Release** of the notes
- `--mod-x-from <events-fsc>` *(shorthand `-x`)* : map the `<events-fsc>` file *event data* to the **Modulation X** of the notes
- `--mod-y-from <events-fsc>` *(shorthand `-y`)* : map the `<events-fsc>` file *event data* to the **Modulation Y** of the notes
- `--pitch-from <events-fsc>` *(shorthand `-P`)* : map the `<events-fsc>` file *event data* to the **Pitch** of the notes

## Build

### Dependencies

To build this project, currently a linux operating system is needed. ([WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) is however supported)

The following packages are required in order to generate targets:

- make
- g++
- mingw-w64

On ubuntu, you can install them with this script:

```sh
apt install build-essential g++ mingw-w64
```

### Generating Targets

To generate debug targets:

```sh
make debug
```

To generate release targets:

```sh
make release
```

To clear all generated targets:

```sh
make clean
```

