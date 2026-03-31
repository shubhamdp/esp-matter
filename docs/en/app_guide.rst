Application User Guide
======================

Delegate Implementation
-----------------------

As per the implementation in the connectedhomeip repository, some of the clusters
require an application defined delegate to consume specific data and actions.
In order to provide this flexibity to the application, esp-matter facilitates delegate
initialization callbacks in the cluster create API. It is expected that application
will define it's data and actions in the form of delegate-impl class and set the
delegate while creating cluster/device type.

List of clusters with delegate:

.. keep-sorted start

- Account Login Cluster.
- Actions Cluster.
- Application Basic Cluster.
- Application Launcher Cluster.
- Audio Output Cluster.
- Boolean State Configuration Cluster.
- Camera AV Settings User Level Management Cluster.
- Channel Cluster.
- Chime Cluster.
- Closure Control Cluster.
- Closure Dimension Cluster.
- Commissioner Control Cluster.
- Commodity Price Cluster.
- Commodity Tariff Cluster.
- Content App Observer Cluster.
- Content Control Cluster.
- Content Launcher Cluster.
- Device Energy Management Cluster.
- Dishwasher Alarm Cluster.
- Door Lock Cluster.
- Electrical Grid Conditions Cluster.
- Electrical Power Measurement Cluster.
- Energy EVSE Cluster.
- Energy Preference Cluster.
- Fan Control Cluster.
- Keypad Input Cluster.
- Laundry Dryer Controls Cluster.
- Laundry Washer Controls Cluster.
- Low Power Cluster.
- Media Input Cluster.
- Media Playback Cluster.
- Messages Cluster.
- Microwave Oven Control Cluster.
- Mode Base Cluster (all derived types of clusters).
- Mode Select Cluster.
- Operational State Cluster.
- Power Topology Cluster.
- Push AV Stream Transport Cluster.
- Resource Monitoring Cluster.
- Service Area Cluster.
- Target Navigator Cluster.
- Thermostat Cluster.
- Time Synchronization Cluster.
- Valve Configuration And Control Cluster.
- Wake On Lan Cluster.
- Water Heater Management Cluster.
- WebRTC Transport Provider Cluster.
- Window Covering Cluster.

.. keep-sorted end

Below is the list of clusters with delegate and their reference implementation header files:

.. keep-sorted start block=yes

