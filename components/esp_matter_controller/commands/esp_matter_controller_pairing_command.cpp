// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <esp_check.h>
#include <esp_log.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_pairing_command.h>
#include <optional>

#include <app-common/zap-generated/cluster-enums.h>
#include <credentials/FabricTable.h>

static const char *TAG = "pairing_command";

using namespace chip;
using namespace chip::Controller;

namespace esp_matter {
namespace controller {

void pairing_command::OnPairingComplete(CHIP_ERROR err)
{
    if (err == CHIP_NO_ERROR) {
        ESP_LOGI(TAG, "PASE session establishment success");
    } else {
        ESP_LOGI(TAG, "PASE session establishment failure: Matter-%s", ErrorStr(err));
        auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
        controller_instance.get_commissioner()->RegisterPairingDelegate(nullptr);
    }
    if (m_callbacks.pase_callback) {
        m_callbacks.pase_callback(err);
    }
}

void pairing_command::OnCommissioningSuccess(chip::PeerId peerId)
{
    ESP_LOGI(TAG, "Commissioning success with node %" PRIX64 "-%" PRIX64, peerId.GetCompressedFabricId(),
             peerId.GetNodeId());
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    controller_instance.get_commissioner()->RegisterPairingDelegate(nullptr);
    if (m_callbacks.commissioning_success_callback) {
        auto fabric = controller_instance.get_commissioner()->GetFabricTable()->FindFabricWithCompressedId(
            peerId.GetCompressedFabricId());
        m_callbacks.commissioning_success_callback(ScopedNodeId(fabric->GetFabricIndex(), peerId.GetNodeId()));
    }
}

void pairing_command::OnCommissioningFailure(
    chip::PeerId peerId, CHIP_ERROR error, chip::Controller::CommissioningStage stageFailed,
    chip::Optional<chip::Credentials::AttestationVerificationResult> additionalErrorInfo)
{
    ESP_LOGI(TAG, "Commissioning failure with node %" PRIX64 "-%" PRIX64, peerId.GetCompressedFabricId(),
             peerId.GetNodeId());
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    controller_instance.get_commissioner()->RegisterPairingDelegate(nullptr);
    if (m_callbacks.commissioning_failure_callback) {
        auto fabric = controller_instance.get_commissioner()->GetFabricTable()->FindFabricWithCompressedId(
            peerId.GetCompressedFabricId());
        m_callbacks.commissioning_failure_callback(
            ScopedNodeId(fabric->GetFabricIndex(), peerId.GetNodeId()), error, stageFailed,
            additionalErrorInfo.HasValue() ? std::make_optional(additionalErrorInfo.Value()) : std::nullopt);
    }
    if (m_device_is_icd) {
        controller_instance.get_icd_client_storage().DeleteEntry(
            ScopedNodeId(peerId.GetNodeId(), controller_instance.get_fabric_index()));
    }
}

void pairing_command::OnICDRegistrationComplete(ScopedNodeId nodeId, uint32_t icdCounter)
{
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    NodeId commissioner_node_id = controller_instance.get_commissioner()->GetNodeId();
    app::ICDClientInfo clientInfo;
    clientInfo.check_in_node = ScopedNodeId(commissioner_node_id, nodeId.GetFabricIndex());
    clientInfo.peer_node = nodeId;
    clientInfo.monitored_subject = commissioner_node_id;
    clientInfo.start_icd_counter = icdCounter;
    CHIP_ERROR err = controller_instance.get_icd_client_storage().SetKey(clientInfo, m_icd_symmetric_key);
    if (err == CHIP_NO_ERROR) {
        err = controller_instance.get_icd_client_storage().StoreEntry(clientInfo);
    }
    if (err != CHIP_NO_ERROR) {
        controller_instance.get_icd_client_storage().RemoveKey(clientInfo);
        return;
    }
    m_device_is_icd = true;
}

void pairing_command::OnICDStayActiveComplete(ScopedNodeId deviceId, uint32_t promisedActiveDuration)
{
    ESP_LOGI(TAG, "ICD Stay Active Complete for device " ChipLogFormatX64 " / promisedActiveDuration: %u",
             ChipLogValueX64(deviceId.GetNodeId()), promisedActiveDuration);
}

void pairing_command::OnDiscoveredDevice(const Dnssd::CommissionNodeData &nodeData)
{
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    // Ignore nodes with closed comissioning window
    VerifyOrReturn(nodeData.commissioningMode != 0);
    const uint16_t port = nodeData.port;
    char buf[Inet::IPAddress::kMaxStringLength];
    nodeData.ipAddress[0].ToString(buf);
    ESP_LOGI(TAG, "Discovered Device: %s:%u", buf, port);

    // Stop Mdns discovery. TODO: Check whether it is a right method
    controller_instance.get_commissioner()->RegisterDeviceDiscoveryDelegate(nullptr);

    Inet::InterfaceId interfaceId =
        nodeData.ipAddress[0].IsIPv6LinkLocal() ? nodeData.interfaceId : Inet::InterfaceId::Null();
    PeerAddress peerAddress = PeerAddress::UDP(nodeData.ipAddress[0], port, interfaceId);
    RendezvousParameters params = RendezvousParameters().SetSetupPINCode(m_setup_pincode).SetPeerAddress(peerAddress);
    CommissioningParameters commissioning_params = CommissioningParameters();
    NodeId commissioner_node_id = controller_instance.get_commissioner()->GetNodeId();
    if (m_icd_registration) {
        m_device_is_icd = false;
        commissioning_params.SetICDRegistrationStrategy(m_icd_registration_strategy)
            .SetICDClientType(app::Clusters::IcdManagement::ClientTypeEnum::kPermanent)
            .SetICDCheckInNodeId(commissioner_node_id)
            .SetICDMonitoredSubject(commissioner_node_id)
            .SetICDSymmetricKey(m_icd_symmetric_key);
    }
    controller_instance.get_commissioner()->PairDevice(m_remote_node_id, params, commissioning_params);
}

esp_err_t pairing_command::pairing_on_network(NodeId node_id, uint32_t pincode)
{
    Dnssd::DiscoveryFilter filter(Dnssd::DiscoveryFilterType::kNone);
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    ESP_RETURN_ON_FALSE(controller_instance.get_commissioner()->GetPairingDelegate() == nullptr, ESP_ERR_INVALID_STATE,
                        TAG, "There is already a pairing process");
    controller_instance.get_commissioner()->RegisterDeviceDiscoveryDelegate(&pairing_command::get_instance());
    controller_instance.get_commissioner()->RegisterPairingDelegate(&pairing_command::get_instance());
    pairing_command::get_instance().m_setup_pincode = pincode;
    pairing_command::get_instance().m_remote_node_id = node_id;
    if (CHIP_NO_ERROR != controller_instance.get_commissioner()->DiscoverCommissionableNodes(filter)) {
        ESP_LOGE(TAG, "Failed to discover commissionable nodes");
        return ESP_FAIL;
    }
    return ESP_OK;
}

#if CONFIG_ENABLE_ESP32_BLE_CONTROLLER
esp_err_t pairing_command::pairing_ble_wifi(NodeId node_id, uint32_t pincode, uint16_t disc, const char *ssid,
                                            const char *pwd)
{
    RendezvousParameters params = RendezvousParameters().SetSetupPINCode(pincode).SetDiscriminator(disc).SetPeerAddress(
        Transport::PeerAddress::BLE());
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    ESP_RETURN_ON_FALSE(controller_instance.get_commissioner()->GetPairingDelegate() == nullptr, ESP_ERR_INVALID_STATE,
                        TAG, "There is already a pairing process");
    controller_instance.get_commissioner()->RegisterPairingDelegate(&pairing_command::get_instance());

    ByteSpan nameSpan(reinterpret_cast<const uint8_t *>(ssid), strlen(ssid));
    ByteSpan pwdSpan(reinterpret_cast<const uint8_t *>(pwd), strlen(pwd));
    CommissioningParameters commissioning_params =
        CommissioningParameters().SetWiFiCredentials(Controller::WiFiCredentials(nameSpan, pwdSpan));
    controller_instance.get_commissioner()->PairDevice(node_id, params, commissioning_params);
    return ESP_OK;
}

esp_err_t pairing_command::pairing_ble_thread(NodeId node_id, uint32_t pincode, uint16_t disc, uint8_t *dataset_tlvs,
                                              uint8_t dataset_len)
{
    RendezvousParameters params = RendezvousParameters().SetSetupPINCode(pincode).SetDiscriminator(disc).SetPeerAddress(
        Transport::PeerAddress::BLE());
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    ESP_RETURN_ON_FALSE(controller_instance.get_commissioner()->GetPairingDelegate() == nullptr, ESP_ERR_INVALID_STATE,
                        TAG, "There is already a pairing process");
    controller_instance.get_commissioner()->RegisterPairingDelegate(&pairing_command::get_instance());
    ByteSpan dataset_span(dataset_tlvs, dataset_len);
    CommissioningParameters commissioning_params = CommissioningParameters().SetThreadOperationalDataset(dataset_span);
    NodeId commissioner_node_id = controller_instance.get_commissioner()->GetNodeId();
    if (pairing_command::get_instance().m_icd_registration) {
        pairing_command::get_instance().m_device_is_icd = false;
        commissioning_params.SetICDRegistrationStrategy(pairing_command::get_instance().m_icd_registration_strategy)
            .SetICDClientType(app::Clusters::IcdManagement::ClientTypeEnum::kPermanent)
            .SetICDCheckInNodeId(commissioner_node_id)
            .SetICDMonitoredSubject(commissioner_node_id)
            .SetICDSymmetricKey(pairing_command::get_instance().m_icd_symmetric_key);
    }
    controller_instance.get_commissioner()->PairDevice(node_id, params, commissioning_params);
    return ESP_OK;
}
#endif

esp_err_t pairing_command::pairing_code(NodeId nodeId, const char *payload)
{
    CommissioningParameters commissioning_params = CommissioningParameters();
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    NodeId commissioner_node_id = controller_instance.get_commissioner()->GetNodeId();
    ESP_RETURN_ON_FALSE(controller_instance.get_commissioner()->GetPairingDelegate() == nullptr, ESP_ERR_INVALID_STATE,
                        TAG, "There is already a pairing process");
    if (pairing_command::get_instance().m_icd_registration) {
        pairing_command::get_instance().m_device_is_icd = false;
        commissioning_params.SetICDRegistrationStrategy(pairing_command::get_instance().m_icd_registration_strategy)
            .SetICDClientType(app::Clusters::IcdManagement::ClientTypeEnum::kPermanent)
            .SetICDCheckInNodeId(commissioner_node_id)
            .SetICDMonitoredSubject(commissioner_node_id)
            .SetICDSymmetricKey(pairing_command::get_instance().m_icd_symmetric_key);
    }
    controller_instance.get_commissioner()->RegisterPairingDelegate(&pairing_command::get_instance());
    controller_instance.get_commissioner()->PairDevice(nodeId, payload, commissioning_params,
                                                       DiscoveryType::kDiscoveryNetworkOnly);
    return ESP_OK;
}

esp_err_t pairing_command::pairing_code_thread(NodeId nodeId, const char *payload, uint8_t *dataset_buf,
                                               uint8_t dataset_len)
{
    ByteSpan dataset_span(dataset_buf, dataset_len);

    CommissioningParameters commissioning_params = CommissioningParameters().SetThreadOperationalDataset(dataset_span);
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    ESP_RETURN_ON_FALSE(controller_instance.get_commissioner()->GetPairingDelegate() == nullptr, ESP_ERR_INVALID_STATE,
                        TAG, "There is already a pairing process");
    NodeId commissioner_node_id = controller_instance.get_commissioner()->GetNodeId();
    if (pairing_command::get_instance().m_icd_registration) {
        pairing_command::get_instance().m_device_is_icd = false;
        commissioning_params.SetICDRegistrationStrategy(pairing_command::get_instance().m_icd_registration_strategy)
            .SetICDClientType(app::Clusters::IcdManagement::ClientTypeEnum::kPermanent)
            .SetICDCheckInNodeId(commissioner_node_id)
            .SetICDMonitoredSubject(commissioner_node_id)
            .SetICDSymmetricKey(pairing_command::get_instance().m_icd_symmetric_key);
    }
    controller_instance.get_commissioner()->RegisterPairingDelegate(&pairing_command::get_instance());
    controller_instance.get_commissioner()->PairDevice(nodeId, payload, commissioning_params, DiscoveryType::kAll);

    return ESP_OK;
}

esp_err_t pairing_command::pairing_code_wifi(NodeId nodeId, const char *ssid, const char *password, const char *payload)
{
    ByteSpan nameSpan(reinterpret_cast<const uint8_t *>(ssid), strlen(ssid));
    ByteSpan pwdSpan(reinterpret_cast<const uint8_t *>(password), strlen(password));

    CommissioningParameters commissioning_params =
        CommissioningParameters().SetWiFiCredentials(Controller::WiFiCredentials(nameSpan, pwdSpan));
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    ESP_RETURN_ON_FALSE(controller_instance.get_commissioner()->GetPairingDelegate() == nullptr, ESP_ERR_INVALID_STATE,
                        TAG, "There is already a pairing process");
    controller_instance.get_commissioner()->RegisterPairingDelegate(&pairing_command::get_instance());
    controller_instance.get_commissioner()->PairDevice(nodeId, payload, commissioning_params, DiscoveryType::kAll);

    return ESP_OK;
}

esp_err_t pairing_command::pairing_code_wifi_thread(NodeId nodeId, const char *ssid, const char *password,
                                                    const char *payload, uint8_t *dataset_buf, uint8_t dataset_len)
{
    ByteSpan nameSpan(reinterpret_cast<const uint8_t *>(ssid), strlen(ssid));
    ByteSpan pwdSpan(reinterpret_cast<const uint8_t *>(password), strlen(password));
    ByteSpan dataset_span(dataset_buf, dataset_len);

    CommissioningParameters commissioning_params =
        CommissioningParameters()
            .SetWiFiCredentials(Controller::WiFiCredentials(nameSpan, pwdSpan))
            .SetThreadOperationalDataset(dataset_span);
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    ESP_RETURN_ON_FALSE(controller_instance.get_commissioner()->GetPairingDelegate() == nullptr, ESP_ERR_INVALID_STATE,
                        TAG, "There is already a pairing process");
    NodeId commissioner_node_id = controller_instance.get_commissioner()->GetNodeId();
    if (pairing_command::get_instance().m_icd_registration) {
        pairing_command::get_instance().m_device_is_icd = false;
        commissioning_params.SetICDRegistrationStrategy(pairing_command::get_instance().m_icd_registration_strategy)
            .SetICDClientType(app::Clusters::IcdManagement::ClientTypeEnum::kPermanent)
            .SetICDCheckInNodeId(commissioner_node_id)
            .SetICDMonitoredSubject(commissioner_node_id)
            .SetICDSymmetricKey(pairing_command::get_instance().m_icd_symmetric_key);
    }
    controller_instance.get_commissioner()->RegisterPairingDelegate(&pairing_command::get_instance());
    controller_instance.get_commissioner()->PairDevice(nodeId, payload, commissioning_params, DiscoveryType::kAll);

    return ESP_OK;
}

static void remove_fabric_handler(NodeId remote_node, CHIP_ERROR status)
{
    if (status == CHIP_NO_ERROR) {
        ESP_LOGI(TAG, "Succeeded to remove fabric for remote node 0x%" PRIx64, remote_node);
    } else {
        ESP_LOGE(TAG, "Failed to remove fabric for remote node 0x%" PRIx64, remote_node);
    }
}

esp_err_t pairing_command::unpair_device(NodeId node_id)
{
    auto &controller_instance = esp_matter::controller::matter_controller_client::get_instance();
    return controller_instance.unpair(node_id, remove_fabric_handler);
}

} // namespace controller
} // namespace esp_matter
