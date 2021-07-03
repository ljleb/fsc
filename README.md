# Community .fsc File Utility

`fsc` is a [*Fruity Loop* score file](https://www.image-line.com/fl-studio-learning/fl-studio-online-manual/html/fformats_other_fsc.htm) editor. It allows to apply many operations that cannot be automated or done easily directly in FL Studio.

## Usage

`fsc`, at the time of writing this, is a Command Line Interface (CLI). You can use it by opening a terminal and typing:

```sh
fsc [options] <notes-fsc> <output-fsc>
```

In the above snippet, `<notes-fsc>` is the path to the *input* `.fsc` note data file, `<output-fsc>` is the path to the *resulting* `.fsc` note data file, and `[options]` is any combination of the supported options described below.

*Be careful when selecting an output path. Currently, if the file already exists, `fsc` silently overwrites the content!*

Supported options:

- `--pan-from <events-fsc>` **OR** `-p <events-fsc>` : map the `<events-fsc>` file *event data* to the pan of the notes
- `--velocity-from <events-fsc>` **OR** `-V <events-fsc>` : map the `<events-fsc>` file *event data* to the **Velocity** of the notes
- `--release-from <events-fsc>` **OR** `-R <events-fsc>` : map the `<events-fsc>` file *event data* to the **Release** of the notes
- `--mod-x-from <events-fsc>` **OR** `-x <events-fsc>` : map the `<events-fsc>` file *event data* to the **Modulation X** of the notes
- `--mod-y-from <events-fsc>` **OR** `-y <events-fsc>` : map the `<events-fsc>` file *event data* to the **Modulation Y** of the notes
- `--pitch-from <events-fsc>` **OR** `-P <events-fsc>` : map the `<events-fsc>` file *event data* to the **Pitch** of the notes

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

