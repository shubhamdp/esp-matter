# Security Audit Report: esp-matter Repository

**Date:** February 19, 2026
**Branch:** `cursor/codebase-security-audit-84b1`

## Summary

This is an ESP-IDF SDK component for building Matter (formerly CHIP/Project Connected Home over IP) devices on Espressif chips. The codebase consists of core Matter libraries, controller components, OTA provider, bridge functionality, console utilities, RainMaker integration, CI/CD pipelines, examples, and Docker infrastructure.

**Total Findings: 20**
- Critical: 5
- High: 6
- Medium: 4
- Low / Informational: 5

---

## CRITICAL Findings

### 1. Sensitive Cryptographic Keys Logged to Console

**File:** `components/esp_matter_controller/core/esp_matter_controller_icd_client.cpp` (lines 49–54)

The `list_registered_icd()` function logs ICD (Intermittently Connected Device) symmetric AES and HMAC keys in hex to the console via `ESP_LOGI`. These are live session security keys, and logging them creates a side-channel for key extraction by anyone with console/UART access.

```cpp
ESP_LOGI(TAG,"  | aes key:  %60s                               |", icd_aes_key_hex);
// ...
ESP_LOGI(TAG,"  | hmac key: %60s                               |", icd_hmac_key_hex);
```

**Impact:** An attacker with access to the serial console or log output can recover live symmetric encryption and authentication keys, compromising all communication with ICD devices.

---

### 2. Secret Key and User ID Logged in Plaintext

**File:** `components/esp_matter_rainmaker/esp_matter_rainmaker.cpp` (line 289)

The RainMaker user node association secret key is logged in plaintext via `ESP_LOGI`:

```cpp
ESP_LOGI(TAG, "user_id: %s, secret_key: %s", user_id, secret_key);
```

**Impact:** The secret key used for RainMaker user-node mapping is exposed to anyone with serial/log access.

---

### 3. Wi-Fi Password Logged in Plaintext During Provisioning

**File:** `examples/light_network_prov/main/app_network.cpp` (lines 148–150)

During Wi-Fi credential provisioning, the SSID and password are logged:

```cpp
ESP_LOGI(TAG,
         "Received Wi-Fi credentials"
         "\n\tSSID     : %s\n\tPassword : %s",
         (const char *)wifi_sta_cfg->ssid, (const char *)wifi_sta_cfg->password);
```

**Impact:** Wi-Fi credentials are exposed on the serial console/log output.

---

### 4. Refresh Token Logged During RainMaker Bridge API

**File:** `examples/bridge_apps/esp_rainmaker_bridge/main/rainmaker_bridge.cpp` (line 473)

The user token (a refresh token used for cloud API authentication) is logged:

```cpp
ESP_LOGI(TAG, "Set user token: %s\n", val.val.s);
```

**Impact:** Authentication tokens for the RainMaker cloud API are exposed on the serial output.

---

### 5. RainMaker API Logs POST Data Including Refresh Tokens

**File:** `examples/bridge_apps/esp_rainmaker_bridge/example_components/rainmaker_api/rainmaker_api.cpp` (line 113)

The HTTP POST data (which includes the refresh token JSON payload) is logged:

```cpp
ESP_LOGI(TAG, "Sending data: %s", post_data);
```

**Impact:** Refresh tokens and other sensitive API payloads are exposed in logs.

---

## HIGH Findings

### 6. SSH StrictHostKeyChecking Disabled in CI/CD

**File:** `.gitlab-ci.yml` (line 40)

```yaml
echo -e "Host gitlab.espressif.cn\n\tStrictHostKeyChecking no\n" >> ~/.ssh/config
```

**File:** `docs/utils.sh` (line 17)

```bash
echo -e "Host ${server_url}\n\tStrictHostKeyChecking no\n\tUser ${server_user}\n" >>~/.ssh/config
```

**Impact:** Disabling SSH host key verification makes CI/CD connections susceptible to man-in-the-middle attacks. An attacker who can intercept network traffic to `gitlab.espressif.cn` or the docs server could substitute their own server and intercept credentials, code, or inject malicious content.

---

### 7. Default Test Credentials Used Throughout Examples and Tests

Multiple files use the hardcoded default Matter test setup PIN code `20202021` and discriminator `3840`:

- `examples/pytest_esp_matter_light.py` (lines 47, 86, 125, 259, 289)
- `examples/all_device_types_app/README.md` (line 36)
- `examples/generic_switch/README.md` (lines 29, 32)
- `examples/sensors/README.md` (line 44)
- `examples/thread_border_router/README.md` (line 55)
- `examples/controller/README.md` (line 75)
- `examples/bridge_apps/esp_rainmaker_bridge/README.md` (line 101)
- `tools/launchpad/qrcode-content.md` (line 6)

While these are well-known Matter test values, they are used in launchpad firmware images deployed to GitHub Pages (the `pages.yml` workflow builds and deploys firmware with these defaults). Firmware served via the launchpad is potentially used by end users.

