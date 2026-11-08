# Control Board Packets

## Overview

The control board acts as a bridge between the Jetson computer and the rover peripherals (motor drivers, LiDARs, etc). Using the Teensy microcontroller, the control board communicates with the Jetson through UART. The Teensy receives commands from the Jetson, executes the corresponding operations, and transmits sensor measurements, status information, etc back to the Jetson.

This `README` contains my proposal for the packet structure, a WIP command table, and an example of the Jetson and Teensy using this proposed structure.

## Communication Protocol / Packet Structure

The UART will follow the 8N1 convention. A __baud rate of 115,200__ is a good starting point, but this may need to be tested and changed in the future. The 8N1 convention is:
- 8 data bits (1 byte).
- No parity bit.
- 1 stop bit.

I am proposing that we use a packet structure based on a Start-Of-Frame (SOF) sequence to identify the beginning of a packet. If more robust packet framing and resynchronization are required, COBS can be used instead.

The packet structure can be the following:

| SOF 1 | SOF 2 | Command | ID | Length `L` | Payload/Parameters | CRC 1 | CRC 2 |
|---|---|---|---|---|---|---|---|
| `0xAA` | `0x55` | 1 byte | 1 byte | 1 byte | Varies with command | 1 byte | 1 byte |

> NOTE: __We will use little-endian__. This is applicable when transmitting payloads and CRC.

The execution algorithm goes as follows:
```
1) Scan for 0xAA.
2) Scan for 0x55.
3) Read the header, containing the command, ID, and length L.
4) Read exactly L bytes from the payload/parameters.
5) Read the two CRC bytes and validate the packet.
6) Return to (1).

NOTE: the receiver should not be scanning for the SOF while reading the payload, as valid data may happen to contain it.
```


## Byte Explanation

### SOF 1
 The first byte of the Start-of-Frame sequence. The Jetson searches for this byte when receiving data from the Teensy, while the Teensy searches for this byte when waiting for a command from the Jetson. __We will define SOF1 as `0xAA`__. This value can be easier to find while debugging because it is just alternating bits: `0b10101010`.
### SOF 2
 The second byte of the Start-of-Frame sequence. If both SOF bytes have been recognized in order, the Jetson/Teensy should begin parsing the packet. __We will define SOF2 as `0x55`__. It is also just alternating bits: `0b01010101`. 
### Command
 Identifies the operation to be executed. A table of commands can be found below __(it will be completed when peripheral firmware becomes available)__.
### ID
 A sequence number used to associate transmitted requests with their corresponding responses. The Jetson will send a command with an ID. The Teensy's response to that command will contain the same ID. The ID increments with each new command, and will wrap around to `0x00` after reaching `0xFF`. An ID of 1 byte is capable of supporting 256 unique values, which should be sufficient.
### Length (`L`)
 Specifies the number of bytes contained in the payload/parameters. This determines where the payload ends and the CRC begins.
### Payload/Parameters
 Contains the data associated with the command. The Jetson sends command parameters (ex: motor driver PWM). The Teensy sends data payloads (ex: the data collected from a sensor).
### CRC
 The UART communication between the Jetson and Teensy may be susceptible to noise from the rover peripherals. To detect  corruption, a two-byte Cyclic Redundancy Check (CRC-16) can be implemented. The receiver calculates CRC from the data bytes (command, ID, length, and payload) and compares it with their received CRC value. If the calculated and received CRC are not the same, the data received was corrupted and should be discarded. We will use CRC-16/XMODEM. More [information about it can be found here](https://resources.basicmicro.com/cyclic-redundancy-check-explained/). There is also an [online CRC-16/XMODEM calculator](https://crccalc.com/?crc=0x38544&method=CRC-16%2FXMODEM&datatype=hex&outtype=hex) if you want to experiment.

 The code below outlines how to calculate and use CRC in C++:

 ```cpp
#include <cstdint>
#include <cstddef>

uint16_t crc16(unsigned char *packet, int nBytes) {

    uint16_t crc = 0;

    for (int byteIndex = 0; byteIndex < nBytes; byteIndex++) {

        crc = crc ^ ((uint16_t)packet[byteIndex] << 8);

        for (unsigned char bit = 0; bit < 8; bit++) {

            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc = crc << 1;
            }
            
        }
    }
    return crc;
}


// Validating packet

uint16_t received = ((uint16_t)(packet[crc_lsb_index + 1]) << 8) | (uint16_t)(packet[crc_lsb_index]);
uint16_t calculated = crc16(packet_data_bytes, num_bytes_in_data);

if (received == calculated) {
    execute();
} else {
    discard();
}
 ```



## Command Table **WIP**

Packet structure repeated for ease of reference:
| SOF 1 | SOF 2 | Command | ID | Length `L` | Payload/Parameters | CRC 1 | CRC 2 |
|---|---|---|---|---|---|---|---|
| `0xAA` | `0x55` | 1 byte | 1 byte | 1 byte | Varies with command | 1 byte | 1 byte |

Command table __(just an example for now)__:

| Command | Operation | Jetson &rarr; Teensy Parameters | Teensy &rarr; Jetson Payload |
|--|--|--|--|
| `0x00` | Set motor driver PWM | Motor driver ID (1 byte) ; Duty cycle (1 byte) | ACK (1 byte) |
| `0x01` | Read from motor encoder | Encoder ID (1 byte) ; ms before timeout (2 bytes) | Data from encoder (4 bytes)
| `0x02` |  |  |  |
| ...  | ... | ... | ... |
| `0xFF` |  |  |  |


## Example
If the 5th command that the Jetson is sending to the Teensy is a read from the 6th motor encoder with a 512 (`0x0200`) ms timeout, it should send the folowing packet to the Teensy:

| `0xAA` | `0x55` | `0x01` | `0x05` | `0x03` | `0x06` | `0x00` | `0x02` | `0x06` | `0x6F` |
|--|--|--|--|--|--|--|--|--|--|
|SOF1|SOF2|Command|ID|Length|Parameter1|Parameter2 byte 1|Parameter2 byte 2|CRC byte 2| CRC byte 1|

The CRC bytes are `0x6F06`, calculated from the data bytes (command, ID, length, and parameters).

After parsing, validating, and executing the command, the Teensy gets a value of `0xAABBCCDD` from the encoder. The packet to send to the Jetson should be:

| `0xAA` | `0x55` | `0x01` | `0x05` | `0x04` | `0xDD` | `0xCC` | `0xBB` | `0xAA` | `0x22` | `0x66` |
|--|--|--|--|--|--|--|--|--|--|--|
|SOF1|SOF2|Command|ID|Length|Payload byte 1|Payload byte 2|Payload byte 3|Payload byte 4|CRC byte 1| CRC byte 2|