---
name: Report Certification Failure
about: Create a bug report for Matter device certification failure
title: '[CERT-FAILURE] '
labels: 'certification'
---

**Describe the bug**
Please describe the certification failure in detail.

**Requested details**
Please provide the following information:
- Matter version:
- ESP-Matter Commit Id:
- ESP-IDF Commit Id:
- SoC (eg: ESP32 or ESP32-C3):
- sdkconfig:
- Partition table:
- Endpoint Hierarchy:

- PICS files:
- Test Harness Version:
- Test Harness SHA:
- Test Harness SDK SHA:

- Device Logs with Debug Log Level (Please attach the log file):
- Test harness logs:

**Checklist**
- [ ] I have gone through the [Github issues](https://github.com/espressif/esp-matter/issues), and the [Matter Programming guide](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/) and did not find a similar issue.
- [ ] Verify the PICS items must match with the cluster configuration on your DUT.
- [ ] Also verify that PICS items are correct as per the test plan
- [ ] I have ran the test locally and verified that the issue is reproducible. (You can run the tests locally by following instructions listed here: [Python tests](https://github.com/project-chip/connectedhomeip/blob/master/docs/testing/python.md#running-tests-locally) and [YAML tests](https://github.com/project-chip/connectedhomeip/blob/master/docs/testing/yaml.md#running-yaml-tests))