**Impact:** Devices flashed with launchpad firmware use known, static test credentials, allowing any nearby attacker to commission the device.

---

### 8. Test Attestation and Credentials Defaults Enabled by Default

**File:** `components/esp_matter_controller/Kconfig` (lines 26–69)

Both the Attestation Trust Store and Operational Credentials Issuer default to "Test" implementations:

```
default TEST_ATTESTATION_TRUST_STORE
...
config TEST_ATTESTATION_TRUST_STORE
    bool "Attestation Trust Store - Test"
    help
        Use the two hardcoded PAA certificates(Chip-Test-PAA-FFF1-Cert&Chip-Test-PAA-NoVID-Cert) in the firmware.
```

```
default TEST_OPERATIONAL_CREDS_ISSUER
...
config TEST_OPERATIONAL_CREDS_ISSUER
    bool "Operational Credentials Issuer - Test"
```

Similarly, in `components/esp_matter/Kconfig`, `EXAMPLE_DAC_PROVIDER` (Test DAC), `EXAMPLE_COMMISSIONABLE_DATA_PROVIDER` (Test commissionable data), and `EXAMPLE_DEVICE_INSTANCE_INFO_PROVIDER` (Test instance info) are all defaults when factory data providers are not configured.

**Impact:** If a product ships without explicitly configuring production credentials, it will use test attestation data that any party can forge, undermining the entire Matter security model.

---

### 9. Buffer Overflow in Console Command Handler (esp-now bridge)

**File:** `examples/bridge_apps/esp-now_bridge_light/main/app_driver.cpp` (lines 31–53)

```cpp
static char console_buffer[101] = {0};
// ...
console_buffer[0] = argc - 4;
for (int i = 0; i < (argc - 4); i++) {
    // ...
    strcpy((console_buffer + 1 + 10*i), &argv[4+i][2]);
}
```

The buffer is 101 bytes. Each argument copies up to 10 characters at offset `1 + 10*i`. If `argc - 4 >= 11`, the write starts at offset 111, which is out of bounds. The `argc` value is not bounded.

The same pattern repeats for the `invoke-group` and `client invoke` commands at lines 91–97 and 114–121.

**Impact:** A user providing more than 10 hex parameters via the console can cause a stack or heap buffer overflow, potentially enabling code execution.

---

### 10. `strncpy` Used Without Null-Termination Guarantee in Path Handling

**File:** `components/esp_matter_controller/attestation_store/esp_matter_attestation_trust_store.cpp` (lines 42–43)

```cpp
strncpy(m_path, path, strnlen(path, 16));
m_path[15] = 0;
```

The `m_path` buffer is 16 bytes. `strncpy` with `strnlen(path, 16)` copies up to 16 bytes, but `strncpy` does not null-terminate if the source is longer. Line 43 forces null-termination at index 15, which works but truncates the path silently. A path longer than 15 chars would silently become the wrong directory without any error.

---

### 11. Potential NULL Pointer Dereference in OTA Image URL Parsing

**File:** `components/esp_matter_ota_provider/src/esp_matter_ota_provider.cpp` (lines 119–120)

```cpp
char *ptr = strrchr(requestor->mOtaImageUrl, '/');
strncpy(otaFileName, ptr + 1, strnlen(ptr + 1, 255));
```

If `mOtaImageUrl` does not contain `/`, `ptr` will be NULL and `ptr + 1` will cause a null pointer dereference / crash.

---

## MEDIUM Findings

### 12. Docker Auth Config Written to Disk in CI

**File:** `.gitlab-ci.yml` (lines 168–169)

```yaml
- mkdir -p $HOME/.docker
- echo $DOCKER_AUTH_CONFIG > $HOME/.docker/config.json
```

The Docker authentication configuration (containing registry credentials) is written to a file. If the CI runner is shared or not properly cleaned up, residual credentials could be exposed.

---

### 13. `format_all.sh` and `check_typos.sh` Process User-Supplied Filenames Without Sanitization

**File:** `tools/check_typos.sh`

The script accepts file paths from `git diff` output and passes them directly to `find`, `cat`, and Python without sanitization. While exploitation requires control over the git repository, this represents a minor code injection risk in CI pipelines if a malicious branch name or file path is crafted.

---

### 14. NVS Data Stored Unencrypted by Default

**File:** `components/esp_matter/esp_matter_core.cpp` (lines 418–433)

NVS encryption is only enabled when `CONFIG_NVS_ENCRYPTION` is defined:

```cpp
#ifdef CONFIG_NVS_ENCRYPTION
    // ... encrypted init ...
    return nvs_flash_secure_init_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, &cfg);
#else
    return nvs_flash_init_partition(CONFIG_ESP_MATTER_NVS_PART_NAME);
#endif
```

By default, NVS encryption is not enabled, meaning all persistent attribute data (which could include sensitive device state) is stored in plaintext on the flash.

