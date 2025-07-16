# ESPHome Electrolux AC

This repository contains an external component that controls the Electrolux AC using IR.

## Supported Models

- EXP26U339HW (tested)
- EXP26U339CW (the same model as the previous one, but without the HEAT mode, you can disable HEAT in `yaml`)
- EXP34U339HW (untested but should work)

_If you tested any other model and it works for you, please create a ticket, so I can update this list._

## Usage

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/sys27/esphome-electrolux-ac
      ref: master
    components: [electrolux_ac]

remote_transmitter:
  pin: GPIOXX
  carrier_duty_percent: 50%

climate:
  - platform: electrolux_ac
    name: "Electrolux AC"
```

Replace `GPIOXX` with the correct pin on your device where IR Transmitter is connected.

## Electrolux Protocol

For `EXP26U339HW`:

| Offset | Size | Meaning               | Value                                                              |
| ------ | ---- | --------------------- | ------------------------------------------------------------------ |
| 0      | 8    | Constant Header       | `11000011`                                                         |
| 8      | 3    | Swing                 | Off - `111`, On - `000`                                            |
| 11     | 5    | Temperature (C)       | Calculated as `<expected_temp> - 8`                                |
| 16     | 16   | Unused                |                                                                    |
| 32     | 5    | Timer (unused)        | Probably hours                                                     |
| 37     | 3    | Fan Speed             | Auto - `101`, Low - `011`, Mid - `010`, High - `001`               |
| 40     | 5    | Timer (unused)        | Probably minutes                                                   |
| 46     | 4    | Unused                |                                                                    |
| 50     | 1    | Sleep (unused)        | On - `1`, Off - `0`                                                |
| 51     | 2    | Unused                |                                                                    |
| 53     | 3    | Mode                  | Auto - `000`, Cool - `001`, Fan - `110`, Heat - `100`, Dry - `010` |
| 56     | 21   | Unused                |                                                                    |
| 77     | 1    | On/Off                | On - `1`, Off - `0`                                                |
| 78     | 1    | Timer On/Off (unused) | On - `1`, Off - `0`                                                |
| 79     | 9    | Unused                |                                                                    |
| 88     | 4    | Buttons (unused)      |                                                                    |
| 92     | 4    | Unused                |                                                                    |
| 96     | 8    | Checksum              | Truncated sum of all previous bytes                                |

_Note: all bits are stored in the reverse order, for example, the `High` fan speed is `001`, but it will be `100` in the packet._

## Run locally / Contribute

- checkout/download the repository
- copy the `components` folder alongside your ESPHome configuration

Expected Folder Structure:

- esp32.yaml
- secrets.yaml
- components
  - electrolux_ac
    - \_\_init\_\_.py
    - climate.py
    - electrolux_ac_component.cpp
    - electrolux_ac_component.h

After that, you need to update your configuration to use the local version instead of `git`:

```yaml
external_components:
  - source:
      type: local
      path: components
    components: [electrolux_ac]
```
