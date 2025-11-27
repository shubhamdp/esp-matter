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
- Append the component name to the TEST_COMPONENTS list. For example, if you add a new component called "new_component", you need to add it to the TEST_COMPONENTS list in CMakeLists.txt:

```cmake
set(TEST_COMPONENTS "esp_matter new_component" CACHE STRING "List of components to test")
```