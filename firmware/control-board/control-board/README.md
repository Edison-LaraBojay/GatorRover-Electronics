# Control Board Firmware

Teensy 4.1 firmware for the Gator Rover control board (PlatformIO).

```
pio run                  # build
pio run -t upload        # flash
pio device monitor       # USB serial
```

Each kind of hardware has an **abstract class** that says what it does, and one or more **implementations** that drive a specific part. Everything outside an implementation only uses the abstract class: the main loop, and later the Jetson command handler. Changing a part means writing a new implementation and changing its lines in `src/hardware.hpp`.

You can't instantiate an abstract class, so every category ships with a **dummy** implementation that produces plausible fake data. That way the whole firmware compiles and runs before any real driver exists.

## Layout

```
src/                           the application
  main.cpp                       setup + loop; only uses abstract classes
  hardware.hpp                   WHICH board, WHICH transports, WHICH implementation of each device

lib/                           modules (PlatformIO builds each folder as a library)
  common/                        shared by everything
    device.hpp                     Device base class + DeviceStatus
    clock.hpp                      sysclock::nowUs()
  transport/                     buses we abstract ourselves (I2C and UART use Arduino's TwoWire / HardwareSerial)
    can_bus.hpp                    CanBus (abstract) + CanDeviceConnector
    dummy/                         drops sends, receives nothing
  board/                         what is wired where
    board_pins.hpp                 Board: which peripheral and pins each connector uses
    control_board_v1.hpp           the v1 board's assignments (PLACEHOLDER until checked)
    connectors.hpp                 what each device receives: its bus or port + extra pins
  imu/
    imu.hpp                        Imu (abstract)
    dummy/                         sitting still and level
  lidar/
    range_finder.hpp               RangeFinder (abstract), single-point LiDAR
    dummy/                         fixed distance
  power/
    power_monitor.hpp              PowerMonitor (abstract)
    dummy/                         constant V / A, energy accumulates
  motor/
    motor_controller.hpp           MotorController (abstract)
    dummy/                         ideal motor, follows the command
```

A real implementation goes next to the dummy, for example `lib/imu/bmi088/`, `lib/lidar/tf02_pro/`, `lib/power/ina226/`, `lib/motor/spark_max/` or `lib/transport/teensy_flexcan/`. Things that aren't hardware modules also go in `lib/` as their own folder, for example the Jetson link.

Includes are by file name: `#include "imu.hpp"`, `#include "dummy/dummy_imu.hpp"`. Give implementation headers distinct names so they don't collide across folders.

## How construction works

The abstract class fixes the methods everything else calls. Constructors aren't part of it, and only `src/hardware.hpp` ever sees one, so each implementation takes whatever it needs. The convention is:

```cpp
Part(name, connector, Part::Config)
```

- **name**: the device id, e.g. `"imu"` or `"drive_fl"`.
- **connector**: the bus or port the device talks over, as the type the device needs, plus any extra pins, e.g. `ImuConnector{board.i2c, gyro_int, accel_int}` (a `TwoWire&`) or `CanDeviceConnector{drive_can, 1}`.
- **Config**: that part's settings, all with defaults (IMU range, current limit, ...). Define it outside the class and alias it inside, as the dummies do. A nested struct with default values breaks GCC when used as a constructor argument inside its own class.

`src/hardware.hpp` has two sections:

```cpp
// Transports: bus settings, the CAN bus object, and beginTransports()
static constexpr uint32_t kLidarBaud = 115200;
DriveCanBus drive_can {board.can_port, DriveCanBus::Config{}};
void beginTransports() { board.i2c.begin(); board.lidar_serial.begin(kLidarBaud); drive_can.begin(); ... }

// Devices: each gets its bus or port through its connector
using DriveMotor = DummyMotorController;        // swap the part here
DriveMotor drive_fl {"drive_fl", {drive_can, 1}, DriveMotor::Config{}};
```

Swapping a part is changing the `using` line, plus its construction line if the new part's constructor or Config differs. Bus settings (I2C clock, UART baud, CAN bitrate) live with the transports, so a part that needs a different baud means editing that constant too.

## Adding an implementation

1. Create `lib/<category>/<part>/<part>.hpp` (and `.cpp`).
2. Inherit from the category's abstract class, implement its pure virtual methods, and give it a `(name, connector, Config)` constructor. The dummy in the same folder is the smallest working example.
3. In `src/hardware.hpp`, include your header and point the `using` line at your class.

Using a vendor library inside an implementation is fine. It stays behind our abstract class, so nothing else depends on it.

## Adding a board revision

Copy `lib/board/control_board_v1.hpp` to e.g. `control_board_v2.hpp`, fill in its peripherals and pins, and switch the include and `board` line in `src/hardware.hpp`. If a new revision adds a connector, add its fields to `Board` in `board_pins.hpp` and its struct to `connectors.hpp`.

## Structure

These keep the dependencies from turning into a tangle:

- **Devices depend only on transports**, never on other devices. They get their `TwoWire&`, `HardwareSerial&` or `CanBus&` through their connector, never by naming `Wire` or `Serial2` themselves. Anything that combines devices (sensor fusion, drive kinematics, the Jetson handler) sits above the modules and only uses the abstract classes.
- **Constructors only store** the name, connector and Config. No hardware access, nothing started, nothing called on what they're given, so construction order never matters.
- **Startup is two phases in `setup()`:** `hw.beginTransports()`, then `begin()` on everything in `hw.devices`. A new bus or port gets started in `beginTransports()`; no implementation starts one itself or checks whether one is ready.
- **Keep a connector by value.** It holds references to long-lived buses, so copying it is cheap and safe.

## Ground rules

These keep implementations interchangeable:

- **Fill the sample in the units and frame its header documents** (SI units; IMU axes x forward, y left, z up). Raw register values never leave the implementation.
- **`status()` is device health; `sample.valid` says whether a reading can be used.** A LiDAR that is talking fine but sees no target is `Ok` with `valid = false`.
- **`update()` never blocks.** No `delay()`, and no loops that wait for data. Do a bounded amount of work and rate-limit inside the implementation.
- **Timestamp every reading** with the `now_us` passed to `update()`, or `sysclock::nowUs()`.
- **Motor controllers only promise duty cycle in [-1, 1] and `disable()`.** Velocity control and feedback are optional (`capabilities()`, `has_*` flags). Controller-specific settings (brake/coast, current limit, PID gains, fault codes) go in the implementation's `Config`.
- **Pins come from the connector, never hard-coded** in an implementation.

## Open decisions

These are deliberately left to whoever takes this on:

- Selecting setups: today you edit `hardware.hpp`. It could become one PlatformIO env per setup (e.g. bench vs. rover vs. the CAN prototype board), a config file, or the Jetson sending a configuration at boot.
- How received CAN frames reach the devices on the bus: each device reads `receive()` itself, or one place reads and routes by id.
- The Jetson ↔ Teensy protocol: framing, polling vs streaming, addressing devices by name.
- Scheduling: a single loop (current), timers, or an RTOS.
- Testing: host-side unit tests against the dummies via PlatformIO's native env.
- Whether the abstract classes need more fields or methods once real parts are in hand. They're a starting point, not a finished spec.
