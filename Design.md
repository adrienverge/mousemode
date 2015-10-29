# Mousemode

## Why timers? Why don't you rely on XNextEvent()?

XNextEvent() stop producing events in certain cases. For instance, after key B
is released in the following scenario, no more events are produced whereas key
A is still down (you can quickly check this using the `xev` command):

- 0s: key A is pressed
- 1s: key B is pressed
- 2s: key B is released
      // no more X events
- 9s: key A is released

## Useful

xinput test-xi2 --root

## Why not XGrabKeyboard()?

Because some window managers also call XGrabKeyboard() and then fail when
mousemode is on. For instance in Gnome Shell with mousemode on, clicks on the
*Activities* button will fail because Gnome tries to grab keyboard, but it is
already taken.

  gnome-shell: js/ui/overview.js:496
that calls
  https://github.com/GNOME/gnome-shell/blob/3.19.1/js/ui/main.js#L360
that calls
  https://github.com/GNOME/gnome-shell/blob/3.19.1/src/shell-global.c#L1086
that calls
  https://developer.gnome.org/meta/stable/MetaPlugin.html#meta-plugin-begin-modal
that calls
  https://github.com/GNOME/mutter/blob/86a913d/src/backends/x11/meta-backend-x11.c#L569

## Resources

http://lists.freedesktop.org/archives/xorg/2009-May/045692.html
