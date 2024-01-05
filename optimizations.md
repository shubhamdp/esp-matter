## Configuration options to optimize RAM and Flash

There are several configuration options available to optimize Flash and RAM storage.
The following list highlights key options that significantly increase the free DRAM, heap, and reduce the flash
footprint.

For more optimization, we've also listed the reference links to esp-idf's optimization guide.

### Environment setup

All numbers mentioned below were collected in the following environment:

* esp-idf: [v5.1.2](https://github.com/espressif/esp-idf/tree/v5.1.2)
* esp-matter: [2f692ac](https://github.com/espressif/esp-matter/tree/2f692ac)
* example: [light](https://github.com/espressif/esp-matter/tree/2f692ac/examples/light)
* SoC: ESP32
* [Patch to collect the heap stats](https://gist.github.com/shubhamdp/d08ed7ff6e7ace1046ea6a0ec616c2dc)

### Few Notes

* These numbers may vary slightly in a different environment.
* All numbers are in bytes
* As we are using BLE only for commissioning, BLE memory is freed after commissioning,
    hence there is an increase in the free heap on commissioning. (`CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING=y`)
* After building an example, some DRAM will be utilized, and the remaining DRAM will be
    allocated as heap. Therefore, a direct increase in the heap will reflect as an increase in free heap.

#### 1. Default Configuration

We have used the default light example here, and below listed are the static and dynamic sizes.

- Static memory

|            |  size   |
|------------|---------|
| Used DRAM  | 97988   |
| Used IRAM  | 128090  |
| Used Flash | 1326251 |

- Dynamic memory

|                     | Free heap | 
|---------------------|-----------|
| On Bootup           | 58560     |
| After Commissioning | 132108    |

<details>
<summary> idf.py size output </summary>

```
Used static DRAM:   97988 bytes (  26592 remain, 78.7% used)
      .data size:   22796 bytes
      .bss  size:   75192 bytes

Used static IRAM:  128090 bytes (   2982 remain, 97.7% used)
      .text size:  127063 bytes
   .vectors size:    1027 bytes

Used Flash size : 1326251 bytes
           .text: 1090415 bytes
         .rodata:  235580 bytes
Total image size: 1477137 bytes (.bin may be padded larger)
```

</details>

#### 2. Disable the chip-shell

Console shell is helpful when developing/debugging the application, but may not be necessary in
production. Disabling the shell can save space. Disable the below configuration option.

```
CONFIG_ENABLE_CHIP_SHELL=n
```

- Static memory

|            |  size   | Decreased by |
|------------|---------|--------------|
| Used DRAM  | 95076   | 2912         |
| Used IRAM  | 125366  | 2724         |
| Used Flash | 1280743 | 45508        |

- Dynamic memory

|                     | Free heap | Increase |
|---------------------|-----------|----------|
| On Bootup           | 69948     | 11388    | 
| After Commissioning | 143480    | 11372    |

<details>
<summary> idf.py size output </summary>

```
Used static DRAM:   95076 bytes (  29504 remain, 76.3% used)
      .data size:   22780 bytes
      .bss  size:   72296 bytes

Used static IRAM:  125366 bytes (   5706 remain, 95.6% used)
      .text size:  124339 bytes
   .vectors size:    1027 bytes

Used Flash size : 1280743 bytes
           .text: 1054667 bytes
         .rodata:  225820 bytes
Total image size: 1428889 bytes (.bin may be padded larger)
```

</details>

#### 3. Adjust the dynamic endpoint count

The default dynamic endpoint count is 16, which may be excessive for a normal application creating only 2 endpoints.
eg: light, only has two endpoints, one for root endpoint and one for actual light.
Adjusting this to a lower value, corresponding to the actual number of endpoints the application will create, can save DRAM.

Here, we have set the dynamic endpoint count to 4. Increase in the DRAM per endpoint is ~275 bytes.

```
CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT=4
```

- Dynamic memory

|            |  size   | Decreased by |
|------------|---------|--------------|
| Used DRAM  | 91772   | 3304         |
| Used IRAM  | 125366  | 0            |
| Used Flash | 1280699 | 44           |

- Static memory

|                     | Free heap | Increase |
|---------------------|-----------|----------|
| On Bootup           | 73232     | 3284     | 
| After Commissioning | 146772    | 3292     |

<details>
<summary> idf.py size output </summary>

```
Total sizes:
Used static DRAM:   91772 bytes (  32808 remain, 73.7% used)
      .data size:   22780 bytes
      .bss  size:   68992 bytes

Used static IRAM:  125366 bytes (   5706 remain, 95.6% used)
      .text size:  124339 bytes
   .vectors size:    1027 bytes

Used Flash size : 1280699 bytes
           .text: 1054623 bytes
         .rodata:  225820 bytes
Total image size: 1428845 bytes (.bin may be padded larger)
```

</details>

#### 4. Use the newlib nano formatting

This optimization saves approximately 25-50K of flash, depending on the target. In our case, it results in a flash
reduction of 61.5 KB.

Additionally, it lowers the high watermark of task stack for functions that call printf() or other string formatting
functions.

Reference: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/size.html#newlib-nano-formatting

```
CONFIG_NEWLIB_NANO_FORMAT=y
```

- Static memory

|            |  size   | Decreased by |
|------------|---------|--------------|
| Used DRAM  | 91772   | 0            |
| Used IRAM  | 125346  | 20           |
| Used Flash | 1219219 | 61480        |

- Dynamic memory

|                     | Free heap | Increase |
|---------------------|-----------|----------|
| On Bootup           | 76316     | 3084     | 
| After Commissioning | 149332    | 2560     |

<details>
<summary> idf.py size output </summary>

```
Used static DRAM:   91772 bytes (  32808 remain, 73.7% used)
      .data size:   22780 bytes
      .bss  size:   68992 bytes

Used static IRAM:  125346 bytes (   5726 remain, 95.6% used)
      .text size:  124319 bytes
   .vectors size:    1027 bytes

Used Flash size : 1219219 bytes
           .text:  996319 bytes
         .rodata:  222644 bytes
Total image size: 1367345 bytes (.bin may be padded larger)
```

</details>

#### 5. Few BLE Optimizations

Since most devices will primarily operate as BLE peripherals and typically won't need more than one connection
(especially if it's just a Matter app), we can optimize by reducing the maximum allowed connections, thereby
saving DRAM. Additionally, given the peripheral nature of these devices, we can disable the central and
observer roles, for further optimization.

Below are the configuration options that can be set to achieve these optimizations.

```
CONFIG_NIMBLE_MAX_CONNECTIONS=1
CONFIG_BTDM_CTRL_BLE_MAX_CONN=1
CONFIG_BT_NIMBLE_ROLE_CENTRAL=n
CONFIG_BT_NIMBLE_ROLE_OBSERVER=n
```

- Static memory

|            |  size   | Decreased by |
|------------|---------|--------------|
| Used DRAM  | 91332   | 440          |
| Used IRAM  | 125322  | 24           |
| Used Flash | 1214831 | 4388         |

- Dynamic memory

|                     | Free heap | Increase |
|---------------------|-----------|----------|
| On Bootup           | 78884     | 2568     | 
| After Commissioning | 149376    | 44       |

<details>
<summary> idf.py size output </summary>

```
Used static DRAM:   91332 bytes (  33248 remain, 73.3% used)
      .data size:   22780 bytes
      .bss  size:   68552 bytes

Used static IRAM:  125322 bytes (   5750 remain, 95.6% used)
      .text size:  124295 bytes
   .vectors size:    1027 bytes

Used Flash size : 1214831 bytes
           .text:  992099 bytes
         .rodata:  222476 bytes
Total image size: 1362933 bytes (.bin may be padded larger)
```

</details>

#### 6. Configuring logging event buffer

Reducing the critical buffer size from default 4K to 1K would save 3K DRAM

```
CONFIG_EVENT_LOGGING_CRIT_BUFFER_SIZE=1024
```

#### 7. References for futher optimizations

- [RAM optimization](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/ram-usage.html)
- [Binary size optimization](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/size.html)
- [Speed Optimization](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/speed.html)
- [ESP32 Memory Analysis — Case Study](https://blog.espressif.com/esp32-memory-analysis-case-study-eacc75fe5431)

- [Optimizing IRAM](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/performance/ram-usage.html#optimizing-iram-usage)
    can provide additional heap area but at the cost of execution speed. Relocating frequently-called functions
    from IRAM to flash may result in increased execution time
