# ESP Matter Unit Test App

This application runs unit tests for the ESP Matter component using the Unity test framework.

## Unity Test Framework

This test app uses the Unity Test Framework suggested by ESP-IDF.

Further reads:
- [Unit Testing with Unity](https://docs.espressif.com/projects/vscode-esp-idf-extension/en/latest/additionalfeatures/unit-testing.html)
- [Unit Testing in ESP32](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/unit-tests.html)

## Running the Tests

1. Build the application:
```bash
cd examples/unit_test_app
idf.py build
```

2. Flash to device:
```bash
idf.py -p <PORT> flash monitor
```

3. Run tests:
Once flashed, the test menu will appear in the serial monitor. You can:
- Press `Enter` to see the list of available tests
- Enter a test number to run a specific test
- Enter `*` to run all tests

## Running Tests with QEMU (no hardware needed)

You can run the unit tests locally under QEMU emulation without physical hardware. This is the same method used in CI.

### Prerequisites

- Install QEMU for RISC-V (esp32c3):
```bash
python3 -m pip install pytest-embedded-qemu
```

- Ensure the QEMU binary (`qemu-system-riscv32`) is available. Install it via ESP-IDF tools:
```bash
$IDF_PATH/tools/idf_tools.py install qemu-riscv32
source $IDF_PATH/export.sh  # re-source to pick up the new tool in PATH
```

### Build and Run

```bash
cd examples/unit_test_app
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.qemu" set-target esp32c3 build

# Run all QEMU test groups (each gets a fresh QEMU reboot)
pytest pytest_unit_test_app.py \
    --target esp32c3 \
    -m qemu \
    --embedded-services idf,qemu \
    --qemu-extra-args="-global driver=timer.esp32c3.timg,property=wdt_disable,value=true"

# Run a single test group
pytest pytest_unit_test_app.py \
    --target esp32c3 \
    -m qemu \
    --embedded-services idf,qemu \
    --qemu-extra-args="-global driver=timer.esp32c3.timg,property=wdt_disable,value=true" \
    -k test_get_val
```

### Why multiple test functions?

Each test file has its own `setup_*()` function that calls `esp_matter::start()`, and there is no teardown/stop.
Since only one setup can succeed per boot, tests are grouped so each group runs after a fresh QEMU reboot.
Each pytest function (eg: `test_get_val`, `test_get_val_type`, `test_update_report`) gets its own QEMU instance.

## Running Tests with esp-emu (no hardware needed)

Same tests can also run under [esp-emu](https://github.com/espressif/esp-emulator), Espressif's lightweight RISC-V emulator. `conftest.py` spawns `esp-emu` per test with its UART served over TCP, and pytest-embedded connects via `socket://`.

### Prerequisites

Install esp-emu:
```bash
curl -fsSL https://raw.githubusercontent.com/espressif/esp-emulator/main/install.sh | sh
```

### Build and Run

```bash
cd examples/unit_test_app
idf.py set-target esp32c3 build merge-bin   # merge-bin produces build/merged-binary.bin

pytest pytest_unit_test_app.py \
    --target esp32c3 \
    -m emu \
    --embedded-services idf,serial \
    --port socket://127.0.0.1:5555
```

### Quick smoke test (no pytest)

esp-emu can drive the Unity menu by itself — handy to check a build boots and a group passes:

```bash
esp-emu --chip esp32c3 --firmware build/merged-binary.bin \
    --inject-on "Press ENTER to see the list of tests" --inject '[get_val]\n' \
    --exit-on "Enter next test" --timeout 120s | tee /tmp/unity.log
! grep -q ":FAIL" /tmp/unity.log
```

## Extending the Tests

### Adding tests to existing component

1. Create a new `.cpp` file in `components/<component_name>/test/`
2. Add the filename to the source list in `components/<component_name>/test/CMakeLists.txt`:
```cmake
list(APPEND srcs_list "your_new_test_file.cpp")
```
3. Write the test cases in the new test file.

### Adding new component tests

Please refer to components/esp_matter/test directory for comprehensive structure and example.

- After adding the new component tests, you need to add the component to the TEST_COMPONENTS list in CMakeLists.txt
- Append the component name to the TEST_COMPONENTS list. For example, if you add a new component called "new_component",
you need to add it to the TEST_COMPONENTS list in CMakeLists.txt:

```cmake
set(TEST_COMPONENTS "esp_matter new_component" CACHE STRING "List of components to test")
```

### For running them in the CI,
- Add the test group to the `pytest_unit_test_app.py` file with the appropriate marker and test function name.

```python
@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.emu
@pytest.mark.esp32c3
def test_my_unit_tests(dut: Dut) -> None:
    run_group(dut, 'my_test_group')
```