---

### 15. Tokens Stored in Memory as `std::string` Without Secure Erasure

**File:** `examples/bridge_apps/esp_rainmaker_bridge/example_components/rainmaker_api/rainmaker_api.h` (lines 347–348)

```cpp
std::string access_token_;
std::string refresh_token_;
```

Access and refresh tokens are stored as `std::string` objects. When `clear()` is called, the memory is not securely zeroed (the `std::string` allocator may leave the old data in the freed heap). An attacker with memory read access (e.g., via a debug port or memory dump) could recover expired tokens.

---

## LOW / INFORMATIONAL Findings

### 16. Hardcoded Thread Dataset in Test Script

**File:** `examples/pytest_esp_matter_light.py` (line 23)

A full OpenThread network dataset (including the network key) is hardcoded in the test file. This is appropriate for a test environment but should never be used in production.

---

### 17. `snprintf` Format String Mismatch

**File:** `components/esp_matter_ota_provider/src/esp_matter_ota_candidates.cpp` (line 225)

```cpp
snprintf(url, sizeof(url), "%s/%d/%d/%ld", dcl_rest_url, model->vendor_id, model->product_id, new_software_version);
```

`new_software_version` is `uint32_t` but is formatted with `%ld` (long int). On 32-bit platforms this works, but on 64-bit it could cause truncation or undefined behavior. Should use `PRIu32` or `%u`.

---

### 18. GitHub Actions Workflows Use `actions/checkout@master`

**Files:** `.github/workflows/issue_comment.yml`, `new_issues.yml`, `new_prs.yml`

These workflows reference `actions/checkout@master` which is a mutable reference. It is best practice to pin to a specific commit SHA or version tag to prevent supply chain attacks.

---

### 19. Dockerfile Uses Mutable Base Image Tags

**File:** `tools/docker/chip_idf/Dockerfile` (line 2)

```dockerfile
ARG VERSION=latest
FROM ghcr.io/project-chip/chip-build:${VERSION} as build
```

Using `latest` as the default tag means builds are not reproducible and could silently pick up a compromised or incompatible base image.

---

### 20. `strcat` Usage in Console OT CLI

**File:** `components/esp_matter_console/esp_matter_console_otcli.cpp` (lines 46–48)

The function uses `strcat` to build a CLI string. While the buffer appears to be sized adequately for the expected use case, `strcat` is inherently unsafe and should be replaced with bounds-checked alternatives.

---

## Positive Security Observations

1. **TLS Certificate Validation**: All HTTP connections to DCL endpoints and RainMaker APIs correctly use `esp_crt_bundle_attach` for TLS certificate verification, and `skip_cert_common_name_check` is set to `false`.

2. **OTA Security**: The OTA Provider uses BDX (Bulk Data Transfer) protocol with proper token generation using `chip::Crypto::GetRandU8()`, and the OTA requestor supports encrypted OTA via `esp_matter_ota_requestor_encrypted_init()`.

3. **Secure Boot and Flash Encryption Support**: The `mfg_test_app` example validates secure boot and flash encryption status, showing awareness of platform security features.

4. **NVS Encryption Support**: While not default, NVS encryption is properly implemented when enabled.

5. **Factory Data Providers**: The SDK provides proper production-grade DAC, commissionable data, and device info providers backed by factory partition or secure certificate storage.

6. **Proper Use of `snprintf`**: Most string operations use `snprintf` with proper buffer size limits.

7. **CI Secrets Management**: GitHub Actions workflows properly use `${{ secrets.* }}` for sensitive values and the GitLab CI reads tokens from environment variables rather than hardcoding them.

---

## Recommendations

1. **Remove all plaintext logging of secrets**: ICD keys, RainMaker secret keys, tokens, and Wi-Fi passwords must never be logged, even at debug level. Replace with masked or redacted logging.

2. **Enable SSH host key checking in CI**: Use known host keys or at minimum `StrictHostKeyChecking=accept-new` rather than `no`.

3. **Fix buffer overflow in esp-now bridge console**: Add bounds checking on `argc` to ensure writes stay within `console_buffer`.

4. **Change Kconfig defaults**: Consider making the test/example credential providers explicitly opt-in rather than default, or add compile-time warnings when test providers are selected.

5. **Pin GitHub Actions to commit SHAs**: Replace `@master` with specific version tags or commit SHAs.

6. **Pin Docker base image tags**: Use digest-pinned base images for reproducible builds.

7. **Implement secure memory erasure for tokens**: Use `memset_s` or platform-specific secure zeroing for sensitive strings before deallocation.

8. **Enable NVS encryption by default** or at minimum add a build-time warning when it is disabled on production builds.

9. **Add NULL check for `strrchr` return** in OTA provider URL parsing before dereferencing.

10. **Replace `strcpy`/`strcat`** with bounds-checked alternatives (`strlcpy`, `snprintf`) in console command handlers.