.. {

Account Login Cluster
~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Account Login`_, `Account Login Delegate`_
.. }
.. {

Actions Cluster
~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Actions`_, None
.. }
.. {

Application Basic Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Application Basic`_, `Application Basic Delegate`_
.. }
.. {

Application Launcher Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Application Launcher`_, `Application Launcher Delegate`_
.. }
.. {

Audio Output Cluster
~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Audio Output`_, `Audio Output Delegate`_
.. }
.. {

Boolean State Configuration Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Boolean State Configuration`_, `Boolean State Configuration Delegate`_
.. }
.. {

Camera AV Settings User Level Management Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Camera AV Settings User Level Management`_, `Camera AV Settings User Level Management Delegate`_
.. }
.. {

Channel Cluster
~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Channel`_, `Channel Delegate`_
.. }
.. {

Chime Cluster
~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Chime`_, `Chime Delegate`_
.. }
.. {

Closure Control Cluster
~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Closure Control`_, `Closure Control Delegate`_
.. }
.. {

Closure Dimension Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Closure Dimension`_, `Closure Dimension Delegate`_
.. }
.. {

Commissioner Control Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Commissioner Control`_, `Commissioner Control Delegate`_
.. }
.. {

Commodity Price Cluster
~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Commodity Price`_, `Commodity Price Delegate`_
.. }
.. {

Commodity Tariff Cluster
~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Commodity Tariff`_, `Commodity Tariff Delegate`_
.. }
.. {

Content App Observer Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Content App Observer`_, None
.. }
.. {

Content Control Cluster
~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Content Control`_, None
.. }
.. {

Content Launcher Cluster
~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Content Launcher`_, `Content Launcher Delegate`_
.. }
.. {

Device Energy Management Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Device Energy Management`_, `Device Energy Management Delegate`_
.. }
.. {

Dishwasher Alarm Cluster
~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Dishwasher Alarm`_, `Dishwasher Alarm Delegate`_
.. }
.. {

Door Lock Cluster
~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Door Lock`_, `Door Lock Delegate`_
.. }
.. {

Electrical Grid Conditions Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Electrical Grid Conditions`_, `Electrical Grid Conditions Delegate`_

.. note::
    Make sure that after implementing delegate class, you set the delegate class pointer at the time of creating cluster.

   ::

      robotic_vacuum_cleaner::config_t rvc_config;
      rvc_config.rvc_run_mode.delegate = object_of_delegate_class;
      endpoint_t *endpoint = robotic_vacuum_cleaner::create(node, & rvc_config, ENDPOINT_FLAG_NONE);
.. }
.. {

Electrical Power Measurement Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Electrical Power Measurement`_, `Electrical Power Measurement Delegate`_
.. }
.. {

Energy Evse Cluster
~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Energy Evse`_, `Energy Evse Delegate`_
.. }
.. {

Energy Preference Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Energy Preference`_, `Energy Preference Delegate`_
.. }
.. {

Fan Control Cluster
~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Fan Control`_, `Fan Control Delegate`_
.. }
.. {

Keypad Input Cluster
~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Keypad Input`_, `Keypad Input Delegate`_
.. }
.. {

Laundry Dryer Controls Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Laundry Dryer Controls`_, `Laundry Dryer Controls Delegate`_
.. }
.. {

Laundry Washer Controls Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Laundry Washer Controls`_, `Laundry Washer Controls Delegate`_
.. }
.. {

Low Power Cluster
~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Low Power`_, `Low Power Delegate`_
.. }
.. {

Media Input Cluster
~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Media Input`_, `Media Input Delegate`_
.. }
.. {

Media Playback Cluster
~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Media Playback`_, `Media Playback Delegate`_
.. }
.. {

Messages Cluster
~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Messages`_, `Messages Delegate`_
.. }
.. {

Microwave Oven Control Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Microwave Oven Control`_, `Microwave Oven Control Delegate`_
.. }
.. {

Mode Base Cluster
~~~~~~~~~~~~~~~~~

It is a base cluster for ModeEVSE, ModeOven, ModeRVSRun, ModeRVSClean, ModeDishwasher,
ModeWaterHeater, ModeRefrigerator, ModeLaundryWasher and ModeMicrowaveOven.

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Mode Base`_, `Refrigeratore And TCC Mode`_
              , `Laundry Washer Mode`_
              , `Dish Washer Mode`_
              , `Rvc Run And Rvc Clean Mode`_
              , `Energy Evse Mode`_
              , `Microwave Oven Mode`_
              , `Device Energy Management Mode`_
              , `Water Heater Mode`_
.. }
.. {

Mode Select Cluster
~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Mode Select`_, `Mode Select Delegate`_
.. }
.. {

Operational State Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Operational State`_, `Operational State Delegate`_
.. }
.. {

Power Topology Cluster
~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Power Topology`_, `Power Topology Delegate`_
.. }
.. {

Push AV Stream Transport Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Push AV Stream Transport`_, `Push AV Stream Transport Delegate`_
.. }
.. {

Resource Monitoring Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Resource Monitoring`_, `HEPA Filter Monitoring Delegate`_
                        , `Activated Carbon Filter Monitoring Delegate`_
                        , `Water Tank Level Monitoring Delegate`
.. }
.. {

Service Area Cluster
~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Service Area`_, `Service Area Delegate`_
.. }
.. {

Target Navigator Cluster
~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Target Navigator`_, `Target Navigator Delegate`_
.. }
.. {

Thermostat Cluster
~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"
  
  `Thermostat`_, `Thermostat Delegate`_
.. }
.. {

Time Synchronization Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Time Synchronization`_, `Time Synchronization Delegate`_
.. }
.. {

Valve Configuration And Control Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Valve Configuration And Control`_, `Valve Configuration And Control Delegate`_
.. }
.. {

Wake On LAN Cluster
~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Wake On LAN`_, `Wake On LAN Delegate`_
.. }
.. {

Water Heater Management Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Water Heater Management`_, `Water Heater Management Delegate`_
.. }
.. {

WebRTC Transport Provider Cluster
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `WebRTC Transport Provider`_, `WebRTC Transport Provider Delegate`_
.. }
.. {

Window Covering Cluster
~~~~~~~~~~~~~~~~~~~~~~~

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Window Covering`_, `Window Covering Delegate`_
.. }

.. keep-sorted end

.. keep-sorted start

