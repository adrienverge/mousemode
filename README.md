# mousemode

Trash your mouse.

## What?!

mousemode is a program to move your pointer and issue clicks by using your
keyboard only. Activate the "mouse" mode with a special key combo, then use
the keyboard to move, click, drag and drop, double-click etc. Finally escape
mousemode by hitting the Escape key.

## Compile and install

Dependencies:

* `libXtst-devel` `libyaml-devel`
* `libxtst-dev` `libyaml-dev`

```sh
aclocal && autoconf && automake --add-missing
./configure
make
sudo make install
```

## Run

```sh
mousemode
```

## Use

Once `mousemode` is running in the background, hit `Ctrl` + `Super` + `Alt` to
enter the "mouse" mode.

Then use `H`, `J`, `K`, `L` to move the pointer, and `S`, `D`, `F` to perform
clicks.

To escape from the "mouse" mode, hit `Esc`.