.. _`Account Login Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/android/include/account-login/AccountLoginManager.h
.. _`Account Login`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/account-login-server/account-login-delegate.h
.. _`Actions Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/bridged-actions-stub.h
.. _`Actions`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/actions-server/actions-server.h
.. _`Activated Carbon Filter Monitoring Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/resource-monitoring-delegates.h
.. _`Application Basic Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/application-basic/ApplicationBasicManager.h
.. _`Application Basic`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/application-basic-server/application-basic-delegate.h
.. _`Application Launcher Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/application-launcher/ApplicationLauncherManager.h
.. _`Application Launcher`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/application-launcher-server/application-launcher-delegate.h
.. _`Audio Output Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/audio-output/AudioOutputManager.h
.. _`Audio Output`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/audio-output-server/audio-output-delegate.h
.. _`Boolean State Configuration Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/boolcfg-stub.cpp
.. _`Boolean State Configuration`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-delegate.h
.. _`Camera AV Settings User Level Management Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/camera-av-settings-user-level-management-instance.h
.. _`Camera AV Settings User Level Management`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/camera-av-settings-user-level-management-server/camera-av-settings-user-level-management-server.h
.. _`Channel Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/channel/ChannelManager.h
.. _`Channel`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/channel-server/channel-delegate.h
.. _`Chime Delegate`: https://github.com/espressif/connectedhomeip/blob/faf4d09/examples/all-clusters-app/all-clusters-common/include/chime-instance.h
.. _`Chime`: https://github.com/espressif/connectedhomeip/blob/faf4d09/src/app/clusters/chime-server/chime-server.h
.. _`Closure Control Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/closure-app/closure-common/include/ClosureControlEndpoint.h
.. _`Closure Control`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/closure-control-server/closure-control-cluster-delegate.h
.. _`Closure Dimension Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/closure-app/closure-common/include/ClosureDimensionEndpoint.h
.. _`Closure Dimension`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/closure-dimension-server/closure-dimension-delegate.h
.. _`Commissioner Control Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/fabric-bridge-app/linux/include/CommissionerControlDelegate.h
.. _`Commissioner Control`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/commissioner-control-server/commissioner-control-server.h
.. _`Commodity Price Delegate`: https://github.com/espressif/connectedhomeip/blob/faf4d09/examples/energy-gateway-app/commodity-price/include/CommodityPriceDelegate.h
.. _`Commodity Price`: https://github.com/espressif/connectedhomeip/blob/faf4d09/src/app/clusters/commodity-price-server/commodity-price-server.h
.. _`Commodity Tariff Delegate`: https://github.com/espressif/connectedhomeip/blob/faf4d09/examples/energy-gateway-app/commodity-tariff/include/CommodityTariffInstance.h
.. _`Commodity Tariff`: https://github.com/espressif/connectedhomeip/blob/faf4d09/src/app/clusters/commodity-tariff-server/commodity-tariff-server.h
.. _`Content App Observer`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/content-app-observer/content-app-observer-delegate.h
.. _`Content Control`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/content-control-server/content-control-delegate.h
.. _`Content Launcher Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/content-launcher/ContentLauncherManager.h
.. _`Content Launcher`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/content-launch-server/content-launch-delegate.h
.. _`Device Energy Management Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/device-energy-management/include/DeviceEnergyManagementDelegateImpl.h
.. _`Device Energy Management Mode`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/device-energy-management/include/device-energy-management-modes.h
.. _`Device Energy Management`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/device-energy-management-server/device-energy-management-server.h
.. _`Dish Washer Mode`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/dishwasher-mode.h
.. _`Dishwasher Alarm Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/dishwasher-alarm-stub.cpp
.. _`Dishwasher Alarm`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/dishwasher-alarm-server/dishwasher-alarm-delegate.h
.. _`Door Lock Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/lock-app/lock-common/src/LockEndpoint.cpp
.. _`Door Lock`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/door-lock-server/door-lock-delegate.h
.. _`Electrical Grid Conditions Delegate`: https://github.com/espressif/connectedhomeip/blob/faf4d09/examples/energy-gateway-app/electrical-grid-conditions/include/ElectricalGridConditionsDelegate.h
.. _`Electrical Grid Conditions`: https://github.com/espressif/connectedhomeip/blob/faf4d09/src/app/clusters/electrical-grid-conditions-server/electrical-grid-conditions-server.h
.. _`Electrical Power Measurement Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-reporting/include/ElectricalPowerMeasurementDelegate.h
.. _`Electrical Power Measurement`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/electrical-power-measurement-server/electrical-power-measurement-server.h
.. _`Energy Evse Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-evse/include/EnergyEvseDelegateImpl.h
.. _`Energy Evse Mode`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-evse/include/energy-evse-modes.h
.. _`Energy Evse`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/energy-evse-server/energy-evse-server.h
.. _`Energy Preference Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/energy-preference-delegate.cpp
.. _`Energy Preference`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/energy-preference-server/energy-preference-server.h
.. _`Fan Control Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/src/fan-stub.cpp
.. _`Fan Control`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/fan-control-server/fan-control-delegate.h
.. _`HEPA Filter Monitoring Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/resource-monitoring-delegates.h
.. _`Keypad Input Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/keypad-input/KeypadInputManager.h
.. _`Keypad Input`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/keypad-input-server/keypad-input-delegate.h
.. _`Laundry Dryer Controls Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/laundry-dryer-controls-delegate-impl.h
.. _`Laundry Dryer Controls`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/laundry-dryer-controls-server/laundry-dryer-controls-delegate.h
.. _`Laundry Washer Controls Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/laundry-washer-controls-delegate-impl.h
.. _`Laundry Washer Controls`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/laundry-washer-controls-server/laundry-washer-controls-delegate.h
.. _`Laundry Washer Mode`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/laundry-washer-mode.h
.. _`Low Power Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/low-power/LowPowerManager.h
.. _`Low Power`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/low-power-server/low-power-delegate.h
.. _`Media Input Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/media-input/MediaInputManager.h
.. _`Media Input`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/media-input-server/media-input-delegate.h
.. _`Media Playback Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/media-playback/MediaPlaybackManager.h
.. _`Media Playback`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/media-playback-server/media-playback-delegate.h
.. _`Messages Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/messages/MessagesManager.h
.. _`Messages`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/messages-server/messages-delegate.h
.. _`Microwave Oven Control Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/microwave-oven-app/microwave-oven-common/include/microwave-oven-device.h
.. _`Microwave Oven Control`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/microwave-oven-control-server/microwave-oven-control-server.h
.. _`Microwave Oven Mode`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/microwave-oven-mode.h
.. _`Mode Base`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/mode-base-server/mode-base-server.h
.. _`Mode Select Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/static-supported-modes-manager.h
.. _`Mode Select`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/mode-select-server/supported-modes-manager.h
.. _`Operational State Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/operational-state-delegate-impl.h
.. _`Operational State`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/operational-state-server/operational-state-server.h
.. _`Power Topology Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/energy-management-app/energy-management-common/energy-reporting/include/PowerTopologyDelegate.h
.. _`Power Topology`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/power-topology-server/power-topology-server.h
.. _`Push AV Stream Transport Delegate`: https://github.com/espressif/connectedhomeip/blob/faf4d09/examples/all-clusters-app/all-clusters-common/include/push-av-stream-transport-delegate-impl.h
.. _`Push AV Stream Transport`: https://github.com/espressif/connectedhomeip/blob/faf4d09/src/app/clusters/push-av-stream-transport-server/push-av-stream-transport-delegate.h
.. _`Refrigeratore And TCC Mode`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/tcc-mode.h
.. _`Resource Monitoring`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/resource-monitoring-server/resource-monitoring-server.h
.. _`Rvc Run And Rvc Clean Mode`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/rvc-modes.h
.. _`Service Area Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/rvc-app/rvc-common/include/rvc-service-area-delegate.h
.. _`Service Area`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/service-area-server/service-area-server.h
.. _`Target Navigator Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/target-navigator/TargetNavigatorManager.h
.. _`Target Navigator`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/target-navigator-server/target-navigator-delegate.h
.. _`Thermostat Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/thermostat/thermostat-common/include/thermostat-delegate-impl.h
.. _`Thermostat`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/thermostat-server/thermostat-delegate.h
.. _`Time Synchronization Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/time-synchronization-server/DefaultTimeSyncDelegate.h
.. _`Time Synchronization`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/time-synchronization-server/time-synchronization-delegate.h
.. _`Valve Configuration And Control Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/linux/ValveControlDelegate.h
.. _`Valve Configuration And Control`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/valve-configuration-and-control-server/valve-configuration-and-control-delegate.h
.. _`Wake On LAN Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/tv-app/tv-common/clusters/wake-on-lan/WakeOnLanManager.h
.. _`Wake On LAN`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/wake-on-lan-server/wake-on-lan-server.h
.. _`Water Heater Management Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/WhmDelegate.h
.. _`Water Heater Management`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/water-heater-management-server/water-heater-management-server.h
.. _`Water Heater Mode`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/all-clusters-app/all-clusters-common/include/water-heater-mode.h
.. _`WebRTC Transport Provider Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/camera-app/linux/include/clusters/webrtc_provider/webrtc-provider-manager.h
.. _`WebRTC Transport Provider`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/webrtc-transport-provider-server/webrtc-transport-provider-server.h
.. _`Window Covering Delegate`: https://github.com/espressif/connectedhomeip/blob/d144bbb/examples/chef/common/clusters/window-covering/chef-window-covering.h
.. _`Window Covering`: https://github.com/espressif/connectedhomeip/blob/d144bbb/src/app/clusters/window-covering-server/window-covering-delegate.h

.. keep-sorted end